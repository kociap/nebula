#include <windowing/window.hpp>

// Do not include the GL headers.
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <components/camera.hpp>
#include <core/input.hpp>

#include <logging/logging.hpp>
#include <ui/ui.hpp>

namespace nebula::windowing {
  struct Window {
    GLFWwindow* glfw_window;
    keyboard_callback_t keyboard_cb = nullptr;
    void* keyboard_data = nullptr;
    mouse_button_callback_t mouse_button_cb = nullptr;
    void* mouse_button_data = nullptr;
    scroll_callback_t scroll_cb = nullptr;
    void* scroll_data = nullptr;
    cursor_position_callback_t cursor_position_cb = nullptr;
    void* cursor_position_data = nullptr;
    framebuffer_resize_callback_t framebuffer_resize_cb = nullptr;
    void* framebuffer_resize_data = nullptr;
  };

  static void keyboard_button_callback(GLFWwindow* glfw_window, int key,
                                       int scancode, int action, int mods)
  {
    (void)scancode;
    (void)mods;
    auto* const window =
      reinterpret_cast<Window*>(glfwGetWindowUserPointer(glfw_window));
    if(window->keyboard_cb != nullptr) {
      window->keyboard_cb(window, static_cast<Key>(key),
                          static_cast<Input_Action>(action),
                          window->keyboard_data);
    }
  }

  static void scroll_cb(GLFWwindow* glfw_window, double xoffset, double yoffset)
  {
    auto* const window =
      reinterpret_cast<Window*>(glfwGetWindowUserPointer(glfw_window));
    if(window->scroll_cb != nullptr) {
      window->scroll_cb(window, xoffset, yoffset, window->scroll_data);
    }
  }

  static void framebuffer_resize_cb(GLFWwindow* glfw_window, int width,
                                    int height)
  {
    auto* const window =
      reinterpret_cast<Window*>(glfwGetWindowUserPointer(glfw_window));
    if(window->framebuffer_resize_cb != nullptr) {
      window->framebuffer_resize_cb(window, width, height,
                                    window->framebuffer_resize_data);
    }
  }

  static void mouse_button_callbacks(GLFWwindow* const glfw_window, int button,
                                     int action, int mods)
  {
    ANTON_UNUSED(mods);
    auto* const window =
      static_cast<Window*>(glfwGetWindowUserPointer(glfw_window));
    if(window->mouse_button_cb != nullptr) {
      window->mouse_button_cb(window, static_cast<Key>(button),
                              static_cast<Input_Action>(action),
                              window->mouse_button_data);
    }
  }

  static void cursor_position_callbacks(GLFWwindow* const glfw_window,
                                        double const xpos, double const ypos)
  {
    auto* const window =
      static_cast<Window*>(glfwGetWindowUserPointer(glfw_window));
    if(window->cursor_position_cb != nullptr) {
      window->cursor_position_cb(window, xpos, ypos,
                                 window->cursor_position_data);
    }
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

    // Create a GLFW window
    win->glfw_window =
      glfwCreateWindow(mode->width, mode->height, "Nebula", nullptr, nullptr);

    if(!win->glfw_window) {
      glfwTerminate();
      return nullptr;
    }

    // Set custom window pointer to this structure
    glfwSetWindowUserPointer(win->glfw_window, win);

    // Set callbacks
    glfwSetMouseButtonCallback(win->glfw_window, mouse_button_callbacks);
    glfwSetKeyCallback(win->glfw_window, keyboard_button_callback);
    glfwSetScrollCallback(win->glfw_window, scroll_cb);
    glfwSetCursorPosCallback(win->glfw_window, cursor_position_callbacks);
    glfwSetFramebufferSizeCallback(win->glfw_window, framebuffer_resize_cb);

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

  Vec2 get_cursor_position(Window* const window)
  {
    double x, y;
    glfwGetCursorPos(window->glfw_window, &x, &y);
    return Vec2(x, y);
  }

  void set_keyboard_callback(Window* window, keyboard_callback_t callback,
                             void* data)
  {
    window->keyboard_data = data;
    window->keyboard_cb = callback;
  }

  void set_mouse_button_callback(Window* window,
                                 mouse_button_callback_t callback, void* data)
  {
    window->mouse_button_cb = callback;
    window->mouse_button_data = data;
  }

  void set_scroll_callback(Window* window, scroll_callback_t callback,
                           void* data)
  {
    window->scroll_cb = callback;
    window->scroll_data = data;
  }

  void set_cursor_position_callback(Window* window,
                                    cursor_position_callback_t callback,
                                    void* data)
  {
    window->cursor_position_cb = callback;
    window->cursor_position_data = data;
  }

  void set_framebuffer_resize_callback(Window* window,
                                       framebuffer_resize_callback_t callback,
                                       void* data)
  {
    window->framebuffer_resize_cb = callback;
    window->framebuffer_resize_data = data;
  }
} // namespace nebula::windowing
