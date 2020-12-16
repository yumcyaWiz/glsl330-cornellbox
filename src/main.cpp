#include <iostream>
#include <memory>

#include "glad/glad.h"
//
#include "GLFW/glfw3.h"
//
#include "glm/glm.hpp"
//
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
//
#include "rectangle.h"
#include "renderer.h"
#include "shader.h"

std::unique_ptr<Renderer> renderer;

void keyCallback(GLFWwindow* window, int key, [[maybe_unused]] int scancode,
                 int action, [[maybe_unused]] int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  } else if (key == GLFW_KEY_C && action == GLFW_PRESS) {
    renderer->clear();
  }

  // Camera Movement
  /*
  const float movementSpeed = 10.0f;
  if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
    renderer->moveCamera(movementSpeed * glm::vec3(0, 0, 1.0));
  }
  if (key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
    renderer->moveCamera(movementSpeed * glm::vec3(0.0, 0, -1.0));
  }
  if (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
    renderer->moveCamera(movementSpeed * glm::vec3(1.0, 0, 0.0));
  }
  if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
    renderer->moveCamera(movementSpeed * glm::vec3(-1.0, 0, 0.0));
  }
  */
}

void handleInput(GLFWwindow* window, const ImGuiIO& io) {
  // Close Application
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }

  // Clear Render
  if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
    renderer->clear();
  }

  // Camera Movement
  const float movementSpeed = 100.0f * io.DeltaTime;
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    renderer->moveCamera(movementSpeed * glm::vec3(0, 0, 1.0));
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    renderer->moveCamera(movementSpeed * glm::vec3(0, 0, -1.0));
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    renderer->moveCamera(movementSpeed * glm::vec3(1.0, 0, 0.0));
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    renderer->moveCamera(movementSpeed * glm::vec3(-1.0, 0, 0.0));
  }
}

int main() {
  // default width, height
  const int width = 512;
  const int height = 512;

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

  // set glfw callbacks
  glfwSetKeyCallback(window, keyCallback);

  // setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  (void)io;

  // setup Dear ImGui style
  ImGui::StyleColorsDark();

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 330 core");

  // setup renderer
  renderer = std::make_unique<Renderer>(width, height);

  // main app loop
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Renderer");
    {
      static int resolution[2] = {static_cast<int>(renderer->getWidth()),
                                  static_cast<int>(renderer->getHeight())};
      if (ImGui::InputInt2("Resolution", resolution)) {
        renderer->resize(resolution[0], resolution[1]);
      }

      ImGui::Text("Samples: %d", renderer->getSamples());

      glm::vec3 camPos = renderer->getCameraPosition();
      ImGui::Text("Camera Position: (%.3f, %.3f, %.3f)", camPos.x, camPos.y,
                  camPos.z);

      ImGui::Text("FPS: %.1f", io.Framerate);
    }
    ImGui::End();

    // Handle Input
    handleInput(window, io);

    // Rendering
    glClear(GL_COLOR_BUFFER_BIT);

    renderer->render();

    // ImGui Rendering
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
  }

  // exit
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}