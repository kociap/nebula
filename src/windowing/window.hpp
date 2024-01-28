#pragma once

#include <core/input.hpp>
#include <rendering/shader.hpp>

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

  [[nodiscard]] void* get_native_handle(Window* window);

  /**
   * Returns window dimensions ( width, height )
   * @return Vec2{width, height}
   */
  [[nodiscard]] Vec2 get_window_size(Window* window);

  [[nodiscard]] Vec2 get_framebuffer_size(Window* window);

  [[nodiscard]] Vec2 get_cursor_position(Window* window);

  using keyboard_callback_t = void (*)(Window* window, Key key,
                                       Input_Action action, void* data);
  using mouse_button_callback_t = void (*)(Window* window, Key key,
                                           Input_Action action, void* data);
  using scroll_callback_t = void (*)(Window* window, f32 x, f32 y, void* data);
  using cursor_position_callback_t = void (*)(Window* window, f32 x, f32 y,
                                              void* data);
  using framebuffer_resize_callback_t = void (*)(Window* window, i64 width,
                                                 i64 height, void* data);

  void set_keyboard_callback(Window* window, keyboard_callback_t callback,
                             void* data);
  void set_mouse_button_callback(Window* window,
                                 mouse_button_callback_t callback, void* data);
  void set_scroll_callback(Window* window, scroll_callback_t callback,
                           void* data);
  void set_cursor_position_callback(Window* window,
                                    cursor_position_callback_t callback,
                                    void* data);
  void set_framebuffer_resize_callback(Window* window,
                                       framebuffer_resize_callback_t callback,
                                       void* data);

  /**
   * Destroys window and terminates glfw.
   * @param window - window to destroy.
   */
  void destroy(Window* window);

} // namespace nebula::windowing
