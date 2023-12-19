#pragma once

#include <core/types.hpp>
#include <ui/movable_rect.hpp>

namespace nebula::windowing {

  struct Window;

  /**
   * Initializes glfw and creates new fullscreen window with
   * callbacks.
   */
  [[nodiscard]] Window* init();

  /**
   * Tells if window should close
   * @return true if window should close or is nullptr
   */
  [[nodiscard]] bool should_close(Window* window);

  /**
   * Function to render all objects like movable_rectangles.
   */
  void render_objects(Window* window);

  /**
   * Processes all pending events.
   * This function processes only those events that are already
   * in the event queue and then returns immediately. Processing
   * events will cause the window and input callbacks associated
   * with those events to be called.
   */
  void poll_events();

  /**
   * Swaps the front and back buffers of the specified window.
   */
  void swap_buffers(Window* window);

  /**
   * Returns window dimensions ( width, height )
   * @return Vec2{width, height}
   */
  [[nodiscard]] Vec2 get_dimensions(Window* window);

  /**
   * Destroys window and terminates glfw.
   * @param window - window to destroy.
   */
  void destroy(Window* window);

} // namespace nebula::windowing
