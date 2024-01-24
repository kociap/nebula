#include <rendering/rendering.hpp>

#include <glad/glad.h>

#include <anton/flat_hash_map.hpp>
#include <anton/intrinsics.hpp>
#include <anton/math/vec2.hpp>
#include <anton/math/vec3.hpp>
#include <anton/math/vec4.hpp>
#include <anton/string.hpp>

#include <core/types.hpp>
#include <logging/logging.hpp>
#include <rendering/opengl.hpp>

namespace nebula::rendering {
  struct Texture_Format {
    u32 width;
    u32 height;
    u32 sized_internal_format;
    u32 pixel_format;
    u32 pixel_type;
    u32 filter;
    i32 mip_levels;
    i32 swizzle_mask[4];
  };

  [[nodiscard]] static bool operator==(Texture_Format const& lhs,
                                       Texture_Format const& rhs)
  {
    i32 const result = memcmp(&lhs, &rhs, sizeof(Texture_Format));
    return result == 0;
  }

  [[nodiscard]] static bool operator!=(Texture_Format const& lhs,
                                       Texture_Format const& rhs)
  {
    return !(lhs == rhs);
  }

  struct GPU_Buffer {
    u32 handle;
    void* mapped;
    i64 size;
  };

  static GPU_Buffer gpu_vertex_buffer;
  static GPU_Buffer gpu_element_buffer;
  static GPU_Buffer gpu_draw_cmd_buffer;
  static Flat_Hash_Map<u64, Draw_Elements_Command> persistent_draw_commands_map;
  static Array<Draw_Elements_Command> draw_cmds;
  static Buffer<Vertex> vertex_buffer;
  static Buffer<u32> element_buffer;
  static Buffer<Draw_Elements_Command> draw_cmd_buffer;
  static Framebuffer primary_fb;
  static Framebuffer front_postprocess_fb;
  static Framebuffer back_postprocess_fb;

  [[nodiscard]] static GPU_Buffer create_gpu_buffer(i64 const size,
                                                    u32 const flags)
  {
    GPU_Buffer buffer;
    buffer.size = size;
    glCreateBuffers(1, &buffer.handle);
    glNamedBufferStorage(buffer.handle, size, nullptr, flags);
    // Remove all bits except for WRITE_BIT, READ_BIT, PERSISTENT_BIT and
    // COHERENT_BIT from flags.
    constexpr u32 flags_mask = GL_MAP_WRITE_BIT | GL_MAP_READ_BIT |
                               GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
    buffer.mapped =
      glMapNamedBufferRange(buffer.handle, 0, buffer.size, flags & flags_mask);
    return buffer;
  }

  static void create_buffers()
  {
    constexpr u32 buffer_flags = GL_DYNAMIC_STORAGE_BIT | GL_MAP_COHERENT_BIT |
                                 GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT;
    // 1048576 = 1024^2
    gpu_vertex_buffer =
      create_gpu_buffer(1048576 * sizeof(math::Vec4), buffer_flags);
    gpu_element_buffer = create_gpu_buffer(1048576 * sizeof(u32), buffer_flags);
    gpu_draw_cmd_buffer =
      create_gpu_buffer(8 * 1024 * sizeof(Draw_Elements_Command), buffer_flags);

    vertex_buffer.buffer = reinterpret_cast<Vertex*>(gpu_vertex_buffer.mapped);
    vertex_buffer.head = vertex_buffer.buffer;
    vertex_buffer.end =
      vertex_buffer.buffer + gpu_vertex_buffer.size / sizeof(Vertex);
    element_buffer.buffer = reinterpret_cast<u32*>(gpu_element_buffer.mapped);
    element_buffer.head = element_buffer.buffer;
    element_buffer.end =
      element_buffer.buffer + gpu_element_buffer.size / sizeof(u32);
    draw_cmd_buffer.buffer =
      reinterpret_cast<Draw_Elements_Command*>(gpu_draw_cmd_buffer.mapped);
    draw_cmd_buffer.head = draw_cmd_buffer.buffer;
    draw_cmd_buffer.end =
      draw_cmd_buffer.buffer +
      gpu_draw_cmd_buffer.size / sizeof(Draw_Elements_Command);
  }

  [[nodiscard]] static Expected<void, Error>
  create_framebuffers(i64 const width, i64 const height)
  {
    Framebuffer::Construct_Info primary_info;
    primary_info.name = "primary"_sv;
    primary_info.width = width;
    primary_info.height = height;
    primary_info.depth_buffer.enabled = true;
    primary_info.depth_buffer.buffer_type =
      Framebuffer::Buffer_Type::renderbuffer;
    primary_info.color_buffers.resize(1);
    // Albedo
    primary_info.color_buffers[0].internal_format =
      Framebuffer::Internal_Format::rgba8;
    Expected<void, Error> result_primary =
      primary_fb.create_framebuffer(primary_info);
    if(!result_primary) {
      return result_primary;
    }

    Framebuffer::Construct_Info postprocess_info;
    postprocess_info.name = "postprocess back"_sv;
    postprocess_info.color_buffers.resize(1);
    postprocess_info.width = width;
    postprocess_info.height = height;
    Expected<void, Error> result_back =
      back_postprocess_fb.create_framebuffer(postprocess_info);
    if(!result_back) {
      return result_back;
    }

    postprocess_info.name = "postprocess front";
    Expected<void, Error> result_front =
      front_postprocess_fb.create_framebuffer(postprocess_info);
    if(!result_front) {
      return result_front;
    }

    return expected_value;
  }

  static void destroy_framebuffers()
  {
    back_postprocess_fb.delete_framebuffer();
    front_postprocess_fb.delete_framebuffer();
    primary_fb.delete_framebuffer();
  }

  Expected<void, Error> initialise(i64 const width, i64 const height)
  {
    bool const glad_initialised = gladLoadGL();
    if(!glad_initialised) {
      return {expected_error, Error("failed to initialise GLAD")};
    }

    opengl::install_debug_callback();
    opengl::load_constants();

    glDisable(GL_FRAMEBUFFER_SRGB);
    glClipControl(GL_UPPER_LEFT, GL_NEGATIVE_ONE_TO_ONE);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0, 0, 0, 1);

    create_buffers();

    Expected<void, Error> result = create_framebuffers(width, height);
    if(!result) {
      return result;
    }

    return expected_value;
  }

  void teardown()
  {
    draw_cmds.reset();
    destroy_framebuffers();
  }

  Framebuffer* get_primary_framebuffer()
  {
    return &primary_fb;
  }

  Framebuffer* get_front_postprocess_framebuffer()
  {
    return &front_postprocess_fb;
  }

  Framebuffer* get_back_postprocess_framebuffer()
  {
    return &back_postprocess_fb;
  }

  void swap_postprocess_framebuffers()
  {
    swap(front_postprocess_fb, back_postprocess_fb);
  }

  void resize_framebuffers(i64 const width, i64 const height)
  {
    // TODO: This should be a transaction as creating framebuffers might fail
    //       and we'll be left without framebuffers at all.
    destroy_framebuffers();
    Expected<void, Error> result = create_framebuffers(width, height);
    ANTON_UNUSED(result);
  }

  void bind_draw_buffers()
  {
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, gpu_draw_cmd_buffer.handle);
  }

  void bind_transient_geometry_buffers()
  {
    glBindBufferRange(GL_SHADER_STORAGE_BUFFER, vertex_buffer_binding,
                      gpu_vertex_buffer.handle, 0, gpu_vertex_buffer.size);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gpu_element_buffer.handle);
  }

  Draw_Elements_Command write_geometry(Slice<Vertex const> const vertices,
                                       Slice<u32 const> const indices)
  {
    Draw_Elements_Command cmd = {};
    if(vertex_buffer.end - vertex_buffer.head < vertices.size()) {
      vertex_buffer.head = vertex_buffer.buffer;
    }
    memcpy(vertex_buffer.head, vertices.data(),
           vertices.size() * sizeof(Vertex));
    cmd.base_vertex = vertex_buffer.head - vertex_buffer.buffer;
    vertex_buffer.head += vertices.size();

    if(element_buffer.end - element_buffer.head < indices.size()) {
      element_buffer.head = element_buffer.buffer;
    }
    memcpy(element_buffer.head, indices.data(), indices.size() * sizeof(u32));
    cmd.first_index = element_buffer.head - element_buffer.buffer;
    element_buffer.head += indices.size();
    cmd.count = indices.size();
    return cmd;
  }

  void add_draw_command(Draw_Elements_Command const command)
  {
    draw_cmds.push_back(command);
  }

  void add_draw_command(Draw_Persistent_Geometry_Command const cmd)
  {
    auto iter = persistent_draw_commands_map.find(cmd.handle);
    if(iter == persistent_draw_commands_map.end()) {
      LOG_WARNING("persistent draw command ID={} not found. ignoring draw.",
                  cmd.handle);
      return;
    }

    Draw_Elements_Command draw_cmd = iter->value;
    draw_cmd.base_instance = cmd.base_instance;
    draw_cmd.instance_count = cmd.instance_count;
    add_draw_command(draw_cmd);
  }

  void commit_draw()
  {
    if(draw_cmds.size() <= 0) {
      return;
    }

    i64 const remaining_space = draw_cmd_buffer.end - draw_cmd_buffer.head;
    if(draw_cmds.size() > remaining_space) {
      draw_cmd_buffer.head = draw_cmd_buffer.buffer;
    }

    memcpy(draw_cmd_buffer.head, draw_cmds.data(),
           draw_cmds.size() * sizeof(Draw_Elements_Command));
    i64 const offset = (draw_cmd_buffer.head - draw_cmd_buffer.buffer) *
                       sizeof(Draw_Elements_Command);
    glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (void*)offset,
                                draw_cmds.size(),
                                sizeof(Draw_Elements_Command));
    draw_cmd_buffer.head += draw_cmds.size();
    draw_cmds.clear();
  }

  void render_texture_quad()
  {
    struct Vertex {
      math::Vec3 position;
      math::Vec2 uv;
    };

    // 6 vertices plane (2 triangles) facing Z axis.
    Vertex vertices[] = {
      Vertex({-1, 1, 0}, {0, 1}), Vertex({-1, -1, 0}, {0, 0}),
      Vertex({1, -1, 0}, {1, 0}), Vertex({-1, 1, 0}, {0, 1}),
      Vertex({1, -1, 0}, {1, 0}), Vertex({1, 1, 0}, {1, 1})};

    // TODO: Upload vertices to a buffer.

    // glBindVertexBuffer(0, vbo, 0, sizeof(Vertex));
    // glDrawArrays(GL_TRIANGLES, 0, 6);
  }
} // namespace nebula::rendering
