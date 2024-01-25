#include <windowing/window.hpp>

// Do not include the GL headers.
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <components/camera.hpp>
#include <core/input.hpp>

#include <logging/logging.hpp>
#include <ui/ui.hpp>


namespace nebula::windowing {
  enum struct window_mode {
    none,
    camera_moving,
    gate_moving,
    port_connecting,
  };

  struct Window {
    window_mode mode;
    UI ui;
    i32 width; // window width
    i32 height; // window height
    GLFWwindow* glfw_window; // GLFW window instance
    Vec2 last_mouse_position; // last mouse position
    Movable_Gate* currently_moved_gate; // Currently moved rect/gate
    Port* connected_port;
    keyboard_callback_t keyboard_callback = nullptr;
    framebuffer_resize_callback_t framebuffer_resize_callback = nullptr;
  };

  static void keyboard_button_callback(GLFWwindow* glfw_window, int key,
                                       int scancode, int action, int mods)
  {
    (void)scancode;
    (void)mods;

    auto* const window =
      reinterpret_cast<Window*>(glfwGetWindowUserPointer(glfw_window));
    if(window->keyboard_callback != nullptr) {
      window->keyboard_callback(window, static_cast<Key>(key),
                                static_cast<Input_State>(action));
    }
  }

  static void framebuffer_resize_callback(GLFWwindow* glfw_window, int width,
                                          int height)
  {
    auto* const window =
      reinterpret_cast<Window*>(glfwGetWindowUserPointer(glfw_window));
    if(window->framebuffer_resize_callback != nullptr) {
      window->framebuffer_resize_callback(window, width, height);
    }
    window->width = width;
    window->height = height;
  }

  static void mouse_button_callbacks(GLFWwindow* win, int button, int action,
                                     int mods)
  {
    auto* instance = static_cast<Window*>(glfwGetWindowUserPointer(win));
    (void)mods;
    if(button == GLFW_MOUSE_BUTTON_LEFT) {
      if(action == GLFW_PRESS) {
        double x, y;
        glfwGetCursorPos(win, &x, &y);

        Camera& cam = get_primary_camera();
        Vec2 scene_position = cam.window_to_scene_position(
                {static_cast<f32>(x), static_cast<f32>(y)}, instance->width,
                instance->height);
        instance->currently_moved_gate =
          instance->ui.check_if_gate_clicked(scene_position);

        instance->last_mouse_position = scene_position;
        if(instance->currently_moved_gate != nullptr) {
          instance->mode = window_mode::gate_moving;
          return;
        }

        instance->connected_port =
          instance->ui.check_if_port_clicked(scene_position);
        if(instance->connected_port != nullptr) {
          instance->mode = window_mode::port_connecting;
          return;
        }

        instance->mode = window_mode::camera_moving;
        instance->last_mouse_position = scene_position;
      } else if(action == GLFW_RELEASE) {
        instance->currently_moved_gate = nullptr;
        instance->connected_port = nullptr;
        instance->mode = window_mode::none;
      }
    }
  }

  // Function to handle mouse scroll events
  static void scroll_callback(GLFWwindow* window, double xoffset,
                              double yoffset)
  {
    (void)xoffset;
    (void)window;

    Camera& primary_camera = get_primary_camera();
    if(yoffset < 0.0) {
      primary_camera.zoom(1.25);
    } else {
      primary_camera.zoom(0.8);
    }
    LOG_DEBUG("zoom changed: {}", primary_camera.zoom_level);
  }

  static void cursor_position_callbacks(GLFWwindow* win, double xpos,
                                        double ypos)
  {
    auto* instance = static_cast<Window*>(glfwGetWindowUserPointer(win));

    Camera& cam = get_primary_camera();
    Vec2 window_position = {static_cast<f32>(xpos), static_cast<f32>(ypos)};
    Vec2 scene_position = cam.window_to_scene_position(
            window_position, instance->width, instance->height);

    math::Vec2 offset;
    offset.x = scene_position.x - instance->last_mouse_position.x;
    offset.y = scene_position.y - instance->last_mouse_position.y;

    if(instance->mode == window_mode::camera_moving) {
      cam.move(offset);
    } else if(instance->mode == window_mode::gate_moving) {
      instance->currently_moved_gate->move(offset);
    } else if(instance->mode == window_mode::port_connecting) {
      // TODO: Implement port connecting
    }
    instance->last_mouse_position.x = scene_position.x;
    instance->last_mouse_position.y = scene_position.y;
  }

  void add_objects_to_render_loop(Window* window)
  {
    window->ui.add_gates_to_render_loop();
  }


  Window* init()
  {
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // Initialize GLFW
    glfwInit();

    auto* const win = new Window;
    // Get the primary monitor's video mode to determine the screen resolution
    GLFWmonitor* primary_monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primary_monitor);

    win->currently_moved_gate = nullptr;
    // Create a GLFW window
    win->glfw_window =
      glfwCreateWindow(mode->width, mode->height, "Nebula", nullptr, nullptr);

    if(!win->glfw_window) {
      glfwTerminate();
      return nullptr;
    }

    win->ui = UI();
    win->ui.add_movable_gate({0.4f, 0.2f}, {-1.0f, 1.0f, -1.0f, 1.0f}, 2, 1);
    win->mode = window_mode::none;

    // Set custom window pointer to this structure
    glfwSetWindowUserPointer(win->glfw_window, win);

    // Set callbacks
    glfwSetMouseButtonCallback(win->glfw_window, mouse_button_callbacks);
    glfwSetKeyCallback(win->glfw_window, keyboard_button_callback);
    glfwSetCursorPosCallback(win->glfw_window, cursor_position_callbacks);
    glfwSetFramebufferSizeCallback(win->glfw_window,
                                   framebuffer_resize_callback);
    glfwSetScrollCallback(win->glfw_window, scroll_callback);

    glfwMakeContextCurrent(win->glfw_window);

    return win;
  }

  void destroy(Window* window)
  {
    if(window == nullptr) {
      return;
    }

    glfwDestroyWindow(window->glfw_window);
    delete window;
    glfwTerminate();
  }

  void poll_events()
  {
    glfwPollEvents();
  }

  void swap_buffers(Window* window)
  {
    glfwSwapBuffers(window->glfw_window);
  }

  bool should_close(Window* window)
  {
    if(window == nullptr) {
      return true;
    }
    return glfwWindowShouldClose(window->glfw_window);
  }

  Vec2 get_window_size(Window* const window)
  {
    i32 width;
    i32 height;
    glfwGetWindowSize(window->glfw_window, &width, &height);
    return Vec2(width, height);
  }

  Vec2 get_framebuffer_size(Window* const window)
  {
    i32 width;
    i32 height;
    glfwGetFramebufferSize(window->glfw_window, &width, &height);
    return Vec2(width, height);
  }

  void set_keyboard_callback(Window* window, keyboard_callback_t callback)
  {
    window->keyboard_callback = callback;
  }

  void set_framebuffer_resize_callback(Window* window,
                                       framebuffer_resize_callback_t callback)
  {
    window->framebuffer_resize_callback = callback;
  }
} // namespace nebula::windowing
