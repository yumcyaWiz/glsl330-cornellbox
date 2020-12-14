#include <iostream>
#include <memory>

#include "glad/glad.h"
//
#include "GLFW/glfw3.h"
//
#include "glm/glm.hpp"
//
#include "rectangle.h"
#include "renderer.h"
#include "shader.h"

const int width = 1024;
const int height = 1024;

std::unique_ptr<Renderer> renderer;

void keyCallback(GLFWwindow* window, int key, [[maybe_unused]] int scancode,
                 int action, [[maybe_unused]] int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  } else if (key == GLFW_KEY_C && action == GLFW_PRESS) {
    renderer->clear();
  }
}

int main() {
  // init glfw
  if (!glfwInit()) {
    std::cerr << "failed to initialize GLFW" << std::endl;
  }

  // set glfw error callback
  glfwSetErrorCallback([]([[maybe_unused]] int error, const char* description) {
    std::cerr << "Error: " << description << std::endl;
    std::exit(EXIT_FAILURE);
  });

  // setup window and context
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  GLFWwindow* window =
      glfwCreateWindow(width, height, "GLSL CornellBox", nullptr, nullptr);
  if (!window) {
    std::cerr << "failed to create window" << std::endl;
    std::exit(EXIT_FAILURE);
  }
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  // initialize glad
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "failed to initialize glad" << std::endl;
    std::exit(EXIT_FAILURE);
  }

  // set glfw key callback
  glfwSetKeyCallback(window, keyCallback);

  // setup renderer
  renderer = std::make_unique<Renderer>(width, height);

  // main app loop
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, width, height);

    renderer->render();

    glfwSwapBuffers(window);
  }

  // exit
  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}