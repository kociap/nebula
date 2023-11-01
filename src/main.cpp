#include <ui/window.hpp>

int main(int argc, char* argv[])
{
  (void)argc;
  (void)argv;

  if(ui_window::init() != 0) {
    return 1;
  }

  ui_window::start_main_loop();

  ui_window::terminate();
  return 0;
}
