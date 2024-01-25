#include <ui/movable_gate.hpp>

namespace nebula {
  Movable_Gate::Movable_Gate(math::Vec2 const rectangle_dimensions,
                             math::Vec2 const window_dimensions,
                             u8 const num_in_ports, u8 const num_out_ports)
  {
    // Center the rectangle on the screen
    math::Vec2 coordinates;
    // Half of window width - half of rectangle width = x
    coordinates.x =
      (window_dimensions.x / (f32)2.0) - (rectangle_dimensions.x / (f32)2.0);
    // Half of window height + half of rectangle height = y
    coordinates.y =
      (window_dimensions.y / (f32)2.0) + (rectangle_dimensions.y / (f32)2.0);

    rect = {coordinates, rectangle_dimensions};

    // Distance between IN ports
    f32 in_dist = rectangle_dimensions.y / static_cast<f32>(num_in_ports + 1);
    for(u8 i = 1; i <= num_in_ports; i++) {
      // Create IN ports along the left edge of the gate
      in_ports.emplace_front(new Port(
        {coordinates.x, coordinates.y - in_dist * static_cast<f32>(i)}, IN));
    }

    // Distance between OUT ports
    f32 out_dist = rectangle_dimensions.y / static_cast<f32>(num_out_ports + 1);
    for(u8 i = 1; i <= num_out_ports; i++) {
      // Create OUT ports along the right edge of the gate
      out_ports.emplace_front(
        new Port({coordinates.x + rectangle_dimensions.x,
                  coordinates.y - out_dist * static_cast<f32>(i)},
                 OUT));
    }
  }

  void Movable_Gate::Movable_Gate::add_to_render_loop() const
  {
    // TODO: Adding gate to render loop
  }

  bool Movable_Gate::is_under_mouse(math::Vec2 const mouse_location) const
  {
    return rect.is_under_point(mouse_location);
  }

  void Movable_Gate::move(math::Vec2 const offset)
  {
    rect.coordinates.x += offset.x;
    rect.coordinates.y += offset.y;
    // Move ports
    for(Port* p: in_ports) {
      p->move(offset);
    }
    for(Port* p: out_ports) {
      p->move(offset);
    }
  }

  Movable_Gate::~Movable_Gate()
  {
    for(Port* p: in_ports) {
      delete p;
    }
    for(Port* p: out_ports) {
      delete p;
    }
  }

} // namespace nebula
