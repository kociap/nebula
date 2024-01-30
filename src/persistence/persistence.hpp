#ifndef SAVE_HPP
#define SAVE_HPP

#include <ui/scene.hpp>
#include <fstream>
#include <string>

namespace nebula {
  class Persistence {
  public:
    static void saveToFile(const std::string& filename, const Scene& scene);
    static void loadFromFile(const std::string& filename, Scene& scene);
    static void saveConnectionsToFile(const std::string& filename, const Scene& scene);
    static void loadConnectionsFromFile(const std::string& filename, Scene& scene);

  private:
    static void saveGates(std::ofstream& file, const Scene& scene);
    static void loadGates(std::ifstream& file, Scene& scene);
    static void savePorts(std::ofstream& file, const Scene& scene);
    static void loadPorts(std::ifstream& file, Scene& scene);
    static void saveConnections(std::ofstream& file, const Scene& scene);
    static void loadConnections(std::ifstream& file, Scene& scene);
    static Port* findPortByCoordinates(const Scene& scene, const Vec2& coordinates);
  };
} // namespace nebula

#endif // SAVE_HPP