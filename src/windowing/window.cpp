#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <windowing/window.hpp>

namespace nebula::windowing {
  struct Window {
    i32 width; // window width
    i32 height; // window height
    GLFWwindow* glfw_window; // GLFW window instance
    Vec2 last_mouse_position; // last mouse position
    List<Movable_Rect> movable_rectangles; // All created rectangles/gates
    Movable_Rect* currently_moved_rectangle; // Currently moved rect/gate
  };

  static void keyboard_button_callback(GLFWwindow* win, int key, int scancode,
                                       int action, int mods);
  static void mouse_button_callbacks(GLFWwindow* win, int button, int action,
                                     int mods);
  static void cursor_position_callbacks(GLFWwindow* win, double xpos,
                                        double ypos);
  /**
   * Adds new rectangle. Function used by MR buttons.
   * @param window - window instance
   * @param render_function - render function inherited from MR button.
   * @param rectangle_dimensions - dimensions inherited from MR button.
   */
  static void add_rectangle(Window* window, void (*render_function)(Node_Rect),
                            Vec2 rectangle_dimensions);

  static void keyboard_button_callback(GLFWwindow* win, int key, int scancode,
                                       int action, int mods)
  {
    auto* instance = static_cast<Window*>(glfwGetWindowUserPointer(win));
    // EMPTY FOR NOW
    (void)instance;
    (void)key;
    (void)scancode;
    (void)action;
    (void)mods;
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
        // Check if the mouse click is within any rectangle
        for(auto& mr: instance->movable_rectangles) {
          if(mr.is_under_mouse(mouse_position)) {
            instance->currently_moved_rectangle = &mr;
            instance->last_mouse_position = mouse_position;
          }
        }
      } else if(action == GLFW_RELEASE) {
        instance->currently_moved_rectangle = nullptr;
      }
    }
  }

  static void cursor_position_callbacks(GLFWwindow* win, double xpos,
                                        double ypos)
  {
    auto* instance = static_cast<Window*>(glfwGetWindowUserPointer(win));
    if(instance->currently_moved_rectangle == nullptr) {
      return;
    }

    f32 x = static_cast<f32>(xpos);
    f32 y = static_cast<f32>(ypos);
    // Calculate the new position of the rectangle based on mouse movement
    Vec2 offset;
    offset.x = x - instance->last_mouse_position.x;
    offset.y = y - instance->last_mouse_position.y;

    instance->currently_moved_rectangle->move(offset);
    instance->last_mouse_position.x = x;
    instance->last_mouse_position.y = y;
  }

  void render_objects(Window* window)
  {
    for(auto& rect: window->movable_rectangles) {
      rect.render();
    }
  }

  static void add_rectangle(Window* window, void (*render_function)(Node_Rect),
                            Vec2 rectangle_dimensions)
  {
    f32 w = static_cast<f32>(window->width);
    f32 h = static_cast<f32>(window->height);
    Movable_Rect rect(render_function, rectangle_dimensions, {w, h});
    window->movable_rectangles.emplace_front(rect);
  }

  Window* init()
  {
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // Initialize GLFW
    glfwInit();

    auto* win = new Window;
    // Get the primary monitor's video mode to determine the screen resolution
    GLFWmonitor* primary_monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primary_monitor);
    win->width = mode->width;
    win->height = mode->height;

    win->currently_moved_rectangle = nullptr;
    // Create a GLFW window
    win->glfw_window =
      glfwCreateWindow(win->width, win->height, "Nebula", nullptr, nullptr);

    if(!win->glfw_window) {
      glfwTerminate();
      return nullptr;
    }
    // Set custom window pointer to this structure
    glfwSetWindowUserPointer(win->glfw_window, win);

    // Set callbacks
    glfwSetMouseButtonCallback(win->glfw_window, mouse_button_callbacks);
    glfwSetKeyCallback(win->glfw_window, keyboard_button_callback);
    glfwSetCursorPosCallback(win->glfw_window, cursor_position_callbacks);

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

  void pool_events()
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

  Vec2 get_dimensions(Window* window)
  {
    return {static_cast<f32>(window->width), static_cast<f32>(window->height)};
  }

} // namespace nebula::windowing
