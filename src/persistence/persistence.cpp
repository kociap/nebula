#include "persistence/persistence.hpp"
#include <iostream>

namespace nebula {
  void Save::saveToFile(const std::string& filename, const Scene& scene)
  {
    std::ofstream file(filename);

    if(file.is_open()) {
      saveGates(file, scene);
      savePorts(file, scene);

      file.close();
    } else {
      std::cerr << "Error: Unable to open file for saving: " << filename
                << std::endl;
    }
  }

  void Save::loadFromFile(const std::string& filename, Scene& scene)
  {
    std::ifstream file(filename);

    if(file.is_open()) {
      loadGates(file, scene);
      loadPorts(file, scene);

      file.close();
    } else {
      std::cerr << "Error: Unable to open file for loading: " << filename
                << std::endl;
    }
  }

  void Save::saveGates(std::ofstream& file, const Scene& scene)
  {
    for(const Gate& gate: scene.gates) {
      file << "gate " << gate.dimensions.x << " " << gate.dimensions.y << " "
           << gate.coordinates.x << " " << gate.coordinates.y << " "
           << static_cast<int>(gate.kind) << "\n";
    }
  }

  void Save::loadGates(std::ifstream& file, Scene& scene)
  {
    while(file) {
      std::string type;
      file >> type;

      if(type == "gate") {
        Vec2 dimensions, coordinates;
        int kind;
        file >> dimensions.x >> dimensions.y >> coordinates.x >>
          coordinates.y >> kind;

        Gate_Kind gateKind = static_cast<Gate_Kind>(kind);
        scene.add_gate(dimensions, coordinates, gateKind);
      }
    }
  }

  void Save::savePorts(std::ofstream& file, const Scene& scene)
  {
    for(const Port* port: scene.ports) {
      file << "port " << port->coordinates.x << " " << port->coordinates.y
           << " " << static_cast<int>(port->kind) << "\n";
    }
  }

  void Save::loadPorts(std::ifstream& file, Scene& scene)
  {
    while(file) {
      std::string type;
      file >> type;

      if(type == "port") {
        Vec2 coordinates;
        int kind;
        file >> coordinates.x >> coordinates.y >> kind;

        Port_Kind portKind = static_cast<Port_Kind>(kind);
        Port* newPort = new Port(coordinates, portKind, nullptr);
        scene.ports.emplace_back(newPort);
      }
    }
  }
} // namespace nebula