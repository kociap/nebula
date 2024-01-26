#include <rendering/rendering.hpp>
#include <ui/port.hpp>

namespace nebula {
  Port::Port(Vec2 const coordinates, port_t const type)
  {
    this->coordinates = coordinates;
    this->type = type;
    radius = 0.01f; // Adjust this value
  }

  void Port::move(nebula::Vec2 const offset)
  {
    coordinates.x += offset.x;
    coordinates.y += offset.y;
  }

  void Port::add_to_render_loop() const
  {
    math::Vec3 color;
    if(type == port_t::in) {
      color = {0.99f, 0.3f, 0.3f};
    } else {
      color = {0.6f, 0.9f, 0.2f};
    }
    Vertex vert[] = {
      Vertex{.position = {coordinates.x + radius, coordinates.y + radius, 0.0f},
             .normal = color,
             .uv = {1.0f, 1.0f}},
      Vertex{.position = {coordinates.x - radius, coordinates.y + radius, 0.0f},
             .normal = color,
             .uv = {0.0f, 1.0f}},
      Vertex{.position = {coordinates.x + radius, coordinates.y - radius, 0.0f},
             .normal = color,
             .uv = {1.0f, 0.0f}},
      Vertex{.position = {coordinates.x - radius, coordinates.y - radius, 0.0f},
             .normal = color,
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

  void Port::remove_connection(Port* old_port)
  {
    List_Iterator<Port*, Port*> it;
    for(it = connections.begin(); it != connections.end(); it++) {
      if(*it == old_port) {
        connections.erase(it);
        break;
      }
    }
  }

  void Port::add_connection(Port* new_port)
  {
    // For IN type ports only one connection is allowed
    if(type == port_t::in && connections.size() > 0) {
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

  bool Port::is_under_mouse(Vec2 const mouse_position) const
  {
    f32 const distance =
      math::sqrt(math::pow(mouse_position.x - coordinates.x, 2) +
                 math::pow(mouse_position.y - coordinates.y, 2));
    bool const clicked = (distance <= radius);
    return clicked;
  }
} // namespace nebula
