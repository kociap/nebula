#pragma once

#include <anton/math/vec2.hpp>
#include <core/types.hpp>
#include <ui/node_rect.hpp>

namespace nebula {
  /**
   * Movable rectangle ( in short MR ) is movable visual copy of MR Button
   *created
   *after interaction with it. This structure holds information about objects
   *current location in the application window and pointer to render function
   *inherited from MR Button.
   **/
  struct Movable_Rect {
    void (*render_function)(Node_Rect pixel);
    Node_Rect rect;

    /**
     * Creates a new visual movable object at the middle of the screen following
     * instructions in passed function.
     * @param render_function - function used to render object on the screen
     *                          passed by MR button.
     * @return pointer to the newly created movable rectangle structure
     */
    Movable_Rect(void (*render_function)(Node_Rect rect),
                 math::Vec2 rectangle_dimensions, math::Vec2 window_dimensions);

    /**
     * Renders an object.
     */
    void render() const;

    /**
     * Check whether the object is located at the given mouse location
     * @param mouse_location - vector, mouse x and y location
     * @return true if rectangle is under given location
     */
    [[nodiscard]] bool is_under_mouse(math::Vec2 mouse_location) const;
    /**
     * Changes the location of an object
     * @param offset - vector, offset from previous mouse x and y location
     */
    void move(math::Vec2 offset);
  };
} // namespace nebula
