#pragma once

#include <core/types.hpp>
#include <ui/gate.hpp>

namespace nebula {
  enum struct Window_Mode {
    none,
    camera_moving,
    gate_moving,
    port_linking,
  };

  struct Scene {
  public:
    Window_Mode mode = Window_Mode::none;
    Vec2 last_mouse_position;
    Gate* currently_moved_gate = nullptr;
    Port* connected_port = nullptr;
    List<Gate> gates; // All gates visible on the screen
    Array<Port*> ports; // All ports visible on the screen
  private:
    bool tmp_port_exists = false;

  public:
    ~Scene();

    /**
     * Creates a new gate with the specified number of ports in the center of
     * the screen
     */
    void add_gate(math::Vec2 dimensions, math::Vec2 coordinates,
                  Gate_Kind kind);

    /**
     * It cycles through all gates to see if any have been clicked.
     */
    [[nodiscard]] Gate* check_if_gate_clicked(Vec2 mouse_position);

    /**
     * It cycles through all ports to see if any have been clicked.
     */
    [[nodiscard]] Port* check_if_port_clicked(Vec2 mouse_position);

    /**
     * Creates temporary port used to render connection.
     * @param coordinates - scene position
     * @param type - Port_Kind
     */
    void create_tmp_port(Port* p, Vec2 coordinates, Port_Kind type);

    /**
     * Removes tmp port when linking mode ends.
     * @param p - port it was connected to
     */
    void remove_tmp_port(Port* p);

    /**
     * Connect port p1 and p2. Remove tmp port.
     */
    void connect_ports(Port* p1, Port* p2);

    void move_tmp_port(Vec2 offset);
  };
} // namespace nebula
