#include <ui/ui.hpp>

namespace nebula {

  void UI::add_movable_gate(Vec2 const rectangle_dimensions,
                            math::Vec4 const camera_borders,
                            u8 const num_in_ports, u8 const num_out_ports)
  {
    Movable_Gate new_gate = Movable_Gate(rectangle_dimensions, camera_borders,
                                         num_in_ports, num_out_ports);
    gates.emplace_front(new_gate);
    for(Port* p: new_gate.in_ports) {
      ports.emplace_front(p);
    }
    for(Port* p: new_gate.out_ports) {
      ports.emplace_front(p);
    }
  }

  Movable_Gate* UI::check_if_gate_clicked(Vec2 const mouse_position)
  {
    for(Movable_Gate& mg: gates) {
      if(mg.is_under_mouse(mouse_position)) {
        return &mg;
      }
    }
    return nullptr;
  }

  Port* UI::check_if_port_clicked(Vec2 const mouse_position)
  {
    for(Port* p: ports) {
      if(p->is_under_mouse(mouse_position)) {
        return p;
      }
    }
    return nullptr;
  }

  void UI::add_gates_to_render_loop() const
  {
    for(const Movable_Gate& mg: gates) {
      // Draw gate
      mg.add_to_render_loop();
      // Draw in ports
      for(Port* p: mg.in_ports) {
        p->add_to_render_loop();
      }
      // Draw out ports
      for(Port* p: mg.out_ports) {
        p->add_to_render_loop();
      }
    }
  }
} // namespace nebula
