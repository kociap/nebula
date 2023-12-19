#pragma once

#include <anton/expected.hpp>
#include <anton/slice.hpp>

#include <core/error.hpp>
#include <core/types.hpp>
#include <rendering/framebuffer.hpp>
#include <rendering/vertex.hpp>

namespace nebula::rendering {
  struct Draw_Elements_Command {
    u32 count;
    u32 instance_count;
    u32 first_index;
    u32 base_vertex;
    u32 base_instance;
  };

  struct Draw_Persistent_Geometry_Command {
    u32 handle;
    u32 instance_count;
    u32 base_instance;
  };

  template<typename T>
  struct Buffer {
    T* buffer;
    T* head;
    T* end;
  };

  Expected<void, Error> initialise(i64 width, i64 height);
  void teardown();

  [[nodiscard]] Framebuffer* get_primary_framebuffer();
  [[nodiscard]] Framebuffer* get_front_postprocess_framebuffer();
  [[nodiscard]] Framebuffer* get_back_postprocess_framebuffer();
  void swap_postprocess_framebuffers();
  void resize_framebuffers(i64 width, i64 height);

  // Binding numbers of various buffers.

  // The binding number of the buffers containing vertex (geometry) data.
  constexpr u32 vertex_buffer_binding = 1;

  void bind_draw_buffers();
  void bind_transient_geometry_buffers();

  // write_geometry
  // Write indexed vertex data (geometry) to the gpu buffers. The underlying
  // storage works as a ring buffer, hence geometry will eventually be
  // overwritten.
  //
  [[nodiscard]] Draw_Elements_Command write_geometry(Slice<Vertex const>,
                                                     Slice<u32 const>);

  // // Write matrices and materials to gpu buffers. Data will eventually be overwritten.
  // // Returns draw_id offset to be used as base_instance in draw commands.
  // [[nodiscard]] u32 write_matrices_and_materials(Slice<Matrix4 const>,
  //                                                Slice<Material const>);

  // // Write geometry that will persist across multiple frames. Geometry will not be overwritten.
  // // Returns: Handle to the persistent geometry.
  // [[nodiscard]] u64 write_persistent_geometry(Slice<Vertex const>,
  //                                             Slice<u32 const>);

  // // Loads base texture and generates mipmaps (since we don't have pregenerated mipmaps yet).
  // // pixels is a pointer to an array of pointers to the pixel data.
  // // handles (out) array of handles to the textures. Must be at least texture_count big.
  // // handle <internal texture index (u32), layer (f32)>
  // void load_textures_generate_mipmaps(Texture_Format, i32 texture_count,
  //                                     void const* const* pixels,
  //                                     Texture* handles);

  // handle <internal texture index (u32), layer (f32)>
  // The handle is translated to gl texture handle and then bound.
  // Since textures are stored as array textures, textures with same index do not have to
  //   be bound multiple times.
  // void bind_texture(u32 unit, Texture handle);

  // void unload_texture(u64 handle);
  // void unload_textures(i32 handle_count, u64 const* handles);

  void add_draw_command(Draw_Elements_Command);
  void add_draw_command(Draw_Persistent_Geometry_Command);
  void commit_draw();

  // // objects - snapshot of ecs containing Static_Mesh_Components and Transforms
  // void render_scene(ECS objects, Transform camera_transform, Matrix4 view,
  //                   Matrix4 projection);

  // Render a quad taking up the whole viewport
  // Intended for rendering textures to the screen or applying postprocessing effects
  // Rebinds element buffer and vertex buffer with binding index 0
  void render_texture_quad();
} // namespace nebula::rendering
