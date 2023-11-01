#include <ui/window.hpp>

int main(int argc, char* argv[])
{
  (void)argc;
  (void)argv;

  if(UI_Window_Init() != 0) {
      return 1;
  }

  UI_Window_Start_Main_Loop();

  UI_Window_Terminate();
  return 0;
}
