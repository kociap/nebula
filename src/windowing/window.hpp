#pragma once

#include <core/input.hpp>
#include <core/types.hpp>
#include <rendering/shader.hpp>
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
   * Sets up the camera for wireframe rendering using the provided shader.
   * @param shader_wire - handle to the wireframe shader
   */
  void setup_camera_projection(Window* window,
                               Handle<rendering::Shader> shader_wire);

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

  using keyboard_callback_t = void (*)(Window* window, Key key,
                                       Input_State state);
  using framebuffer_resize_callback_t = void (*)(Window* window, i64 width,
                                                 i64 height);

  void set_keyboard_callback(Window* window, keyboard_callback_t callback);
  void set_framebuffer_resize_callback(Window* window,
                                       framebuffer_resize_callback_t callback);

  /**
   * Destroys window and terminates glfw.
   * @param window - window to destroy.
   */
  void destroy(Window* window);

} // namespace nebula::windowing
