#include <glad/glad.h>

// GLAD must be included first. clang-format sorts includes, hence this comment
// here.

#include <anton/filesystem.hpp>
#include <anton/optional.hpp>

#include <components/camera.hpp>
#include <core/input.hpp>
#include <core/types.hpp>
#include <evaluator/evaluator.hpp>
#include <logging/logging.hpp>
#include <rendering/framebuffer.hpp>
#include <rendering/rendering.hpp>
#include <rendering/shader.hpp>
#include <shaders/compiler.hpp>
#include <ui/scene.hpp>
#include <ui/viewport.hpp>
#include <windowing/window.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

using namespace nebula;

namespace {
  bool is_draged_from_menu = false;
  Gate_Kind last_menu_gate_choice = Gate_Kind::e_count;
  bool run_evaluation = false;
  bool single_step_evaluation = false;
  i64 evaluation_frequency = 1; // TODO: Frequency switching button (1,2,4,8,16)
  i64 frame_counter = 0;
  Vec2 const gate_default_size{0.6f, 0.5f};
  Vec2 viewport_position;
  Vec2 viewport_size;
} // namespace

[[nodiscard]] static bool is_within_viewport(Vec2 const point)
{
  Vec2 const p = point - viewport_position;
  return p.x >= 0 && p.y >= 0 && p.x <= viewport_size.x &&
         p.y <= viewport_size.y;
}

static void initialise_imgui(windowing::Window* const window)
{
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  GLFWwindow* glfw_handle =
    reinterpret_cast<GLFWwindow*>(get_native_handle(window));
  ImGui_ImplGlfw_InitForOpenGL(glfw_handle, true);
  ImGui_ImplOpenGL3_Init();
}

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
  } else if((key == Key::key_lctrl || key == Key::key_rctrl) &&
            state == Input_Action::press) {
    if(scene.mode == Window_Mode::none) {
      // scene.set_window_mode(Window_Mode::object_delete);
    }
  } else if((key == Key::key_lctrl || key == Key::key_rctrl) &&
            state == Input_Action::release) {
    // if(scene.mode == Window_Mode::object_delete) {
    //   scene.set_window_mode(Window_Mode::none);
    // }
  }
}

static void framebuffer_resize_callback(windowing::Window* const window,
                                        i64 const width, i64 const height,
                                        void* data)
{
  ANTON_UNUSED(window);
  ANTON_UNUSED(data);
  LOG_INFO("resized window to {}x{}", width, height);
}

[[nodiscard]] static Vec2 unproject_point(Vec2 const cursor_pos)
{
  Vec2 const vp_cursor_pos = {cursor_pos.x - viewport_position.x,
                              viewport_size.y -
                                (cursor_pos.y - viewport_position.y)};
  Camera& camera = get_primary_camera();
  math::Mat4 const inv_view = math::inverse(get_view_matrix(camera));
  math::Mat4 const inv_projection =
    math::inverse(get_projection_matrix(camera, viewport_size));
  Vec2 const scene_position =
    unproject_viewport(vp_cursor_pos, inv_projection, inv_view, viewport_size);
  return scene_position;
}

static void mouse_button_callback(windowing::Window* const window,
                                  Key const key, Input_Action const action,
                                  void* data)
{
  ANTON_UNUSED(window);
  auto& scene = *reinterpret_cast<Scene*>(data);

  switch(scene.mode) {
  case Window_Mode::none: {
    Vec2 const cursor_pos = windowing::get_cursor_position(window);
    if(!is_within_viewport(cursor_pos)) {
      return;
    }

    Vec2 const scene_position = unproject_point(cursor_pos);
    scene.last_mouse_position = scene_position;

    if(action == Input_Action::press && key == Key::mouse_left) {
      // Allow modification of the system only when not running evaluation.
      if(!run_evaluation) {
        Port* const port = test_hit_ports(scene, scene_position);
        if(port != nullptr) {
          Input_Action const lctrl = windowing::get_key(window, Key::key_lctrl);
          // LCTRL + LMB deletes connections.
          if(lctrl == Input_Action::press) {
            port->remove_all_connections();
            return;
          }

          scene.set_window_mode(Window_Mode::port_linking);
          Port_Kind const tmp_port_kind = invert_port_kind(port->kind);
          scene.create_tmp_port(port, scene_position, tmp_port_kind);
          scene.connected_port = port;
          return;
        }
      }

      Gate* const gate = test_hit_gates(scene, scene_position);
      if(gate != nullptr) {
        Input_Action const lctrl = windowing::get_key(window, Key::key_lctrl);
        // LCTRL + LMB deletes gates.
        if(lctrl == Input_Action::press) {
          // TODO: SEGFAULT on delete.
          // scene.delete_gate(gate);
          return;
        }

        scene.currently_moved_gate = gate;
        scene.set_window_mode(Window_Mode::gate_moving);
        return;
      }

      // Move camera regardless of where we click.
      scene.set_window_mode(Window_Mode::camera_moving);
    } else if(action == Input_Action::press && key == Key::mouse_right) {
      Gate* const g = test_hit_gates(scene, scene_position);
      if(g != nullptr && g->kind == Gate_Kind::e_input) {
        g->evaluation = {!g->evaluation.prev_value, !g->evaluation.value};
        return;
      }
    }
  } break;

  case Window_Mode::port_linking: {
    Vec2 const cursor_pos = windowing::get_cursor_position(window);
    Vec2 const scene_position = unproject_point(cursor_pos);
    Port* const p = test_hit_ports(scene, scene_position);
    // Can't connect to the same port
    if(p == nullptr || p->kind == scene.connected_port->kind) {
      scene.remove_tmp_port(scene.connected_port);
    } else {
      scene.connect_ports(scene.connected_port, p);
    }
    // Quit connecting mode
    scene.connected_port = nullptr;
    scene.set_window_mode(Window_Mode::none);
  } break;

  case Window_Mode::gate_moving:
  case Window_Mode::camera_moving: {
    if(action == Input_Action::release) {
      scene.currently_moved_gate = nullptr;
      scene.connected_port = nullptr;
      scene.set_window_mode(Window_Mode::none);
    }
  } break;
  }
}

static void cursor_position_callback(windowing::Window* const window,
                                     f32 const x, f32 const y, void* data)
{
  ANTON_UNUSED(window);
  Scene& scene = *reinterpret_cast<Scene*>(data);
  Vec2 const cursor_pos{x, y};
  Vec2 const scene_position = unproject_point(cursor_pos);
  math::Vec2 const offset = scene_position - scene.last_mouse_position;
  if(scene.mode == Window_Mode::port_linking) {
    scene.move_tmp_port(offset);
  } else if(scene.mode == Window_Mode::camera_moving) {
    Camera& camera = get_primary_camera();
    camera.move(offset);
  } else if(scene.mode == Window_Mode::gate_moving) {
    scene.currently_moved_gate->move(offset);
  }

  scene.last_mouse_position = scene_position;
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

static void render_scene(Scene& scene, Vec2 const viewport_size)
{
  Camera& primary_camera = get_primary_camera();
  f32 const inv_aspect = viewport_size.y / viewport_size.x;
  math::Mat4 const v_mat = get_view_matrix(primary_camera);
  math::Mat4 const p_mat = get_projection_matrix(primary_camera, viewport_size);
  math::Mat4 const vp_mat = p_mat * v_mat;
  f32 const zoom_level = get_zoom(primary_camera);

  // render_grid(v_mat, inv_aspect, zoom_level);

  {
    bool const bind_result = rendering::bind_shader(shader_default);
    if(!bind_result) {
      LOG_ERROR("could not bind 'shader_default'");
      return;
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
    if(port->kind != Port_Kind::out) {
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
      return;
    }
  }

  rendering::set_uniform_mat4(shader_port, "vp_mat", vp_mat);

  for(Port const* const port: scene.ports) {
    rendering::Draw_Elements_Command cmd = prepare_draw(*port);
    rendering::add_draw_command(cmd);
  }
  rendering::commit_draw();
}

static void display_viewport(Scene& scene)
{
  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
  ImGui::Begin("Viewport", nullptr,
               ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar |
                 ImGuiWindowFlags_NoBringToFrontOnFocus);
  ImGui::PopStyleVar(3);
  ImVec2 const im_viewport_size = ImGui::GetContentRegionAvail();
  viewport_size = {im_viewport_size.x, im_viewport_size.y};
  ImVec2 const im_viewport_position = ImGui::GetWindowPos();
  viewport_position = {im_viewport_position.x, im_viewport_position.y};
  rendering::resize_framebuffers(viewport_size.x, viewport_size.y);
  glViewport(0, 0, viewport_size.x, viewport_size.y);
  rendering::Framebuffer* const primary_fb =
    rendering::get_primary_framebuffer();
  primary_fb->bind();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  render_scene(scene, viewport_size);
  scene.viewport_size = viewport_size;
  u32 texture = primary_fb->get_color_texture(0);
  ImGui::Image((void*)(u64)texture, im_viewport_size);
  ImGui::End();
}

[[nodiscard]] static char const* gate_to_string(Gate_Kind kind)
{
  switch(kind) {
  case Gate_Kind::e_and:
    return "AND";
  case Gate_Kind::e_or:
    return "OR";
  case Gate_Kind::e_xor:
    return "XOR";
  case Gate_Kind::e_nand:
    return "NAND";
  case Gate_Kind::e_nor:
    return "NOR";
  case Gate_Kind::e_xnor:
    return "XNOR";
  case Gate_Kind::e_not:
    return "NOT";
  case Gate_Kind::e_input:
    return "INPUT";
  case Gate_Kind::e_clock:
    return "CLOCK";
  case Gate_Kind::e_count:
    ANTON_UNREACHABLE("count is not a valid enumeration");
  }
  // CI complains about control reaching the end of a non-void function.
  // Suppress with return.
  return "INVALID";
}

void display_toolbar()
{
  ImGui::Begin("Toolbar", nullptr,
               ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove |
                 ImGuiWindowFlags_NoTitleBar);
  if(ImGui::Button("Toggle evaluation")) {
    run_evaluation = !run_evaluation;
  }
  if(ImGui::Button("Single step evaluation")) {
    single_step_evaluation = true;
  }

  ImGui::Separator();

  ImGui::BeginChild("Gates");
  u8 number_of_gate_types = static_cast<int>(Gate_Kind::e_count);
  for(int i = 0; i < number_of_gate_types; ++i) {
    Gate_Kind gate = static_cast<Gate_Kind>(i);
    const char* gateString = gate_to_string(gate);

    ImGui::Selectable(gateString);
    ImGuiDragDropFlags src_flags = 0;
    // Keep the source displayed as hovered.
    src_flags |= ImGuiDragDropFlags_SourceNoDisableHover;
    // Because our dragging is local, we disable the feature of opening foreign
    // treenodes/tabs while dragging.
    src_flags |= ImGuiDragDropFlags_SourceNoHoldToOpenOthers;
    if(ImGui::BeginDragDropSource(src_flags)) {
      if(!(src_flags & ImGuiDragDropFlags_SourceNoPreviewTooltip))
        ImGui::Text("Moving \"%s\"", gateString);
      ImGui::SetDragDropPayload("DND_DEMO_NAME", &i, sizeof(int));
      // TODO: remove camera movement when drag and drop
      ImGui::EndDragDropSource();

      last_menu_gate_choice = gate;
      is_draged_from_menu = true;
    }
  }
  ImGui::EndChild();

  // Close the toolbar.
  ImGui::End();
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

  initialise_imgui(window);

  compile_shaders();

  rendering::bind_draw_buffers();
  rendering::bind_transient_geometry_buffers();

  glClearColor(0.1, 0.1, 0.1, 1.0);

  // Main loop
  while(!windowing::should_close(window)) {
    frame_counter += 1;
    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();

    windowing::poll_events();

    if(run_evaluation) {
      if(frame_counter % evaluation_frequency == 0) {
        evaluate(scene.gates);
      }
    } else if(single_step_evaluation) {
      evaluate(scene.gates);
    }

    single_step_evaluation = false;

    Vec2 const window_size = windowing::get_framebuffer_size(window);

    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos({0.0f, 0.0f});
    ImGui::SetNextWindowSize({window_size.x, window_size.y});
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent
    // window not dockable into, because it would be confusing to have two
    // docking targets within each others.
    ImGuiWindowFlags const window_flags =
      ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking |
      ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
      ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
      ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
      ImGuiWindowFlags_NoBackground;
    ImGui::Begin("Main Dock", nullptr, window_flags);
    ImGui::PopStyleVar(3);

    // Dock the toolbar and the viewport side-by-side.
    ImGuiID dockspace_id = ImGui::GetID("_MainDock");
    ImGuiDockNodeFlags const dockspace_flags =
      ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_NoTabBar;
    static bool first_time_dock = true;
    if(first_time_dock) {
      first_time_dock = false;
      ImGui::DockBuilderAddNode(dockspace_id,
                                dockspace_flags | ImGuiDockNodeFlags_DockSpace);
      ImGui::DockBuilderSetNodePos(dockspace_id, ImGui::GetWindowPos());
      ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetWindowSize());
      ImGuiID node_a;
      ImGuiID node_b;
      ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.2f, &node_a,
                                  &node_b);
      ImGui::DockBuilderDockWindow("Toolbar", node_a);
      ImGui::DockBuilderDockWindow("Viewport", node_b);
    } else {
      ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }

    display_viewport(scene);
    display_toolbar();

    // Close the dock window.
    ImGui::End();

    glViewport(0, 0, window_size.x, window_size.y);

    rendering::bind_default_framebuffer();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    ImGuiMouseButton left_button = 0;
    /*
    If mouse is dragged from the manu and released then add gate
    */
    if(ImGui::IsMouseReleased(left_button) && is_draged_from_menu) {
      //later change to gate that was pressed
      scene.add_gate(gate_default_size, scene.last_mouse_position,
                     last_menu_gate_choice);
      is_draged_from_menu = 0;
    }

    windowing::swap_buffers(window);
  }

  ImGui_ImplGlfw_Shutdown();
  ImGui_ImplOpenGL3_Shutdown();
  ImGui::DestroyContext();

  rendering::teardown_shaders();
  rendering::teardown();
  windowing::destroy(window);

  return 0;
}
