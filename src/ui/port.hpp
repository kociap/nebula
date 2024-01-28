#pragma once

#include <core/types.hpp>
#include <rendering/rendering.hpp>

namespace nebula {
  /**
   * @brief Enumeration representing different kinds of ports.
   *
   * The Port_Kind enumeration represents the types of ports, categorized as input (in) or
   * output (out).
   */
  enum struct Port_Kind { in, out };

  /**
   * @brief Port structure.
   *
   * The Port structure represents a circle-shaped port with specified coordinates and type (IN/OUT).
   */
  struct Port {
    List<Port*> connections; ///< List of ports connected to this port.
    Vec2 coordinates; ///< Coordinates of the center of the port.
    f32 radius; ///< Radius of the port.
    Port_Kind type; ///< Type of the port (IN or OUT).

    /**
     * @brief Initializes a circle-shaped port with specified coordinates and type.
     *
     * @param coordinates The x and y coordinates of the center of the port.
     * @param type The type of the port (IN or OUT).
     */
    Port(Vec2 coordinates, Port_Kind type);

    /**
     * @brief Moves the port by the given offset.
     *
     * @param offset The vector representing the offset from the previous gate position.
     */
    void move(Vec2 offset);

    /**
     * @brief Connects the port to another specified port.
     *
     * @param new_port Pointer to the port to be connected.
     */
    void add_connection(Port* new_port);

    /**
     * @brief Disconnects the port from a specified connected port.
     *
     * @param old_port Pointer to the port to be disconnected.
     */
    void remove_connection(Port* old_port);

    /**
     * @brief Disconnects the port from all connected ports.
     */
    void remove_all_connections();

    /**
     * @brief Gets the coordinates of the port.
     *
     * @return The coordinates of the port.
     */
    [[nodiscard]] Vec2 get_coordinates() const;

    /**
     * @brief Checks if the port is under the specified mouse position.
     *
     * @param mouse_position The x and y coordinates of the mouse position.
     * @return True if the port is under the mouse, false otherwise.
     */
    [[nodiscard]] bool is_under_mouse(Vec2 mouse_position) const;
  };

  /**
   * @brief Prepares draw command for rendering a port.
   *
   * This function prepares a draw command for rendering the specified port. The draw
   * command includes information necessary to render the port.
   *
   * @param port The port to prepare the draw command for.
   * @return A rendering::Draw_Elements_Command for rendering the port.
   */
  [[nodiscard]] rendering::Draw_Elements_Command prepare_draw(Port const& port);

  /**
   * @brief Prepares draw command for rendering a connection between two points.
   *
   * This function prepares a draw command for rendering a connection between two points.
   * The draw command includes information necessary to render the connection.
   *
   * @param cords_1 The coordinates of the first point.
   * @param cords_2 The coordinates of the second point.
   * @return A rendering::Draw_Elements_Command for rendering the connection.
   */
  [[nodiscard]] rendering::Draw_Elements_Command
  prepare_draw_connection(Vec2 cords_1, Vec2 cords_2);

} // namespace nebula
