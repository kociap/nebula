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
#include <shaders/compiler.hpp>
#include <ui/scene.hpp>
#include <ui/ui.hpp>
#include <windowing/window.hpp>

using namespace nebula;

static Handle<rendering::Shader> shader_wire;
static Handle<rendering::Shader> shader_grid;

static void compile_shaders()
{
  shader_wire = compile_shader(String("shaders/passthrough.vert"),
                               String("shaders/wire.frag"), String("uv"));
  shader_grid = compile_shader(String("shaders/grid.vert"),
                               String("shaders/grid.frag"), String("grid"));
}

struct Globals {
  UI ui;
  Scene scene;
};

static void keyboard_callback(windowing::Window* const window, Key const key,
                              Input_Action const state, void* data)
{
  ANTON_UNUSED(window);
  ANTON_UNUSED(data);
  if(key == Key::key_r && state == Input_Action::release) {
    compile_shaders();
  }
}

static void framebuffer_resize_callback(windowing::Window* const window,
                                        i64 const width, i64 const height,
                                        void* data)
{
  ANTON_UNUSED(window);
  ANTON_UNUSED(data);
  rendering::resize_framebuffers(width, height);
  glViewport(0, 0, width, height);
  LOG_INFO("resized framebuffer to {}x{}", width, height);
}

static void mouse_button_callback(windowing::Window* const window,
                                  Key const key, Input_Action const action,
                                  void* data)
{
  ANTON_UNUSED(window);
  Globals* const globals = reinterpret_cast<Globals*>(data);
  if(key == Key::mouse_left) {
    if(action == Input_Action::press) {
      Vec2 const cursor_position = windowing::get_cursor_position(window);

      Camera& cam = get_primary_camera();
      Vec2 const viewport_size = get_framebuffer_size(window);
      Vec2 const window_size = get_window_size(window);
      Vec2 const scene_position = cam.window_to_scene_position(
        cursor_position, window_size, viewport_size);

      globals->scene.currently_moved_gate =
        globals->ui.check_if_gate_clicked(scene_position);
      globals->scene.last_mouse_position = scene_position;
      if(globals->scene.currently_moved_gate != nullptr) {
        globals->scene.mode = Window_Mode::gate_moving;
        return;
      }

      globals->scene.connected_port =
        globals->ui.check_if_port_clicked(scene_position);
      if(globals->scene.connected_port != nullptr) {
        globals->scene.mode = Window_Mode::port_connecting;
        return;
      }

      globals->scene.mode = Window_Mode::camera_moving;
      globals->scene.last_mouse_position = scene_position;
    } else if(action == Input_Action::release) {
      globals->scene.currently_moved_gate = nullptr;
      globals->scene.connected_port = nullptr;
      globals->scene.mode = Window_Mode::none;
    }
  }
}

static void cursor_position_callback(windowing::Window* const window,
                                     f32 const x, f32 const y, void* data)
{
  ANTON_UNUSED(window);
  Globals* const globals = reinterpret_cast<Globals*>(data);
  Camera& cam = get_primary_camera();
  Vec2 const window_position{x, y};
  Vec2 const viewport_size = get_framebuffer_size(window);
  Vec2 const window_size = get_window_size(window);
  Vec2 const scene_position =
    cam.window_to_scene_position(window_position, window_size, viewport_size);

  math::Vec2 offset;
  offset.x = scene_position.x - globals->scene.last_mouse_position.x;
  offset.y = scene_position.y - globals->scene.last_mouse_position.y;

  if(globals->scene.mode == Window_Mode::camera_moving) {
    cam.move(offset);
  } else if(globals->scene.mode == Window_Mode::gate_moving) {
    globals->scene.currently_moved_gate->move(offset);
  } else if(globals->scene.mode == Window_Mode::port_connecting) {
    // TODO: Implement port connecting
  }
  globals->scene.last_mouse_position.x = scene_position.x;
  globals->scene.last_mouse_position.y = scene_position.y;
}

static void scroll_callback(windowing::Window* const window, f32 const dx,
                            f32 const dy, void* data)
{
  ANTON_UNUSED(window);
  ANTON_UNUSED(data);
  Camera& primary_camera = get_primary_camera();
  if(dy < 0.0) {
    primary_camera.zoom(1.25);
  } else {
    primary_camera.zoom(0.8);
  }
  LOG_DEBUG("zoom changed: {}", primary_camera.zoom_level);
}

static void render_grid(math::Mat4 const v_mat, f32 const inv_aspect,
                        f32 const zoom)
{
  Vertex fsq[] = {
    Vertex{.position = {-1.0f, 1.0f, 0.0f}, .normal = {}, .uv = {0.0f, 1.0f}},
    Vertex{.position = {-1.0f, -1.0f, 0.0f}, .normal = {}, .uv = {0.0f, 0.0f}},
    Vertex{.position = {1.0, 1.0f, 0.0f}, .normal = {}, .uv = {1.0f, 1.0f}},
    Vertex{.position = {1.0f, -1.0f, 0.0f}, .normal = {}, .uv = {1.0f, 0.0f}},
  };
  u32 indices[] = {
    0, 1, 2, 1, 3, 2,
  };
  rendering::Draw_Elements_Command cmd =
    rendering::write_geometry(fsq, indices);
  cmd.instance_count = 1;
  rendering::add_draw_command(cmd);

  rendering::bind_shader(shader_grid);
  rendering::set_uniform_mat4(shader_grid, "v_mat", v_mat);
  rendering::set_uniform_f32(shader_grid, "inv_aspect", inv_aspect);
  rendering::set_uniform_f32(shader_grid, "zoom", zoom);
  rendering::commit_draw();
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

  Globals globals;

  windowing::set_keyboard_callback(window, keyboard_callback, &globals);
  windowing::set_scroll_callback(window, scroll_callback, &globals);
  windowing::set_framebuffer_resize_callback(
    window, framebuffer_resize_callback, &globals);
  windowing::set_cursor_position_callback(window, cursor_position_callback,
                                          &globals);
  windowing::set_mouse_button_callback(window, mouse_button_callback, &globals);

  compile_shaders();

  rendering::bind_draw_buffers();
  rendering::bind_transient_geometry_buffers();

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
    f32 const zoom_level = get_zoom(primary_camera);

    render_grid(v_mat, inv_aspect, zoom_level);

    bool const bind_result = rendering::bind_shader(shader_wire);
    if(!bind_result) {
      LOG_ERROR("could not bind 'shader_wire'");
    }

    rendering::set_uniform_f32(shader_wire, "zoom_level", zoom_level);
    rendering::set_uniform_mat4(shader_wire, "vp_mat", vp_mat);

    globals.ui.add_gates_to_render_loop();

    rendering::commit_draw();

    windowing::swap_buffers(window);
    windowing::poll_events();
  }

  rendering::teardown_shaders();
  rendering::teardown();
  windowing::destroy(window);

  return 0;
}
