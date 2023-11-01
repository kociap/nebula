#pragma once

#include <types.hpp>

namespace ui_window {
  /**
   * Window initialization function.
   * @return 0 on success else 1
   */
  nebula::u8 init();

  /**
   * Start main loop of initialized window.
   */
  void start_main_loop();

  /**
   * Termination function. Make sure to place it at the end
   * of the program to free up resources.
   */
  void terminate();
} // namespace ui_window
