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
  if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS &&
      glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) {
    renderer->moveCamera(glm::vec3(0, 0, -io.MouseDelta.y));
  } else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS &&
             glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) ==
                 GLFW_PRESS) {
    renderer->moveCamera(glm::vec3(io.MouseDelta.x, io.MouseDelta.y, 0));
  }
  // Camera Orbit
  else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) {
    const float orbitSpeed = 0.01f;
    renderer->orbitCamera(orbitSpeed * io.MouseDelta.y,
                          orbitSpeed * io.MouseDelta.x);
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

      static RenderMode mode = renderer->getRenderMode();
      if (ImGui::Combo("Layer", reinterpret_cast<int*>(&mode),
                       "Render\0Normal\0\0")) {
        renderer->setRenderMode(mode);
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