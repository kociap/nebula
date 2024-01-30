#include "persistence/persistence.hpp"
#include <iostream>

namespace nebula {
  void Save::saveToFile(const std::string& filename, const Scene& scene)
  {
    std::ofstream file(filename);

    if(file.is_open()) {
      saveGates(file, scene);
      savePorts(file, scene);
      saveConnections(file, scene);

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
      loadConnections(file, scene);

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
    file<<"endgates\n";
  }

  void Save::loadGates(std::ifstream& file, Scene& scene)
  {
    while(file) {
      std::string type;
      file >> type;
      if(type == "endgates"){
        return;
      }
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
    file<<"endports\n";
  }

  void Save::loadPorts(std::ifstream& file, Scene& scene)
  {
    while(file) {
      std::string type;
      file >> type;
      if(type == "endports"){
        return;
      }
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

  // Add these two functions to Save class
void Save::saveConnectionsToFile(const std::string& filename, const Scene& scene)
{
    std::ofstream file(filename);

    if (file.is_open()) {
        saveConnections(file, scene);
        file.close();
    }
    else {
        std::cerr << "Error: Unable to open file for saving connections: " << filename << std::endl;
    }
}

void Save::loadConnectionsFromFile(const std::string& filename, Scene& scene)
{
    std::ifstream file(filename);

    if (file.is_open()) {
        loadConnections(file, scene);
        file.close();
    }
    else {
        std::cerr << "Error: Unable to open file for loading connections: " << filename << std::endl;
    }
}

void Save::saveConnections(std::ofstream& file, const Scene& scene)
{
    for (const Port* port : scene.ports) {
        for (const Port* connection : port->connections) {
            file << "connection " << port->get_coordinates().x << " " << port->get_coordinates().y << " "
                 << connection->get_coordinates().x << " " << connection->get_coordinates().y << "\n";
        }
    }
    file<<"endconnections\n";
}

void Save::loadConnections(std::ifstream& file, Scene& scene)
{
    
    while (file) {
        std::string type;
        file >> type;
        if(type == "endconnections"){
        return;
      }
        
        if (type == "connection") {
            Vec2 source, destination;
            file >> source.x >> source.y >> destination.x >> destination.y;

            // Find the corresponding ports in the scene and establish the connection
            Port* sourcePort = findPortByCoordinates(scene, source);
            Port* destinationPort = findPortByCoordinates(scene, destination);

            if (sourcePort && destinationPort) {
                std::cout<<source[0]<<" "<<source[1]<<":"<<destination[0]<<" "<<destination[0]<<std::endl;
                sourcePort->add_connection(destinationPort);
            }
        }
    }
}

Port* Save::findPortByCoordinates(const Scene& scene, const Vec2& coordinates)
{
    for (const Port* port : scene.ports) {
        if (port->get_coordinates() == coordinates) {
            return const_cast<Port*>(port); // Const-cast to maintain compatibility with add_connection
        }
    }
    return nullptr;
}

} // namespace nebula