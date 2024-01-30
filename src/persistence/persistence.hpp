#ifndef SAVE_HPP
#define SAVE_HPP

#include "ui/scene.hpp"
#include <fstream>
#include <string>

namespace nebula {
  class Save {
  public:
    static void saveToFile(const std::string& filename, const Scene& scene);
    static void loadFromFile(const std::string& filename, Scene& scene);

  private:
    static void saveGates(std::ofstream& file, const Scene& scene);
    static void loadGates(std::ifstream& file, Scene& scene);
    static void savePorts(std::ofstream& file, const Scene& scene);
    static void loadPorts(std::ifstream& file, Scene& scene);
  };
} // namespace nebula

#endif // SAVE_HPP