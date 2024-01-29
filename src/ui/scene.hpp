#pragma once

#include <core/types.hpp>
#include <model/gate.hpp>

namespace nebula {
  /**
   * @brief Enumeration representing different modes for the application window.
   */
  enum struct Window_Mode {
    none, ///< No specific mode.
    camera_moving, ///< Moving the camera mode.
    gate_moving, ///< Moving a gate mode.
    port_linking, ///< Linking ports mode.
    object_delete, ///< Deleting objects mode.
  };

  /**
   * @brief Representation of a scene in the application.
   */
  struct Scene {
  public:
    Window_Mode mode =
      Window_Mode::none; ///< The current mode of the application window.
    Vec2 last_mouse_position; ///< The last known mouse position.
    Gate* currently_moved_gate =
      nullptr; ///< Pointer to the gate currently being moved.
    Port* connected_port =
      nullptr; ///< Pointer to the port being connected during linking.
    List<Gate*> gates; ///< List of all gates visible on the screen.
    Array<Port*> ports; ///< Array of all ports visible on the screen.
    Vec2 viewport_size = {1920, 1080};

  private:
    bool tmp_port_exists =
      false; ///< Flag indicating whether a temporary port exists.

  public:
    ~Scene();

    /**
     * @brief Adds a new gate with the specified dimensions and coordinates to the center of the screen.
     *
     * This function creates a new gate with the specified dimensions and coordinates, adding it to the list of gates.
     *
     * @param dimensions The dimensions of the new gate.
     * @param coordinates The coordinates of the new gate.
     * @param kind The kind of gate to be created.
     */
    void add_gate(math::Vec2 dimensions, math::Vec2 coordinates,
                  Gate_Kind kind);

    /**
     * @brief Deletes the specified gate from the scene.
     *
     * This function deletes the specified gate from the list of gates.
     *
     * @param gate Pointer to the gate to be deleted.
     */
    void delete_gate(Gate* gate);

    /**
     * @brief Checks if any gate has been clicked at the given mouse position.
     *
     * This function cycles through all gates to check if any has been clicked based on the provided mouse position.
     *
     * @param mouse_position The position of the mouse.
     * @return Pointer to the gate that has been clicked, or nullptr if no gate was clicked.
     */
    [[nodiscard]] Gate* check_if_gate_clicked(Vec2 mouse_position);

    /**
     * @brief Checks if any port has been clicked at the given mouse position.
     *
     * This function cycles through all ports to check if any has been clicked based on the provided mouse position.
     *
     * @param mouse_position The position of the mouse.
     * @return Pointer to the port that has been clicked, or nullptr if no port was clicked.
     */
    [[nodiscard]] Port* check_if_port_clicked(Vec2 mouse_position);

    /**
     * @brief Creates a temporary port used to render a connection.
     *
     * This function creates a temporary port at the specified coordinates with the given type.
     *
     * @param p The port to connect to during linking.
     * @param coordinates The coordinates of the temporary port.
     * @param type The type of the temporary port (IN or OUT).
     */
    void create_tmp_port(Port* p, Vec2 coordinates, Port_Kind type);

    /**
     * @brief Removes the temporary port created for linking.
     *
     * This function removes the temporary port created for linking when the linking mode ends.
     *
     * @param p The port that the temporary port was connected to.
     */
    void remove_tmp_port(Port* p);

    /**
     * @brief Connects two ports and removes the temporary port.
     *
     * This function connects two ports and removes the temporary port used for linking.
     *
     * @param p1 The first port to connect.
     * @param p2 The second port to connect.
     */
    void connect_ports(Port* p1, Port* p2);

    /**
     * @brief Moves the temporary port by the specified offset.
     *
     * This function moves the temporary port by the specified offset.
     *
     * @param offset The offset vector.
     */
    void move_tmp_port(Vec2 offset);
  };

} // namespace nebula
