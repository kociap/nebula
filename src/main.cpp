#include <glad/glad.h>

// GLAD must be included first. clang-format sorts includes, hence this comment
// here.

#include <anton/filesystem.hpp>
#include <anton/optional.hpp>

#include <components/camera.hpp>
#include <core/input.hpp>
#include <core/types.hpp>
#include <logging/logging.hpp>
#include <rendering/framebuffer.hpp>
#include <rendering/rendering.hpp>
#include <rendering/shader.hpp>
#include <windowing/window.hpp>

using namespace nebula;

static Handle<rendering::Shader> shader_wire;

[[nodiscard]] static Optional<String> read_file(String const& path)
{
  fs::Input_File_Stream file(path);
  if(!file.is_open()) {
    return null_optional;
  }

  file.seek(Seek_Dir::end, 0);
  i64 const uv_vert_size = file.tell();
  file.seek(Seek_Dir::beg, 0);
  String uv_vert_source{reserve, uv_vert_size};
  uv_vert_source.force_size(uv_vert_size);
  file.read(uv_vert_source.data(), uv_vert_size);
  return ANTON_MOV(uv_vert_source);
}

[[nodiscard]] static Handle<rendering::Shader>
compile_shader(String const& vertex, String const& fragment, String const& name)
{
  LOG_INFO("compiling shader '{}'", vertex);
  Optional<String> vert_source = read_file(vertex);
  if(!vert_source) {
    LOG_ERROR("could not open '{}'", vertex);
    return {};
  }

  Expected<Handle<rendering::Shader_Stage>, String> vertex_result =
    rendering::compile_shader_stage(
      vertex, rendering::Shader_Stage_Kind::vertex, vert_source.value());
  if(!vertex_result) {
    LOG_ERROR("compilation of '{}' failed\n{}", vertex, vertex_result.error());
    return {};
  }

  LOG_INFO("compiling shader '{}'", fragment);
  Optional<String> frag_source = read_file(fragment);
  if(!frag_source) {
    LOG_ERROR("could not open '{}'", fragment);
    return {};
  }

  Expected<Handle<rendering::Shader_Stage>, String> fragment_result =
    rendering::compile_shader_stage(
      fragment, rendering::Shader_Stage_Kind::fragment, frag_source.value());
  if(!fragment_result) {
    LOG_ERROR("compilation of '{}' failed\n{}", fragment,
              fragment_result.error());
    return {};
  }

  Handle<rendering::Shader> shader = rendering::create_shader(name);
  bool const vertex_attach_result =
    rendering::attach_shader_stage(shader, vertex_result.value());
  if(!vertex_attach_result) {
    LOG_ERROR("attach of '{}' to '{}' failed", vertex, name);
    return {};
  }
  bool const fragment_attach_result =
    rendering::attach_shader_stage(shader, fragment_result.value());
  if(!fragment_attach_result) {
    LOG_ERROR("attach of '{}' to '{}' failed", fragment, name);
    return {};
  }

  Expected<void, Error> link_result = rendering::link_shader(shader);
  if(!link_result) {
    LOG_ERROR("linking of '{}' failed\n{}", name, link_result.error());
    return {};
  }

  return shader;
}

static void compile_shaders()
{
  shader_wire = compile_shader(String("shaders/passthrough.vert"),
                               String("shaders/wire.frag"), String("uv"));
}

static void keyboard_callback(windowing::Window* const window, Key const key,
                              Input_State const state)
{
  ANTON_UNUSED(window);
  if(key == Key::key_r && state == Input_State::release) {
    compile_shaders();
  }
}

static void framebuffer_resize_callback(windowing::Window* const window,
                                        i64 const width, i64 const height)
{
  rendering::resize_framebuffers(width, height);
  glViewport(0, 0, width, height);
  LOG_INFO("resized framebuffer to {}x{}", width, height);
}

#define INITIALISE(fn, msg)            \
  {                                    \
    Expected<void, Error> result = fn; \
    if(!result) {                      \
      LOG_FATAL(msg, result.error());  \
      return 1;                        \
    }                                  \
  }

int main(int argc, char* argv[])
{
  (void)argc;
  (void)argv;

  windowing::Window* window = windowing::init();
  if(window == nullptr) {
    return 1;
  }

  Vec2 dim = windowing::get_framebuffer_size(window);

  i64 const width = static_cast<i64>(dim.x);
  i64 const height = static_cast<i64>(dim.y);

  // TODO: Does not deinitialise previously initialised modules on failure.
  INITIALISE(rendering::initialise(width, height),
             "initialisation of rendering failed: {}");
  INITIALISE(rendering::initialise_shaders(),
             "initialisation of shaders failed: {}");

  windowing::set_keyboard_callback(window, keyboard_callback);
  windowing::set_framebuffer_resize_callback(window,
                                             framebuffer_resize_callback);

  compile_shaders();

  rendering::bind_draw_buffers();
  rendering::bind_transient_geometry_buffers();

  Vertex triangle[] = {
    Vertex{.position = {0.2f, 0.1f, 0.0f}, .normal = {}, .uv = {1.0f, 1.0f}},
    Vertex{.position = {-0.2f, 0.1f, 0.0f}, .normal = {}, .uv = {0.0f, 1.0f}},
    Vertex{.position = {0.2f, -0.1f, 0.0f}, .normal = {}, .uv = {1.0f, 0.0f}},
    Vertex{.position = {-0.2f, -0.1f, 0.0f}, .normal = {}, .uv = {0.0f, 0.0f}},
  };
  u32 indices[] = {
    0, 1, 2, 1, 3, 2,
  };
  rendering::Draw_Elements_Command cmd =
    rendering::write_geometry(triangle, indices);
  cmd.instance_count = 1;

  glClearColor(0.0, 0.0, 0.0, 0.0);

  // Main loop
  while(!windowing::should_close(window)) {
    rendering::bind_default_framebuffer();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Camera& primary_camera = get_primary_camera();
    Vec2 const viewport_size = windowing::get_framebuffer_size(window);
    f32 const inv_aspect = viewport_size.y / viewport_size.x;
    math::Mat4 const v_mat = get_view_matrix(primary_camera);
    math::Mat4 const p_mat =
      get_projection_matrix(primary_camera, viewport_size);
    math::Mat4 const vp_mat = p_mat * v_mat;
    f32 const zoom = get_zoom(primary_camera);

    bool const bind_result = rendering::bind_shader(shader_wire);
    if(!bind_result) {
      LOG_ERROR("could not bind 'shader_wire'");
    }

    rendering::set_uniform_f32(shader_wire, "zoom", get_zoom(primary_camera));
    rendering::set_uniform_mat4(shader_wire, "vp_mat", vp_mat);

    rendering::add_draw_command(cmd);
    rendering::commit_draw();

    windowing::render_objects(window);

    windowing::swap_buffers(window);
    windowing::poll_events();
  }

  rendering::teardown_shaders();
  rendering::teardown();
  windowing::destroy(window);

  return 0;
}
