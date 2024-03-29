#include <model/port.hpp>
#include <rendering/rendering.hpp>

namespace nebula {
  Port_Kind invert_port_kind(Port_Kind const kind)
  {
    return kind == Port_Kind::in ? Port_Kind::out : Port_Kind::in;
  }

  Port::Port(Vec2 const coordinates, Port_Kind const kind, Gate* gate)
    : coordinates(coordinates), kind(kind), gate(gate)
  {
    radius = 0.11f; // Adjust this value
  }

  void Port::move(nebula::Vec2 const offset)
  {
    coordinates.x += offset.x;
    coordinates.y += offset.y;
  }

  void Port::remove_connection(Port* old_port)
  {
    List_Iterator<Port*, Port*> it;
    for(it = connections.begin(); it != connections.end(); ++it) {
      if(*it == old_port) {
        connections.erase(it);
        break;
      }
    }
  }

  void Port::add_connection(Port* new_port)
  {
    // For IN type ports only one connection is allowed
    if(kind == Port_Kind::in && connections.size() > 0) {
      // Remove old connection
      Port* old_port = *(connections.begin());
      old_port->remove_connection(this);
      connections.erase_front();
    }
    connections.emplace_front(new_port);
  }

  void Port::remove_all_connections()
  {
    for(Port* port: connections) {
      port->remove_connection(this);
    }
    connections = {};
  }

  Vec2 Port::get_coordinates() const
  {
    return coordinates;
  }

  bool test_hit(Port const& port, Vec2 const point)
  {
    f32 const r2 = port.radius * port.radius;
    return math::length_squared(point - port.coordinates) <= r2;
  }

  rendering::Draw_Elements_Command prepare_draw(Port const& port)
  {
    math::Vec3 color;
    if(port.kind == Port_Kind::in) {
      color = {0.99f, 0.3f, 0.3f};
    } else {
      color = {0.6f, 0.9f, 0.2f};
    }
    Vertex vert[] = {
      Vertex{.position = {port.coordinates.x + port.radius,
                          port.coordinates.y + port.radius, 0.0f},
             .normal = color,
             .uv = {1.0f, 1.0f}},
      Vertex{.position = {port.coordinates.x - port.radius,
                          port.coordinates.y + port.radius, 0.0f},
             .normal = color,
             .uv = {0.0f, 1.0f}},
      Vertex{.position = {port.coordinates.x + port.radius,
                          port.coordinates.y - port.radius, 0.0f},
             .normal = color,
             .uv = {1.0f, 0.0f}},
      Vertex{.position = {port.coordinates.x - port.radius,
                          port.coordinates.y - port.radius, 0.0f},
             .normal = color,
             .uv = {0.0f, 0.0f}},
    };
    u32 indices[] = {
      0, 1, 2, 1, 3, 2,
    };
    rendering::Draw_Elements_Command cmd =
      rendering::write_geometry(vert, indices);
    cmd.instance_count = 1;
    return cmd;
  }

  rendering::Draw_Elements_Command prepare_draw_connection(Vec2 const cords_1,
                                                           Vec2 const cords_2)
  {
    // TODO: make it 'greener' when port output is true
    // Calculate normalized direction vector
    math::Vec2 direction = {cords_2.x - cords_1.x, cords_2.y - cords_1.y};
    f32 const length =
      math::sqrt(direction.x * direction.x + direction.y * direction.y);
    direction /= length;
    math::Vec2 const perpendicular = {-direction.y, direction.x};
    f32 const thickness = 0.04f;

    // Transform vertices using the normalized direction
    Vertex vert[] = {
      Vertex{.position = {cords_1.x + thickness * perpendicular.x,
                          cords_1.y + thickness * perpendicular.y, 0.0f},
             .normal = {0.5f, 0.8f, 0.5f},
             .uv = {1.0f, 1.0f}},
      Vertex{.position = {cords_1.x - thickness * perpendicular.x,
                          cords_1.y - thickness * perpendicular.y, 0.0f},
             .normal = {0.5f, 0.8f, 0.5f},
             .uv = {0.0f, 1.0f}},
      Vertex{.position = {cords_2.x + thickness * perpendicular.x,
                          cords_2.y + thickness * perpendicular.y, 0.0f},
             .normal = {0.5f, 0.8f, 0.5f},
             .uv = {1.0f, 0.0f}},
      Vertex{.position = {cords_2.x - thickness * perpendicular.x,
                          cords_2.y - thickness * perpendicular.y, 0.0f},
             .normal = {0.5f, 0.8f, 0.5f},
             .uv = {0.0f, 0.0f}},
    };
    u32 indices[] = {
      0, 1, 2, 1, 3, 2,
    };
    rendering::Draw_Elements_Command cmd =
      rendering::write_geometry(vert, indices);
    cmd.instance_count = 1;
    return cmd;
  }
} // namespace nebula
