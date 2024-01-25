#pragma once
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
                          math::Vec2 window_dimensions, u8 num_in_ports,
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
    void add_gates_to_render_loop() const;

  private:
    List<Movable_Gate> gates; // All gates visible on the screen
    List<Port*> ports; // All ports visible on the screen
  };
} // namespace nebula
