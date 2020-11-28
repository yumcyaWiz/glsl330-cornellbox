#include <iostream>

#include "glad/glad.h"
//
#include "GLFW/glfw3.h"
//
#include "glm/glm.hpp"
//
#include "shader.h"

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
      glfwCreateWindow(512, 512, "GLSL CornellBox", nullptr, nullptr);
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
  glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode,
                                int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
      glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
  });

  // setup VAO
  GLuint VAO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  // setup VBO;
  GLfloat vertices[] = {-1.0f, -1.0f, 0.0f, 1.0f,  -1.0f, 0.0f,
                        1.0f,  1.0f,  0.0f, -1.0f, 1.0f,  0.0f};
  GLuint VBO;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // setup EBO;
  GLuint indices[] = {0, 1, 2, 2, 3, 0};
  GLuint EBO;
  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  // position attribute
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat),
                        (GLvoid*)0);

  // setup accumulate FBO
  GLuint accumFBO;
  glGenFramebuffers(1, &accumFBO);
  glBindFramebuffer(GL_FRAMEBUFFER, accumFBO);
  // setup texture
  GLuint accumTexture;
  glGenTextures(1, &accumTexture);
  glBindTexture(GL_TEXTURE_2D, accumTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 512, 512, 0, GL_RGBA, GL_FLOAT, 0);
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
  int samples = 0;
  const glm::vec2 resolution = glm::vec2(512, 512);
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, 512, 512);

    // path tracing
    pt_shader.useShader();
    pt_shader.setUniform("time", static_cast<float>(glfwGetTime()));
    pt_shader.setUniform("samples", samples);
    pt_shader.setUniform("resolution", resolution);
    glBindFramebuffer(GL_FRAMEBUFFER, accumFBO);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // output
    output_shader.useShader();
    output_shader.setUniform("time", static_cast<float>(glfwGetTime()));
    output_shader.setUniform("samples", samples);
    output_shader.setUniform("resolution", resolution);
    output_shader.setUniform("accumTexture", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, accumTexture);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glfwSwapBuffers(window);

    // update uniforms
    samples++;
  }

  // exit
  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}