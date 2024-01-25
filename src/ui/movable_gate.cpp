#include <iostream>
#include <rendering/rendering.hpp>
#include <ui/movable_gate.hpp>

namespace nebula {
  Movable_Gate::Movable_Gate(math::Vec2 const rectangle_dimensions,
                             math::Vec4 const camera_borders,
                             u8 const num_in_ports, u8 const num_out_ports)
  {
    // Center the rectangle on the screen
    math::Vec2 coordinates;
    // Center of the camera - half of rectangle width = x
    coordinates.x = ((camera_borders.y + camera_borders.x) / (f32)2.0) -
                    (rectangle_dimensions.x / (f32)2.0);
    // Center of the camera - half of rectangle height = y
    coordinates.y = ((camera_borders.z + camera_borders.w) / (f32)2.0) -
                    (rectangle_dimensions.y / (f32)2.0);

    rect = {coordinates, rectangle_dimensions};

    // Distance between IN ports
    f32 in_dist = rectangle_dimensions.y / static_cast<f32>(num_in_ports + 1);
    for(u8 i = 1; i <= num_in_ports; i++) {
      // Create IN ports along the left edge of the gate
      in_ports.emplace_front(
        new Port({coordinates.x, coordinates.y - in_dist * static_cast<f32>(i)},
                 port_t::in));
    }

    // Distance between OUT ports
    f32 out_dist = rectangle_dimensions.y / static_cast<f32>(num_out_ports + 1);
    for(u8 i = 1; i <= num_out_ports; i++) {
      // Create OUT ports along the right edge of the gate
      out_ports.emplace_front(
        new Port({coordinates.x + rectangle_dimensions.x,
                  coordinates.y - out_dist * static_cast<f32>(i)},
                 port_t::out));
    }
  }

  void Movable_Gate::add_to_render_loop() const
  {
    Vertex vert[] = {
      Vertex{.position = {rect.coordinates.x + rect.dimensions.x,
                          rect.coordinates.y + rect.dimensions.y, 0.0f},
             .normal = {},
             .uv = {1.0f, 1.0f}},
      Vertex{.position = {rect.coordinates.x,
                          rect.coordinates.y + rect.dimensions.y, 0.0f},
             .normal = {},
             .uv = {0.0f, 1.0f}},
      Vertex{.position = {rect.coordinates.x + rect.dimensions.x,
                          rect.coordinates.y, 0.0f},
             .normal = {},
             .uv = {1.0f, 0.0f}},
      Vertex{.position = {rect.coordinates.x, rect.coordinates.y, 0.0f},
             .normal = {},
             .uv = {0.0f, 0.0f}},
    };
    u32 indices[] = {
      0, 1, 2, 1, 3, 2,
    };
    rendering::Draw_Elements_Command cmd =
      rendering::write_geometry(vert, indices);
    cmd.instance_count = 1;

    rendering::add_draw_command(cmd);
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
