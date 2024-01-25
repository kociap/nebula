#include <windowing/window.hpp>

// Do not include the GL headers.
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <components/camera.hpp>
#include <core/input.hpp>
#include <logging/logging.hpp>
#include <ui/ui.hpp>


namespace nebula::windowing {
  enum window_mode {
    WIN_MODE_DEFAULT,
    WIN_MODE_CAMERA_MOVING,
    WIN_MODE_GATE_MOVING,
    WIN_MODE_PORT_CONNECTING,
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

        Vec2 mouse_position;
        mouse_position.x = static_cast<f32>(x);
        mouse_position.y = static_cast<f32>(y);
        instance->currently_moved_gate =
          instance->ui.check_if_gate_clicked(mouse_position);

        instance->last_mouse_position = mouse_position;
        if(instance->currently_moved_gate != nullptr) {
          instance->mode = WIN_MODE_GATE_MOVING;
          return;
        }

        instance->connected_port =
          instance->ui.check_if_port_clicked(mouse_position);
        if(instance->connected_port != nullptr) {
          instance->mode = WIN_MODE_PORT_CONNECTING;
          return;
        }

        instance->mode = WIN_MODE_CAMERA_MOVING;
      } else if(action == GLFW_RELEASE) {
        instance->currently_moved_gate = nullptr;
        instance->connected_port = nullptr;
        instance->mode = WIN_MODE_DEFAULT;
      }
    }
  }

  // Function to handle mouse scroll events
  static void scroll_callback(GLFWwindow* window, double xoffset,
                              double yoffset)
  {
    (void)xoffset;

    auto* instance = static_cast<Window*>(glfwGetWindowUserPointer(window));

    Camera& primary_camera = get_primary_camera();
    if(yoffset < 0.0) {
      primary_camera.zoom(2);
    } else {
      primary_camera.zoom(0.5);
    }
    LOG_DEBUG("zoom changed: {}", primary_camera.zoom_level);
  }

  static void cursor_position_callbacks(GLFWwindow* win, double xpos,
                                        double ypos)
  {
    auto* instance = static_cast<Window*>(glfwGetWindowUserPointer(win));
    if(instance->mode == WIN_MODE_CAMERA_MOVING) {
      f32 x = static_cast<f32>(xpos);
      f32 y = static_cast<f32>(ypos);

      math::Vec3 offset;
      offset.x = x - instance->last_mouse_position.x;
      offset.y = y - instance->last_mouse_position.y;

      get_primary_camera().move(offset);
    } else if(instance->mode == WIN_MODE_GATE_MOVING) {
      f32 x = static_cast<f32>(xpos);
      f32 y = static_cast<f32>(ypos);
      // Calculate the new position of the rectangle based on mouse movement
      Vec2 offset;
      offset.x = x - instance->last_mouse_position.x;
      offset.y = y - instance->last_mouse_position.y;

      instance->currently_moved_gate->move(offset);
      instance->last_mouse_position.x = x;
      instance->last_mouse_position.y = y;
    } else if(instance->mode == WIN_MODE_PORT_CONNECTING) {
      // TODO: Implement port connecting
    }
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
    win->mode = WIN_MODE_DEFAULT;

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
