#ifndef _RENDERER_H
#define _RENDERER_H
#include <cstdint>
#include <limits>
#include <random>
#include <vector>

#include "camera.h"
#include "glad/glad.h"
#include "rectangle.h"
#include "scene.h"
#include "shader.h"

enum class RenderMode {
  Render,
  Normal,
  Depth,
  Albedo,
};

enum class Integrator {
  PT,
  PTNEE,
};

class Renderer {
 private:
  struct alignas(16) GlobalBlock {
    alignas(8) glm::uvec2 resolution;
    float resolutionYInv;

    GlobalBlock(const glm::uvec2& resolution) { setResolution(resolution); }

    void setResolution(const glm::uvec2& resolution) {
      this->resolution = resolution;
      resolutionYInv = 1.0f / resolution.y;
    }
  };

  unsigned int samples;
  GlobalBlock global;
  Camera camera;
  Scene scene;

  GLuint accumTexture;
  GLuint stateTexture;
  GLuint accumFBO;

  GLuint globalUBO;
  GLuint cameraUBO;
  GLuint materialUBO;
  GLuint primitiveUBO;
  GLuint lightUBO;

  Rectangle rectangle;

  Shader pt_shader;
  Shader pt_nee_shader;
  Shader output_shader;
  Shader normal_shader;
  Shader depth_shader;
  Shader albedo_shader;

  RenderMode mode;
  Integrator integrator;

  bool clear_flag;

 public:
  Renderer(unsigned int width, unsigned int height)
      : samples(0),
        global({width, height}),
        pt_shader({"./shaders/pt.vert", "./shaders/pt.frag"}),
        pt_nee_shader({"./shaders/pt.vert", "./shaders/pt-nee.frag"}),
        output_shader({"./shaders/pt.vert", "./shaders/output.frag"}),
        normal_shader({"./shaders/pt.vert", "./shaders/normal.frag"}),
        depth_shader({"./shaders/pt.vert", "./shaders/depth.frag"}),
        albedo_shader({"./shaders/pt.vert", "./shaders/albedo.frag"}),
        mode(RenderMode::Render),
        integrator(Integrator::PT) {
    // setup accumulate texture
    glGenTextures(1, &accumTexture);
    glBindTexture(GL_TEXTURE_2D, accumTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB,
                 GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    // setup RNG state texture
    glGenTextures(1, &stateTexture);
    glBindTexture(GL_TEXTURE_2D, stateTexture);
    std::vector<uint32_t> seed(width * height);
    std::random_device rnd_dev;
    std::mt19937 mt(rnd_dev());
    std::uniform_int_distribution<uint32_t> dist(
        1, std::numeric_limits<uint32_t>::max());
    for (unsigned int i = 0; i < seed.size(); ++i) {
      seed[i] = dist(mt);
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32UI, width, height, 0, GL_RED_INTEGER,
                 GL_UNSIGNED_INT, seed.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    // setup accumulate FBO
    glGenFramebuffers(1, &accumFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, accumFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           accumTexture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D,
                           stateTexture, 0);
    GLuint attachments[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
    glDrawBuffers(2, attachments);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // setup UBO
    glGenBuffers(1, &globalUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, globalUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(GlobalBlock), &global,
                 GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glGenBuffers(1, &cameraUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, cameraUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(CameraBlock), &camera.params,
                 GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glGenBuffers(1, &materialUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, materialUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(Material) * 100,
                 scene.materials.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glGenBuffers(1, &primitiveUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, primitiveUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(Primitive) * 100,
                 scene.primitives.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glGenBuffers(1, &lightUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, lightUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(Light) * 100, scene.lights.data(),
                 GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferBase(GL_UNIFORM_BUFFER, 0, globalUBO);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, cameraUBO);
    glBindBufferBase(GL_UNIFORM_BUFFER, 2, materialUBO);
    glBindBufferBase(GL_UNIFORM_BUFFER, 3, primitiveUBO);
    glBindBufferBase(GL_UNIFORM_BUFFER, 4, lightUBO);

    // set uniforms
    pt_shader.setUniformTexture("accumTexture", accumTexture, 0);
    pt_shader.setUniformTexture("stateTexture", stateTexture, 1);
    pt_shader.setUBO("GlobalBlock", 0);
    pt_shader.setUBO("CameraBlock", 1);
    pt_shader.setUBO("MaterialBlock", 2);
    pt_shader.setUBO("PrimitiveBlock", 3);

    pt_nee_shader.setUniformTexture("accumTexture", accumTexture, 0);
    pt_nee_shader.setUniformTexture("stateTexture", stateTexture, 1);
    pt_nee_shader.setUBO("GlobalBlock", 0);
    pt_nee_shader.setUBO("CameraBlock", 1);
    pt_nee_shader.setUBO("MaterialBlock", 2);
    pt_nee_shader.setUBO("PrimitiveBlock", 3);
    pt_nee_shader.setUBO("LightBlock", 4);

    output_shader.setUniformTexture("accumTexture", accumTexture, 0);

    normal_shader.setUBO("GlobalBlock", 0);
    normal_shader.setUBO("CameraBlock", 1);
    normal_shader.setUBO("PrimitiveBlock", 3);

    depth_shader.setUBO("GlobalBlock", 0);
    depth_shader.setUBO("CameraBlock", 1);
    depth_shader.setUBO("PrimitiveBlock", 3);

    albedo_shader.setUBO("GlobalBlock", 0);
    albedo_shader.setUBO("CameraBlock", 1);
    albedo_shader.setUBO("MaterialBlock", 2);
    albedo_shader.setUBO("PrimitiveBlock", 3);
  }

  void destroy() {
    glDeleteTextures(1, &accumTexture);
    glDeleteTextures(1, &stateTexture);

    glDeleteFramebuffers(1, &accumFBO);

    glDeleteBuffers(1, &globalUBO);
    glDeleteBuffers(1, &cameraUBO);
    glDeleteBuffers(1, &materialUBO);
    glDeleteBuffers(1, &primitiveUBO);

    pt_shader.destroy();
    output_shader.destroy();
    normal_shader.destroy();
    depth_shader.destroy();
    albedo_shader.destroy();

    rectangle.destroy();
  }

  unsigned int getWidth() const { return global.resolution.x; }
  unsigned int getHeight() const { return global.resolution.y; }
  unsigned int getSamples() const { return samples; }

  glm::vec3 getCameraPosition() const { return camera.params.camPos; }
  float getCameraFOV() const { return camera.fov; }

  void setFOV(float fov) {
    camera.setFOV(fov);
    glBindBuffer(GL_UNIFORM_BUFFER, cameraUBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(CameraBlock), &camera.params);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    clear_flag = true;
  }
  void moveCamera(const glm::vec3& v) {
    camera.move(v);
    glBindBuffer(GL_UNIFORM_BUFFER, cameraUBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(CameraBlock), &camera.params);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    clear_flag = true;
  }
  void orbitCamera(float dTheta, float dPhi) {
    camera.orbit(dTheta, dPhi);
    glBindBuffer(GL_UNIFORM_BUFFER, cameraUBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(CameraBlock), &camera.params);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    clear_flag = true;
  }

  RenderMode getRenderMode() const { return mode; }
  void setRenderMode(const RenderMode& mode) {
    this->mode = mode;
    clear();
  }

  Integrator getIntegrator() const { return integrator; }
  void setIntegrator(const Integrator& integrator) {
    this->integrator = integrator;
    clear();
  }

  void render() {
    if (clear_flag) {
      clear();
      clear_flag = false;
    }

    glViewport(0, 0, global.resolution.x, global.resolution.y);

    switch (mode) {
      case RenderMode::Render:
        glBindFramebuffer(GL_FRAMEBUFFER, accumFBO);
        switch (integrator) {
          case Integrator::PT:
            rectangle.draw(pt_shader);
            break;
          case Integrator::PTNEE:
            rectangle.draw(pt_nee_shader);
            break;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // update samples
        samples++;

        // output
        output_shader.setUniform("samplesInv", 1.0f / samples);
        rectangle.draw(output_shader);
        break;

      case RenderMode::Normal:
        rectangle.draw(normal_shader);
        break;

      case RenderMode::Depth:
        rectangle.draw(depth_shader);
        break;

      case RenderMode::Albedo:
        rectangle.draw(albedo_shader);
        break;
    }
  }

  void clear() {
    // clear accumTexture
    glBindTexture(GL_TEXTURE_2D, accumTexture);
    std::vector<GLfloat> data(3 * global.resolution.x * global.resolution.y);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, global.resolution.x,
                    global.resolution.y, GL_RGB, GL_FLOAT, data.data());
    glBindTexture(GL_TEXTURE_2D, 0);

    // update texture uniforms
    pt_shader.setUniformTexture("accumTexture", accumTexture, 0);
    pt_nee_shader.setUniformTexture("accumTexture", accumTexture, 0);
    output_shader.setUniformTexture("accumTexture", accumTexture, 0);

    // reset samples
    samples = 0;
  }

  void resize(unsigned int width, unsigned int height) {
    // update resolution
    global.setResolution(glm::uvec2(width, height));
    glBindBuffer(GL_UNIFORM_BUFFER, globalUBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(GlobalBlock), &global);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // resize textures
    glBindTexture(GL_TEXTURE_2D, accumTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB,
                 GL_FLOAT, 0);

    glBindTexture(GL_TEXTURE_2D, stateTexture);
    std::vector<uint32_t> seed(width * height);
    std::random_device rnd_dev;
    std::mt19937 mt(rnd_dev());
    std::uniform_int_distribution<uint32_t> dist(
        1, std::numeric_limits<uint32_t>::max());
    for (unsigned int i = 0; i < seed.size(); ++i) {
      seed[i] = dist(mt);
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32UI, width, height, 0, GL_RED_INTEGER,
                 GL_UNSIGNED_INT, seed.data());
    glBindTexture(GL_TEXTURE_2D, 0);

    // clear textures
    clear();
  }
};

#endif