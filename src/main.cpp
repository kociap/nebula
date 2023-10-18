#include <glad/glad.h>

// GLAD must be included first. clang-format sorts includes, hence this comment
// here.

#include <GLFW/glfw3.h>

int main(int argc, char* argv[])
{
  (void)argc;
  (void)argv;

  // Initialize GLFW
  if(!glfwInit()) {
    return -1;
  }
  // Create a GLFW window
  GLFWwindow* window =
    glfwCreateWindow(800, 600, "Hello GLFW", nullptr, nullptr);

  if(!window) {
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  // Initialize Glad
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    glfwTerminate();
    return -1;
  }

  // Main loop
  while(!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();

  return 0;
}
