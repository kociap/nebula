#include <anton/filesystem.hpp>
#include <glad/glad.h>

// GLAD must be included first. clang-format sorts includes, hence this comment
// here.

#include <anton/optional.hpp>

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

static void compile_shaders()
{
  LOG_INFO("compiling shader 'passthrough.vert'");
  Optional<String> vert_source = read_file(String("shaders/passthrough.vert"));
  if(!vert_source) {
    LOG_ERROR("could not open 'shaders/passthrough.vert'");
    return;
  }

  Expected<Handle<rendering::Shader_Stage>, String> vertex_result =
    rendering::compile_shader_stage(String("passthrough.vert"),
                                    rendering::Shader_Stage_Kind::vertex,
                                    vert_source.value());
  if(!vertex_result) {
    LOG_ERROR("compilation of 'passthrough.vert' failed\n{}",
              vertex_result.error());
    return;
  }

  LOG_INFO("compiling shader 'wire.frag'");
  Optional<String> frag_source = read_file(String("shaders/wire.frag"));
  if(!frag_source) {
    LOG_ERROR("could not open 'shaders/wire.frag'");
    return;
  }

  Expected<Handle<rendering::Shader_Stage>, String> fragment_result =
    rendering::compile_shader_stage(String("wire.frag"),
                                    rendering::Shader_Stage_Kind::fragment,
                                    frag_source.value());
  if(!fragment_result) {
    LOG_ERROR("compilation of 'wire.frag' failed\n{}", fragment_result.error());
    return;
  }

  Handle<rendering::Shader> shader = rendering::create_shader(String("uv"));
  bool const vertex_attach_result =
    rendering::attach_shader_stage(shader, vertex_result.value());
  if(!vertex_attach_result) {
    LOG_ERROR("vertex attach failed");
    return;
  }
  bool const fragment_attach_result =
    rendering::attach_shader_stage(shader, fragment_result.value());
  if(!fragment_attach_result) {
    LOG_ERROR("fragment attach failed");
    return;
  }

  Expected<void, Error> link_result = rendering::link_shader(shader);
  if(!link_result) {
    LOG_ERROR("linking of uv shader failed\n{}", link_result.error());
    return;
  }

  shader_wire = shader;
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
  ANTON_UNUSED(window);
  LOG_INFO("resized framebuffer to {}x{}", width, height);
  rendering::resize_framebuffers(width, height);
  glViewport(0, 0, width, height);
}

struct Shutdown_Guard {
private:
  windowing::Window* window = nullptr;

public:
  Shutdown_Guard(windowing::Window* window): window(window) {}

  ~Shutdown_Guard()
  {
    rendering::teardown();
    rendering::teardown_shaders();
    windowing::destroy(window);
  }
};

int main(int argc, char* argv[])
{
  (void)argc;
  (void)argv;

  windowing::Window* window = windowing::init();
  if(window == nullptr) {
    return 1;
  }

  Vec2 dim = windowing::get_dimensions(window);

  i64 const width = static_cast<i64>(dim.x);
  i64 const height = static_cast<i64>(dim.y);

  {
    Expected<void, Error> result = rendering::initialise(width, height);
    if(!result) {
      LOG_FATAL("rendering initialisation failed: {}", result.error());
      return 1;
    }
  }

  Shutdown_Guard shutdown_guard(window);

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

    bool const bind_result = rendering::bind_shader(shader_wire);
    if(!bind_result) {
      LOG_ERROR("could not bind 'shader_wire'");
    }

    rendering::add_draw_command(cmd);
    rendering::commit_draw();

    windowing::setup_camera_projection(window, shader_wire);
    windowing::render_objects(window);

    windowing::swap_buffers(window);
    windowing::poll_events();
  }

  return 0;
}
