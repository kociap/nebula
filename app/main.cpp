#include <project/tmp.hpp>
#include <GLFW/glfw3.h>

int main(int argc, char* argv[]) {
  (void) argc;
  (void) argv;

  // Initialize GLFW
  if (!glfwInit()) {
    return -1;
  }
  // Create a GLFW window
  GLFWwindow* window = glfwCreateWindow(tmp::add(400, 400), 600, "Hello GLFW", nullptr, nullptr);

  if (!window) {
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  // Main loop
  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();

  return 0;
}


