#pragma once

#include <anton/math/vec2.hpp>
#include <ui/node_rect.hpp>
#include <ui/port.hpp>

namespace nebula {
  /**
   * Movable gate.
   * This structure holds information about objects
   * current location in the application window and its gates.
   **/
  struct Movable_Gate {
    Node_Rect rect;
    List<Port*> in_ports;
    List<Port*> out_ports;

    /**
     * Creates a new visual movable object at the middle of the screen following
     * instructions in passed function.
     * @param num_in_ports - number of IN type ports
     * @param num_out_ports - number of OUT type ports
     * @return pointer to the newly created movable gate structure
     */
    Movable_Gate(math::Vec2 rectangle_dimensions, math::Vec4 camera_borders,
                 u8 num_in_ports, u8 num_out_ports);

    /**
     * Renders the gate.
     */
    void add_to_render_loop() const;

    /**
     * Check whether the object is located at the given mouse location
     * @param mouse_location - vector, mouse x and y location
     * @return true if gate is under given location
     */
    [[nodiscard]] bool is_under_mouse(math::Vec2 mouse_location) const;
    /**
     * Changes the location of an object
     * @param offset - vector, offset from previous mouse x and y location
     */
    void move(math::Vec2 offset);
  };
} // namespace nebula
