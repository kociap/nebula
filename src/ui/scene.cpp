#include <ui/scene.hpp>

namespace nebula {
  Scene::~Scene()
  {
    for(Port* p: ports) {
      delete p;
    }
  }

  void Scene::add_gate(Vec2 const dimensions, math::Vec2 const coordinates,
                       Gate_Kind const kind)
  {
    Gate& gate = *gates.emplace_back(dimensions, coordinates, kind);
    for(Port* p: gate.in_ports) {
      ports.push_back(p);
    }
    for(Port* p: gate.out_ports) {
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
      if(test_hit(*ports[i], mouse_position)) {
        return ports[i];
      }
    }
    return nullptr;
  }

  void Scene::create_tmp_port(Port* p, Vec2 const coordinates,
                              Port_Kind const type)
  {
    Port* tmp_port = new Port(coordinates, type, nullptr);
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

  void Scene::delete_gate(Gate* gate)
  {
    for(Port* p: gate->in_ports) {
      // Remove all IN ports from ports list
      for(auto it = ports.begin(); it != ports.end(); ++it) {
        auto c = it;
        if((*it) == p) {
          ports.erase(c, ++it);
          break;
        }
      }
      // Remove connections
      p->remove_all_connections();
      delete p;
    }

    for(Port* p: gate->out_ports) {
      // Remove all OUT ports from ports list
      for(auto it = ports.begin(); it != ports.end(); ++it) {
        auto c = it;
        if((*it) == p) {
          ports.erase(c, ++it);
          break;
        }
      }
      p->remove_all_connections();
      delete p;
    }
    currently_moved_gate = nullptr;

    // Remove gate from gates list
    for(auto it = gates.begin(); it != gates.end(); ++it) {
      if(&(*it) == gate) {
        gates.erase(it);
        break;
      }
    }
  }

  void Scene::toggle_evaluation_mode()
  {
    // if(mode == Window_Mode::evaluation_mode) {
    //   mode = Window_Mode::none;
    //   return;
    // }
    // if(mode == Window_Mode::port_linking) {
    //   remove_tmp_port(connected_port);
    // }
    // mode = Window_Mode::evaluation_mode;
  }

  void Scene::set_window_mode(Window_Mode _mode)
  {
    // if(mode == Window_Mode::evaluation_mode) {
    //   return;
    // }
    mode = _mode;
  }

  Gate* test_hit_gates(Scene& scene, Vec2 const point)
  {
    for(Gate& gate: scene.gates) {
      bool const hit = test_hit(gate, point);
      if(hit) {
        return &gate;
      }
    }
    return nullptr;
  }

  Port* test_hit_ports(Scene& scene, Vec2 const point)
  {
    i64 const range =
      (scene.tmp_port_exists) ? scene.ports.size() - 1 : scene.ports.size();
    for(i64 i = 0; i < range; i++) {
      if(test_hit(*scene.ports[i], point)) {
        return scene.ports[i];
      }
    }
    return nullptr;
  }
} // namespace nebula
