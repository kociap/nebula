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
#include <windowing/window.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

using namespace nebula;

namespace {
  bool is_draged_from_menu = false;
  Gate_Kind last_menu_gate_choice = Gate_Kind::e_count;
  bool run_evaluation = false;
  bool single_step_evaluation = false;
  i64 evaluation_frequency = 1; // TODO: Frequency switching button (1,2,4,8,16)
  i64 frame_counter = 0;
  Vec2 const gate_default_size{0.6f, 0.5f};
} // namespace

void initialise_imgui(windowing::Window* const window)
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
  LOG_INFO("resized window to {}x{}", width, height);
}

static void mouse_button_callback(windowing::Window* const window,
                                  Key const key, Input_Action const action,
                                  void* data)
{
  // TODO: do not allow linking, port creating and deleting in evaluation mode
  ANTON_UNUSED(window);
  auto& scene = *reinterpret_cast<Scene*>(data);
  if(key == Key::mouse_left) {
    if(action == Input_Action::press) {
      Vec2 cursor_position = windowing::get_cursor_position(window);

      Camera& cam = get_primary_camera();
      Vec2 const window_size = get_window_size(window);
      Vec2 framebuffer_size = get_framebuffer_size(window);
      cursor_position -= window_size - scene.viewport_size - 10; // margin

      if(cursor_position.x < 0 || cursor_position.y < 0) {
        return;
      }
      framebuffer_size.x =
        framebuffer_size.y * (scene.viewport_size.x / scene.viewport_size.y);
      Vec2 const scene_position = cam.window_to_scene_position(
        cursor_position, scene.viewport_size, framebuffer_size);

      // Check if connected to another port
      if(scene.mode == Window_Mode::port_linking) {
        Port* p = scene.check_if_port_clicked(scene_position);
        // Can't connect to the same port
        if(p == nullptr || p->kind == scene.connected_port->kind) {
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
        if(scene.connected_port->kind == Port_Kind::in) {
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
  } else if(key == Key::mouse_right && action == Input_Action::press) {
    Vec2 const cursor_position = windowing::get_cursor_position(window);

    Camera& cam = get_primary_camera();
    Vec2 const framebuffer_size = get_framebuffer_size(window);
    Vec2 const window_size = get_window_size(window);
    Vec2 const scene_position = cam.window_to_scene_position(
      cursor_position, window_size, framebuffer_size);
    Port* p = scene.check_if_port_clicked(scene_position);
    if(p != nullptr) {
      p->remove_all_connections();
      return;
    }
    Gate* g = scene.check_if_gate_clicked(scene_position);
    if(g != nullptr && g->kind == Gate_Kind::e_input) {
      g->evaluation = {!g->evaluation.prev_value, !g->evaluation.value};
      return;
    }
  }
}

static void cursor_position_callback(windowing::Window* const window,
                                     f32 const x, f32 const y, void* data)
{
  ANTON_UNUSED(window);
  Scene& scene = *reinterpret_cast<Scene*>(data);
  Vec2 cursor_position = {x, y};
  Camera& cam = get_primary_camera();
  Vec2 const window_size = get_window_size(window);
  Vec2 framebuffer_size = get_framebuffer_size(window);
  cursor_position -= window_size - scene.viewport_size - 10;
  if(cursor_position.x < 0 || cursor_position.y < 0) {
    return;
  }
  framebuffer_size.x =
    framebuffer_size.y * (scene.viewport_size.x / scene.viewport_size.y);
  Vec2 const scene_position = cam.window_to_scene_position(
    cursor_position, scene.viewport_size, framebuffer_size);

  math::Vec2 offset = (scene_position - scene.last_mouse_position) *
                      framebuffer_size / window_size;

  if(scene.mode == Window_Mode::port_linking) {
    scene.move_tmp_port(offset);
  } else if(scene.mode == Window_Mode::camera_moving) {
    cam.move(offset);
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

static void render_viewport(Scene& scene)
{
  i8 const margin = 10;
  ImVec2 display_size = ImGui::GetIO().DisplaySize;
  // Leave margin
  display_size.x -= margin;
  display_size.y -= margin;
  ImGui::SetNextWindowSize({display_size.x, display_size.y},
                           ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowSizeConstraints({display_size.x, display_size.y},
                                      {display_size.x, display_size.y});
  ImGui::SetNextWindowPos(display_size, ImGuiCond_Always, {1.0f, 1.0f});
  ImGui::SetNextWindowPos({display_size.x, margin}, ImGuiCond_Always,
                          {1.0f, 0.0f});
  ImGui::Begin("Viewport", nullptr,
               ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar |
                 ImGuiWindowFlags_NoBringToFrontOnFocus);
  ImVec2 const im_viewport_size = ImGui::GetWindowSize();
  Vec2 const viewport_size{im_viewport_size.x - margin,
                           im_viewport_size.y - margin};
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

    if(single_step_evaluation) {
      evaluate(scene.gates);
      single_step_evaluation = false;
    } else if(run_evaluation) {
      if(frame_counter % evaluation_frequency == 0) {
        evaluate(scene.gates);
      }
    }

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

    ImGuiID dockspace_id = ImGui::GetID("_MainDock");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f),
                     ImGuiDockNodeFlags_PassthruCentralNode);

    render_viewport(scene);

    f32 display_size_y = ImGui::GetIO().DisplaySize.y;
    ImGui::SetNextWindowPos({0.0, 0.0}, ImGuiCond_FirstUseEver, {0.0, 0.0});
    // ImGui::SetNextWindowPos({0.0, display_size_y}, ImGuiCond_FirstUseEver,
    //                         {0.0f, 1.0f});
    ImGui::SetNextWindowSizeConstraints({FLT_MIN, display_size_y},
                                        {FLT_MAX, display_size_y});

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
