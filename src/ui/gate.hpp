#pragma once

#include <anton/math/vec2.hpp>

#include <rendering/rendering.hpp>
#include <ui/port.hpp>

namespace nebula {
  enum struct Gate_Kind : u8 {
    // Two input gates.
    e_and,
    e_or,
    e_xor,
    e_nand,
    e_nor,
    e_xnor,
    // One input gates.
    e_not,
  };

  /**
   * Gate.
   * This structure holds information about objects
   * current location in the application window and its gates.
   **/
  struct Gate {
    Array<Port*> in_ports;
    Array<Port*> out_ports;
    /**
     * Coordinates of the top left corner of the rect.
     */
    math::Vec2 coordinates;
    /**
     * Dimensions of the rect. x - width, y - height
     */
    math::Vec2 dimensions;
    Gate_Kind kind;

    /**
     * Creates a new visual movable object at the middle of the screen following
     * instructions in passed function.
     * @param num_in_ports - number of IN type ports
     * @param num_out_ports - number of OUT type ports
     * @return pointer to the newly created movable gate structure
     */
    Gate(math::Vec2 dimensions, math::Vec2 coordinates, Gate_Kind kind);

    /**
     * Changes the location of an object
     * @param offset - vector, offset from previous mouse x and y location
     */
    void move(math::Vec2 offset);
  };

  /**
   * Test whether a point is within the bounds of a gate.
   * @param point - x and y coordinates of point to check
   * @return true if rect is under given point
   */
  [[nodiscard]] bool test_hit(Gate const& gate, math::Vec2 point);

  [[nodiscard]] rendering::Draw_Elements_Command prepare_draw(Gate const& gate);
} // namespace nebula
