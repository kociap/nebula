#ifndef NEBULA_WINDOW_H
#define NEBULA_WINDOW_H

#include <GLFW/glfw3.h>
#include <types.hpp>

using namespace nebula;

/**
 * Window initialization function.
 * @return 0 on success else 1
 */
u8 UI_Window_Init();

/**
 * Start main loop of initialized window.
 */
void UI_Window_Start_Main_Loop();

/**
 * Termination function. Make sure to place it at the end
 * of the program to free up resources.
 */
void UI_Window_Terminate();

#endif //NEBULA_WINDOW_H
