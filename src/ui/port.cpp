#include <rendering/rendering.hpp>
#include <ui/port.hpp>

namespace nebula {
  Port::Port(Vec2 const coordinates, port_t const type)
  {
    this->coordinates = coordinates;
    this->type = type;
    radius = 0.05f; // Adjust this value
  }

  void Port::move(nebula::Vec2 const offset)
  {
    coordinates.x += offset.x;
    coordinates.y += offset.y;
  }

  void Port::add_to_render_loop() const
  {
    // TODO: Add port to render loop
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
