#ifndef NEBULA_MOVABLE_RECT_HPP
#define NEBULA_MOVABLE_RECT_HPP

#include <types.hpp>

using namespace nebula;
/**
 * Movable rectangle ( in short MR ) is movable visual copy of MR Button created after interaction with it.
 * This structure holds information about objects current location in the application window and pointer
 * to render function inherited from MR Button.
 **/
typedef struct UI_Movable_Rect_S UI_Movable_Rect_S;

/**
 * Creates a new visual movable object at the middle of the screen following instructions in passed function.
 * @param render_function - function used to render object on the screen passed by MR button.
 * @param window_width - window width. Important for correct position calculation during movement
 * @param window_height - window height. Important for correct position calculation during movement
 * @return pointer to the newly created movable rectangle structure
 */
UI_Movable_Rect_S* UI_Movable_Rect_Init(void (*render_function)(anton::Rect_f32 position), i32 window_width
                                        , i32 window_height);

/**
 * Renders an object
 * @param mr - pointer to the structure of the movable rectangle we want to render
 */
void UI_Movable_Rect_Render(UI_Movable_Rect_S* mr);

/**
 * Check whether the object is located at the given mouse location
 * @param mr - pointer to the structure of the movable rectangle
 * @param x - mouse x location
 * @param y - mouse y location
 * @return true if rectangle is under given location
 */
bool UI_Movable_Rect_Is_Under_Mouse(UI_Movable_Rect_S* mr, f32 x, f32 y);

/**
 * Changes the location of an object
 * @param mr - pointer to the structure of the movable rectangle
 * @param x_offset - offset from previous mouse x location
 * @param y_offset - offset from previous mouse y location
 */
void UI_Movable_Rect_Move(UI_Movable_Rect_S* mr, f32 x_offset, f32 y_offset);

/**
 * Frees memory used to store information about Movable Rectangle.
 * @param mr - pointer to the structure of the movable rectangle to destroy
 */
void UI_Movable_Rect_Destroy(UI_Movable_Rect_S* mr);

#endif //NEBULA_MOVABLE_RECT_HPP
