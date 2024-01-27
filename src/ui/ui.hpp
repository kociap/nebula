#pragma once
#include <anton/math/vec4.hpp>
#include <core/types.hpp>
#include <ui/movable_gate.hpp>

namespace nebula {
  struct UI {
  public:
    /**
     * Creates a new gate with the specified number of ports in the center of the screen
     * @param num_in_ports - number of IN type ports
     * @param num_out_ports - number of OUT type ports
     */
    void add_movable_gate(math::Vec2 rectangle_dimensions,
                          math::Vec2 coordinates, u8 num_in_ports,
                          u8 num_out_ports);

    /**
     * It cycles through all gates to see if any have been clicked.
     */
    [[nodiscard]] Movable_Gate* check_if_gate_clicked(Vec2 mouse_position);

    /**
     * It cycles through all ports to see if any have been clicked.
     */
    [[nodiscard]] Port* check_if_port_clicked(Vec2 mouse_position);

    /**
     * Adds all gates and their ports to the render loop.
     */
    void add_gates_to_render_loop();

    /**
     * Creates temporary port used to render connection.
     * @param coordinates - scene position
     * @param type - port_t
     */
    void create_tmp_port(Port* p, Vec2 coordinates, port_t type);

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

    ~UI();

  private:
    List<Movable_Gate> gates; // All gates visible on the screen
    Array<Port*> ports; // All ports visible on the screen
    bool tmp_port_exists = false;
  };
} // namespace nebula
