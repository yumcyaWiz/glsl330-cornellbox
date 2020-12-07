#include <iostream>

#include "glad/glad.h"
//
#include "GLFW/glfw3.h"
//
#include "glm/glm.hpp"
//
#include "rectangle.h"
#include "shader.h"

const int width = 1024;
const int height = 1024;
int samples = 0;

GLuint accumFBO;
GLuint accumTexture;

void keyCallback(GLFWwindow* window, int key, int scancode, int action,
                 int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  } else if (key == GLFW_KEY_C && action == GLFW_PRESS) {
    // clear accumTexture
    glBindTexture(GL_TEXTURE_2D, accumTexture);
    std::vector<GLfloat> data(4 * width * height);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_FLOAT,
                    data.data());
    glBindTexture(GL_TEXTURE_2D, 0);

    samples = 0;
  }
}

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
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  GLFWwindow* window =
      glfwCreateWindow(width, height, "GLSL CornellBox", nullptr, nullptr);
  if (!window) {
    std::cerr << "failed to create window" << std::endl;
    std::exit(EXIT_FAILURE);
  }
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  // set glfw key callback
  glfwSetKeyCallback(window, keyCallback);

  // initialize glad
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "failed to initialize glad" << std::endl;
    std::exit(EXIT_FAILURE);
  }

  Rectangle rectangle;

  // setup accumulate FBO
  glGenFramebuffers(1, &accumFBO);
  glBindFramebuffer(GL_FRAMEBUFFER, accumFBO);
  // setup texture
  glGenTextures(1, &accumTexture);
  glBindTexture(GL_TEXTURE_2D, accumTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA,
               GL_FLOAT, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         accumTexture, 0);

  // setup shaders
  Shader pt_shader("./shaders/pt.vert", "./shaders/pt.frag");
  pt_shader.compileShader();
  pt_shader.linkShader();

  Shader output_shader("./shaders/pt.vert", "./shaders/output.frag");
  output_shader.compileShader();
  output_shader.linkShader();

  // main app loop
  const glm::vec2 resolution = glm::vec2(width, height);
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, width, height);

    // path tracing
    pt_shader.setUniform("time", static_cast<float>(glfwGetTime()));
    pt_shader.setUniform("samples", samples);
    pt_shader.setUniform("resolution", resolution);
    pt_shader.setUniform("accumTexture", 0);
    glBindFramebuffer(GL_FRAMEBUFFER, accumFBO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, accumTexture);
    rectangle.draw(pt_shader);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // output
    output_shader.setUniform("time", static_cast<float>(glfwGetTime()));
    output_shader.setUniform("samples", samples);
    output_shader.setUniform("resolution", resolution);
    output_shader.setUniform("accumTexture", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, accumTexture);
    rectangle.draw(output_shader);

    glfwSwapBuffers(window);

    // update uniforms
    samples++;
  }

  // exit
  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}