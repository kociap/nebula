#pragma once

#include <anton/math/vec2.hpp>

#include <model/port.hpp>
#include <rendering/rendering.hpp>

namespace nebula {
  /**
   * @brief Enumeration representing different kinds of logic gates.
   *
   * The Gate_Kind enumeration represents various types of logic gates, categorized based
   * on the number of inputs they accept. Two-input gates include AND, OR, XOR, NAND, NOR, and XNOR,
   * while one-input gates include NOT.
   */
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

    // One output gates.
    e_input,

    GATES_COUNT, // 
  };

  struct Evaluation_State {
    bool prev_value = false;
    bool value = false;
  };

  /**
   * @brief Gate structure.
   *
   * The Gate structure holds information about an object's current location in the
   * application window and its gates.
   */
  struct Gate {
    Array<Port*> in_ports;
    Array<Port*> out_ports;

    /**
     * @brief Coordinates of the top-left corner of the rectangle.
     */
    math::Vec2 coordinates;

    /**
     * @brief Dimensions of the rectangle.
     * x - width, y - height.
     */
    math::Vec2 dimensions;

    Gate_Kind kind;

    Evaluation_State evaluation;

    /**
     * @brief Constructs a new gate.
     *
     * Creates a new visual movable object at the specified location with the specified dimensions
     * and logic gate kind.
     *
     * @param dimensions The dimensions of the gate (width and height).
     * @param coordinates The coordinates of the top-left corner of the gate.
     * @param kind The kind of logic gate.
     */
    Gate(math::Vec2 dimensions, math::Vec2 coordinates, Gate_Kind kind);

    /**
     * @brief Moves the gate to a new location.
     *
     * Changes the location of the gate based on the provided offset from the previous
     * mouse x and y location.
     *
     * @param offset The vector representing the offset from the previous mouse x and y location.
     */
    void move(math::Vec2 offset);
  };

  /**
   * @brief Tests whether a point is within the bounds of a gate.
   *
   * This function checks if the given point (x and y coordinates) is within the bounds
   * of the specified gate.
   *
   * @param gate The gate to check.
   * @param point The x and y coordinates of the point to check.
   * @return True if the point is within the bounds of the gate, false otherwise.
   */
  [[nodiscard]] bool test_hit(Gate const& gate, math::Vec2 point);

  /**
   * @brief Prepares draw command for rendering a gate.
   *
   * This function prepares a draw command for rendering the specified gate. The draw
   * command includes information necessary to render the gate.
   *
   * @param gate The gate to prepare the draw command for.
   * @return A rendering::Draw_Elements_Command for rendering the gate.
   */
  [[nodiscard]] rendering::Draw_Elements_Command prepare_draw(Gate const& gate);

} // namespace nebula
