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
#include <windowing/window.hpp>

using namespace nebula;

static Handle<rendering::Shader> shader_default;
static Handle<rendering::Shader> shader_port;
static Handle<rendering::Shader> shader_grid;

static void compile_shaders()
{
  shader_default =
    compile_shader(String("shaders/passthrough.vert"),
                   String("shaders/default.frag"), String("default"));
  shader_grid = compile_shader(String("shaders/grid.vert"),
                               String("shaders/grid.frag"), String("grid"));
  shader_port = compile_shader(String("shaders/passthrough.vert"),
                               String("shaders/port.frag"), String("port"));
}

static void keyboard_callback(windowing::Window* const window, Key const key,
                              Input_Action const state, void* data)
{
  auto& scene = *reinterpret_cast<Scene*>(data);
  ANTON_UNUSED(window);
  ANTON_UNUSED(data);
  if(key == Key::key_r && state == Input_Action::release) {
    compile_shaders();
  } else if((key == Key::key_left_control || key == Key::key_right_control) &&
            state == Input_Action::press) {
    if(scene.mode == Window_Mode::none) {
      scene.mode = Window_Mode::object_delete;
    }
  } else if((key == Key::key_left_control || key == Key::key_right_control) &&
            state == Input_Action::release) {
    if(scene.mode == Window_Mode::object_delete) {
      scene.mode = Window_Mode::none;
    }
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
  auto& scene = *reinterpret_cast<Scene*>(data);
  if(key == Key::mouse_left) {
    if(action == Input_Action::press) {
      Vec2 const cursor_position = windowing::get_cursor_position(window);

      Camera& cam = get_primary_camera();
      Vec2 const viewport_size = get_framebuffer_size(window);
      Vec2 const window_size = get_window_size(window);
      Vec2 const scene_position = cam.window_to_scene_position(
        cursor_position, window_size, viewport_size);

      // Check if connected to another port
      if(scene.mode == Window_Mode::port_linking) {
        Port* p = scene.check_if_port_clicked(scene_position);
        // Can't connect to the same port
        if(p == nullptr || p->type == scene.connected_port->type) {
          scene.remove_tmp_port(scene.connected_port);
        } else {
          scene.connect_ports(scene.connected_port, p);
        }
        // Quit connecting mode
        scene.connected_port = nullptr;
        scene.mode = Window_Mode::none;
        return;
      }

      scene.connected_port = scene.check_if_port_clicked(scene_position);
      if(scene.connected_port != nullptr) {
        if(scene.mode == Window_Mode::object_delete) {
          scene.connected_port->remove_all_connections();
          return;
        }

        scene.mode = Window_Mode::port_linking;
        Port_Kind tmp_port_type;
        if(scene.connected_port->type == Port_Kind::in) {
          tmp_port_type = Port_Kind::out;
        } else {
          tmp_port_type = Port_Kind::in;
        }
        scene.create_tmp_port(scene.connected_port, scene_position,
                              tmp_port_type);
        return;
      }

      scene.currently_moved_gate = scene.check_if_gate_clicked(scene_position);
      scene.last_mouse_position = scene_position;
      if(scene.currently_moved_gate != nullptr) {
        if(scene.mode == Window_Mode::object_delete) {
          scene.delete_gate(scene.currently_moved_gate);
          return;
        }
        scene.mode = Window_Mode::gate_moving;
        return;
      }

      scene.mode = Window_Mode::camera_moving;
      scene.last_mouse_position = scene_position;
    } else if(action == Input_Action::release) {
      if(scene.mode == Window_Mode::port_linking) {
        return;
      }

      scene.currently_moved_gate = nullptr;
      scene.connected_port = nullptr;
      scene.mode = Window_Mode::none;
    }
  } else if(key == Key::mouse_right) {
    Vec2 const cursor_position = windowing::get_cursor_position(window);

    Camera& cam = get_primary_camera();
    Vec2 const viewport_size = get_framebuffer_size(window);
    Vec2 const window_size = get_window_size(window);
    Vec2 const scene_position =
      cam.window_to_scene_position(cursor_position, window_size, viewport_size);
    Port* p = scene.check_if_port_clicked(scene_position);
    if(p != nullptr) {
      p->remove_all_connections();
    }
  }
}

static void cursor_position_callback(windowing::Window* const window,
                                     f32 const x, f32 const y, void* data)
{
  ANTON_UNUSED(window);
  Scene& scene = *reinterpret_cast<Scene*>(data);
  Camera& cam = get_primary_camera();
  Vec2 const window_position{x, y};
  Vec2 const viewport_size = get_framebuffer_size(window);
  Vec2 const window_size = get_window_size(window);
  Vec2 const scene_position =
    cam.window_to_scene_position(window_position, window_size, viewport_size);

  math::Vec2 offset;
  offset.x = scene_position.x - scene.last_mouse_position.x;
  offset.y = scene_position.y - scene.last_mouse_position.y;

  if(scene.mode == Window_Mode::port_linking) {
    scene.move_tmp_port(offset);
  } else if(scene.mode == Window_Mode::camera_moving) {
    cam.move(offset);
  } else if(scene.mode == Window_Mode::gate_moving) {
    scene.currently_moved_gate->move(offset);
  }

  scene.last_mouse_position.x = scene_position.x;
  scene.last_mouse_position.y = scene_position.y;
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

  Scene scene;

  windowing::set_keyboard_callback(window, keyboard_callback, &scene);
  windowing::set_scroll_callback(window, scroll_callback, &scene);
  windowing::set_framebuffer_resize_callback(
    window, framebuffer_resize_callback, &scene);
  windowing::set_cursor_position_callback(window, cursor_position_callback,
                                          &scene);
  windowing::set_mouse_button_callback(window, mouse_button_callback, &scene);

  compile_shaders();

  rendering::bind_draw_buffers();
  rendering::bind_transient_geometry_buffers();

  glClearColor(0.0, 0.0, 0.0, 0.0);

  scene.add_gate({1.5f, 1.5f}, {1.0f, 0.0f}, Gate_Kind::e_and);
  scene.add_gate({1.5f, 1.5f}, {-1.0f, 0.0f}, Gate_Kind::e_not);

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

    // render_grid(v_mat, inv_aspect, zoom_level);

    {
      bool const bind_result = rendering::bind_shader(shader_default);
      if(!bind_result) {
        LOG_ERROR("could not bind 'shader_default'");
        continue;
      }
    }

    rendering::set_uniform_mat4(shader_default, "vp_mat", vp_mat);

    // Draw order:
    // 1. gates.
    // 2. connections.
    // 3. ports.

    for(Gate const& gate: scene.gates) {
      rendering::Draw_Elements_Command cmd = prepare_draw(gate);
      rendering::add_draw_command(cmd);
    }

    for(Port const* const port: scene.ports) {
      if(port->type != Port_Kind::out) {
        continue;
      }

      for(Port const* const conn: port->connections) {
        rendering::Draw_Elements_Command cmd =
          prepare_draw_connection(port->coordinates, conn->coordinates);
        rendering::add_draw_command(cmd);
      }
    }
    rendering::commit_draw();

    {
      bool const bind_result = rendering::bind_shader(shader_port);
      if(!bind_result) {
        LOG_ERROR("could not bind 'shader_port'");
        continue;
      }
    }

    rendering::set_uniform_mat4(shader_port, "vp_mat", vp_mat);

    for(Port const* const port: scene.ports) {
      rendering::Draw_Elements_Command cmd = prepare_draw(*port);
      rendering::add_draw_command(cmd);
    }
    rendering::commit_draw();

    windowing::swap_buffers(window);
    windowing::poll_events();
  }

  rendering::teardown_shaders();
  rendering::teardown();
  windowing::destroy(window);

  return 0;
}
