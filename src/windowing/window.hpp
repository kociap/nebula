#pragma once

#include <core/input.hpp>
#include <rendering/shader.hpp>

namespace nebula::windowing {
  struct Window;

  /**
   * @brief Initializes GLFW and creates a new fullscreen window with callbacks.
   *
   * This function initializes GLFW and creates a new fullscreen window, setting up associated callbacks.
   *
   * @return A pointer to the created window.
   */
  [[nodiscard]] Window* init();

  /**
   * @brief Checks if the window should close.
   *
   * @param window The window to check.
   * @return True if the window should close or if it is nullptr.
   */
  [[nodiscard]] bool should_close(Window* window);

  /**
   * @brief Processes all pending events.
   *
   * This function processes only those events that are already in the event queue and then returns immediately.
   * Processing events will cause the window and input callbacks associated with those events to be called.
   */
  void poll_events();

  /**
   * @brief Swaps the front and back buffers of the specified window.
   *
   * @param window The window whose buffers should be swapped.
   */
  void swap_buffers(Window* window);

  /**
  * @brief Retrieves the native handle of the GLFW window.
  *
  * @param window The window for which to retrieve the native handle.
  * @return A pointer to the native window handle.
  */
  [[nodiscard]] void* get_native_handle(Window* window);

  /**
   * @brief Returns the window dimensions (width, height).
   *
   * @param window The window to query.
   * @return A Vec2 representing the window dimensions.
   */
  [[nodiscard]] Vec2 get_window_size(Window* window);

  /**
   * @brief Returns the framebuffer size of the window.
   *
   * @param window The window to query.
   * @return A Vec2 representing the framebuffer size.
   */
  [[nodiscard]] Vec2 get_framebuffer_size(Window* window);

  /**
   * @brief Returns the cursor position within the window.
   *
   * @param window The window to query.
   * @return A Vec2 representing the cursor position.
   */
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

  /**
   * @brief Sets the keyboard callback for the specified window.
   *
   * @param window The window to set the callback for.
   * @param callback The callback function to be set.
   * @param data Additional data to be passed to the callback.
   */
  void set_keyboard_callback(Window* window, keyboard_callback_t callback,
                             void* data);

  /**
   * @brief Sets the mouse button callback for the specified window.
   *
   * @param window The window to set the callback for.
   * @param callback The callback function to be set.
   * @param data Additional data to be passed to the callback.
   */
  void set_mouse_button_callback(Window* window,
                                 mouse_button_callback_t callback, void* data);

  /**
   * @brief Sets the scroll callback for the specified window.
   *
   * @param window The window to set the callback for.
   * @param callback The callback function to be set.
   * @param data Additional data to be passed to the callback.
   */
  void set_scroll_callback(Window* window, scroll_callback_t callback,
                           void* data);

  /**
   * @brief Sets the cursor position callback for the specified window.
   *
   * @param window The window to set the callback for.
   * @param callback The callback function to be set.
   * @param data Additional data to be passed to the callback.
   */
  void set_cursor_position_callback(Window* window,
                                    cursor_position_callback_t callback,
                                    void* data);

  /**
   * @brief Sets the framebuffer resize callback for the specified window.
   *
   * @param window The window to set the callback for.
   * @param callback The callback function to be set.
   * @param data Additional data to be passed to the callback.
   */
  void set_framebuffer_resize_callback(Window* window,
                                       framebuffer_resize_callback_t callback,
                                       void* data);

  /**
   * @brief Destroys the window and terminates GLFW.
   *
   * @param window The window to destroy.
   */
  void destroy(Window* window);

} // namespace nebula::windowing
