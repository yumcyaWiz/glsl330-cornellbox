#include <iostream>

#include "GLFW/glfw3.h"

int main() {
  if (!glfwInit()) {
    std::cerr << "failed to initialize GLFW" << std::endl;
  }

  glfwSetErrorCallback([](int error, const char* description) {
    std::cerr << "Error: " << description << std::endl;
  });

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  GLFWwindow* window =
      glfwCreateWindow(512, 512, "GLSL CornellBox", nullptr, nullptr);
  if (!window) {
    std::cerr << "failed to create window" << std::endl;
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}