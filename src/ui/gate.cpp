#include <rendering/rendering.hpp>
#include <ui/gate.hpp>

namespace nebula {
  Gate::Gate(math::Vec2 const _dimensions, math::Vec2 const _coordinates,
             u8 const num_in_ports, u8 const num_out_ports)
    : coordinates(_coordinates), dimensions(_dimensions)
  {
    // Center the rectangle on the screen
    coordinates += dimensions / 2.0f;

    // Distance between IN ports
    f32 in_dist = dimensions.y / static_cast<f32>(num_in_ports + 1);
    for(u8 i = 1; i <= num_in_ports; i++) {
      // Create IN ports along the left edge of the gate
      Vec2 port_coordinates = {coordinates.x,
                               coordinates.y + in_dist * static_cast<f32>(i)};
      in_ports.emplace_back(new Port(port_coordinates, port_t::in));
    }

    // Distance between OUT ports
    f32 out_dist = dimensions.y / static_cast<f32>(num_out_ports + 1);
    for(u8 i = 1; i <= num_out_ports; i++) {
      // Create OUT ports along the right edge of the gate
      Vec2 port_coordinates = {coordinates.x + dimensions.x,
                               coordinates.y + out_dist * static_cast<f32>(i)};
      out_ports.emplace_back(new Port(port_coordinates, port_t::out));
    }
  }

  void Gate::add_to_render_loop() const
  {
    Vertex vert[] = {
      Vertex{.position = {coordinates.x + dimensions.x,
                          coordinates.y + dimensions.y, 0.0f},
             .normal = {0.4f, 0.4f, 0.7f},
             .uv = {1.0f, 1.0f}},
      Vertex{.position = {coordinates.x, coordinates.y + dimensions.y, 0.0f},
             .normal = {0.4f, 0.4f, 0.7f},
             .uv = {0.0f, 1.0f}},
      Vertex{.position = {coordinates.x + dimensions.x, coordinates.y, 0.0f},
             .normal = {0.4f, 0.4f, 0.7f},
             .uv = {1.0f, 0.0f}},
      Vertex{.position = {coordinates.x, coordinates.y, 0.0f},
             .normal = {0.4f, 0.4f, 0.7f},
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

  void Gate::move(math::Vec2 const offset)
  {
    coordinates.x += offset.x;
    coordinates.y += offset.y;
    // Move ports
    for(Port* p: in_ports) {
      p->move(offset);
    }
    for(Port* p: out_ports) {
      p->move(offset);
    }
  }

  bool test_hit(Gate const& gate, math::Vec2 const point)
  {
    Vec2 const coordinates = gate.coordinates;
    Vec2 const dimensions = gate.dimensions;
    bool const left = point.x >= coordinates.x;
    bool const right = point.x <= coordinates.x + dimensions.x;
    bool const top = point.y >= coordinates.y;
    bool const bottom = point.y <= coordinates.y + dimensions.y;
    return left && right && top && bottom;
  }
} // namespace nebula
