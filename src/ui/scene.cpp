#include <ui/scene.hpp>

namespace nebula {
  Scene::~Scene()
  {
    for(Port* p: ports) {
      delete p;
    }
  }

  void Scene::add_gate(Vec2 const rectangle_dimensions,
                       math::Vec2 const coordinates, u8 const num_in_ports,
                       u8 const num_out_ports)
  {
    Gate new_gate =
      Gate(rectangle_dimensions, coordinates, num_in_ports, num_out_ports);
    gates.emplace_back(new_gate);
    for(Port* p: new_gate.in_ports) {
      ports.push_back(p);
    }
    for(Port* p: new_gate.out_ports) {
      ports.push_back(p);
    }
  }

  Gate* Scene::check_if_gate_clicked(Vec2 const mouse_position)
  {
    for(Gate& mg: gates) {
      if(test_hit(mg, mouse_position)) {
        return &mg;
      }
    }
    return nullptr;
  }

  Port* Scene::check_if_port_clicked(Vec2 const mouse_position)
  {
    u32 range = (tmp_port_exists) ? ports.size() - 1 : ports.size();
    for(u32 i = 0; i < range; i++) {
      if(ports[i]->is_under_mouse(mouse_position)) {
        return ports[i];
      }
    }
    return nullptr;
  }

  void Scene::create_tmp_port(Port* p, Vec2 const coordinates,
                              port_t const type)
  {
    Port* tmp_port = new Port(coordinates, type);
    ports.emplace_back(tmp_port);
    p->add_connection(tmp_port);
    tmp_port->add_connection(p);
    tmp_port_exists = true;
  }

  void Scene::connect_ports(Port* p1, Port* p2)
  {
    remove_tmp_port(p1);
    p1->add_connection(p2);
    p2->add_connection(p1);
  }

  void Scene::move_tmp_port(Vec2 const offset)
  {
    ports.back()->move(offset);
  }

  void Scene::remove_tmp_port(Port* p)
  {
    Port* tmp_port = ports.back();
    p->remove_connection(tmp_port);
    ports.pop_back();
    delete tmp_port;
    tmp_port_exists = false;
  }
} // namespace nebula
