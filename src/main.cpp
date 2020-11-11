#include <iostream>

#include "glad/glad.h"
//
#include "GLFW/glfw3.h"

int main() {
  // init glfw
  if (!glfwInit()) {
    std::cerr << "failed to initialize GLFW" << std::endl;
  }

  // set glfw error callback
  glfwSetErrorCallback([](int error, const char* description) {
    std::cerr << "Error: " << description << std::endl;
    std::exit(EXIT_FAILURE);
  });

  // setup window and context
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  GLFWwindow* window =
      glfwCreateWindow(512, 512, "GLSL CornellBox", nullptr, nullptr);
  if (!window) {
    std::cerr << "failed to create window" << std::endl;
    std::exit(EXIT_FAILURE);
  }
  glfwMakeContextCurrent(window);

  // initialize glad
  if (!gladLoadGL()) {
    std::cerr << "failed to initialize glad" << std::endl;
    std::exit(EXIT_FAILURE);
  }

  // set glfw key callback
  glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode,
                                int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
      glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
  });

  // main app loop
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
  }

  // exit
  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}