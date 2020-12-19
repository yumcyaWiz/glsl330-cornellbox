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

class Renderer {
 private:
  unsigned int samples;
  glm::uvec2 resolution;
  Camera camera;
  Scene scene;

  GLuint accumTexture;
  GLuint stateTexture;
  GLuint accumFBO;

  GLuint materialUBO;
  GLuint primitiveUBO;

  Rectangle rectangle;

  Shader pt_shader;
  Shader output_shader;

  bool clear_flag;

 public:
  Renderer(unsigned int width, unsigned int height)
      : samples(0),
        resolution({width, height}),
        scene(),
        rectangle(),
        pt_shader({"./shaders/pt.vert", "./shaders/pt.frag"}),
        output_shader({"./shaders/pt.vert", "./shaders/output.frag"}) {
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
    glGenBuffers(1, &materialUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, materialUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(Material) * 100, NULL,
                 GL_STATIC_DRAW);
    glBufferSubData(GL_UNIFORM_BUFFER, 0,
                    sizeof(Material) * scene.materials.size(),
                    scene.materials.data());
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, materialUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glGenBuffers(1, &primitiveUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, primitiveUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(Primitive) * 100, NULL,
                 GL_STATIC_DRAW);
    glBufferSubData(GL_UNIFORM_BUFFER, 0,
                    sizeof(Primitive) * scene.primitives.size(),
                    scene.primitives.data());
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, primitiveUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // set uniforms
    pt_shader.setUniform("resolution", resolution);
    pt_shader.setUniform("resolutionYInv", 1.0f / resolution.y);
    pt_shader.setUniformTexture("accumTexture", accumTexture, 0);
    pt_shader.setUniformTexture("stateTexture", stateTexture, 1);
    pt_shader.setUBO("MaterialBlock", 0);
    pt_shader.setUBO("PrimitiveBlock", 1);

    output_shader.setUniformTexture("accumTexture", accumTexture, 0);

    // setup scene
    // setCornellBoxScene();
  }

  unsigned int getWidth() const { return resolution.x; }
  unsigned int getHeight() const { return resolution.y; }
  unsigned int getSamples() const { return samples; }

  glm::vec3 getCameraPosition() const { return camera.camPos; }

  void moveCamera(const glm::vec3& v) {
    camera.move(v);
    clear_flag = true;
  }
  void orbitCamera(float dTheta, float dPhi) {
    camera.orbit(dTheta, dPhi);
    clear_flag = true;
  }

  void render() {
    if (clear_flag) {
      clear();
      clear_flag = false;
    }

    // path tracing
    glBindFramebuffer(GL_FRAMEBUFFER, accumFBO);
    pt_shader.setUniform("camPos", camera.camPos);
    pt_shader.setUniform("camForward", camera.camForward);
    pt_shader.setUniform("camRight", camera.camRight);
    pt_shader.setUniform("camUp", camera.camUp);

    glViewport(0, 0, resolution.x, resolution.y);
    rectangle.draw(pt_shader);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // update samples
    samples++;

    // output
    output_shader.setUniform("samplesInv", 1.0f / samples);
    rectangle.draw(output_shader);
  }

  void clear() {
    // clear accumTexture
    glBindTexture(GL_TEXTURE_2D, accumTexture);
    std::vector<GLfloat> data(3 * resolution.x * resolution.y);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, resolution.x, resolution.y, GL_RGB,
                    GL_FLOAT, data.data());
    glBindTexture(GL_TEXTURE_2D, 0);

    // update texture uniforms
    pt_shader.setUniformTexture("accumTexture", accumTexture, 0);
    output_shader.setUniformTexture("accumTexture", accumTexture, 0);

    // reset samples
    samples = 0;
  }

  void resize(unsigned int width, unsigned int height) {
    // update resolution
    resolution = glm::uvec2(width, height);
    pt_shader.setUniform("resolution", resolution);
    pt_shader.setUniform("resolutionYInv", 1.0f / resolution.y);
    output_shader.setUniform("resolution", resolution);

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

 private:
  void setCornellBoxScene() {
    const auto white1 = glm::vec3(0.8);
    const auto red = glm::vec3(0.8, 0.05, 0.05);
    const auto green = glm::vec3(0.05, 0.8, 0.05);
    const auto light = glm::vec3(34, 19, 10);

    pt_shader.setUniform("primitives[0].id", 0);
    pt_shader.setUniform("primitives[0].type", 1);
    pt_shader.setUniform("primitives[0].leftCornerPoint", glm::vec3(0));
    pt_shader.setUniform("primitives[0].right", glm::vec3(0, 0, 559.2));
    pt_shader.setUniform("primitives[0].up", glm::vec3(556, 0, 0));
    pt_shader.setUniform("primitives[0].brdf_type", 0);
    pt_shader.setUniform("primitives[0].kd", white1);
    pt_shader.setUniform("primitives[0].le", glm::vec3(0));

    pt_shader.setUniform("primitives[1].id", 1);
    pt_shader.setUniform("primitives[1].type", 1);
    pt_shader.setUniform("primitives[1].leftCornerPoint", glm::vec3(0));
    pt_shader.setUniform("primitives[1].right", glm::vec3(0, 548.8, 0));
    pt_shader.setUniform("primitives[1].up", glm::vec3(0, 0, 559.2));
    pt_shader.setUniform("primitives[1].brdf_type", 0);
    pt_shader.setUniform("primitives[1].kd", green);
    pt_shader.setUniform("primitives[1].le", glm::vec3(0));

    pt_shader.setUniform("primitives[2].id", 2);
    pt_shader.setUniform("primitives[2].type", 1);
    pt_shader.setUniform("primitives[2].leftCornerPoint", glm::vec3(556, 0, 0));
    pt_shader.setUniform("primitives[2].right", glm::vec3(0, 0, 559.2));
    pt_shader.setUniform("primitives[2].up", glm::vec3(0, 548.8, 0));
    pt_shader.setUniform("primitives[2].brdf_type", 0);
    pt_shader.setUniform("primitives[2].kd", red);
    pt_shader.setUniform("primitives[2].le", glm::vec3(0));

    pt_shader.setUniform("primitives[3].id", 3);
    pt_shader.setUniform("primitives[3].type", 1);
    pt_shader.setUniform("primitives[3].leftCornerPoint",
                         glm::vec3(0, 548.8, 0));
    pt_shader.setUniform("primitives[3].right", glm::vec3(556, 0, 0));
    pt_shader.setUniform("primitives[3].up", glm::vec3(0, 0, 559.2));
    pt_shader.setUniform("primitives[3].brdf_type", 0);
    pt_shader.setUniform("primitives[3].kd", white1);
    pt_shader.setUniform("primitives[3].le", glm::vec3(0));

    pt_shader.setUniform("primitives[4].id", 4);
    pt_shader.setUniform("primitives[4].type", 1);
    pt_shader.setUniform("primitives[4].leftCornerPoint",
                         glm::vec3(0, 0, 559.2));
    pt_shader.setUniform("primitives[4].right", glm::vec3(0, 548.8, 0));
    pt_shader.setUniform("primitives[4].up", glm::vec3(556, 0, 0));
    pt_shader.setUniform("primitives[4].brdf_type", 0);
    pt_shader.setUniform("primitives[4].kd", white1);
    pt_shader.setUniform("primitives[4].le", glm::vec3(0));

    pt_shader.setUniform("primitives[5].id", 5);
    pt_shader.setUniform("primitives[5].type", 1);
    pt_shader.setUniform("primitives[5].leftCornerPoint",
                         glm::vec3(130, 165, 65));
    pt_shader.setUniform("primitives[5].right", glm::vec3(-48, 0, 160));
    pt_shader.setUniform("primitives[5].up", glm::vec3(160, 0, 49));
    pt_shader.setUniform("primitives[5].brdf_type", 0);
    pt_shader.setUniform("primitives[5].kd", white1);
    pt_shader.setUniform("primitives[5].le", glm::vec3(0));

    pt_shader.setUniform("primitives[6].id", 6);
    pt_shader.setUniform("primitives[6].type", 1);
    pt_shader.setUniform("primitives[6].leftCornerPoint",
                         glm::vec3(290, 0, 114));
    pt_shader.setUniform("primitives[6].right", glm::vec3(0, 165, 0));
    pt_shader.setUniform("primitives[6].up", glm::vec3(-50, 0, 158));
    pt_shader.setUniform("primitives[6].brdf_type", 0);
    pt_shader.setUniform("primitives[6].kd", white1);
    pt_shader.setUniform("primitives[6].le", glm::vec3(0));

    pt_shader.setUniform("primitives[7].id", 7);
    pt_shader.setUniform("primitives[7].type", 1);
    pt_shader.setUniform("primitives[7].leftCornerPoint",
                         glm::vec3(130, 0, 65));
    pt_shader.setUniform("primitives[7].right", glm::vec3(0, 165, 0));
    pt_shader.setUniform("primitives[7].up", glm::vec3(160, 0, 49));
    pt_shader.setUniform("primitives[7].brdf_type", 0);
    pt_shader.setUniform("primitives[7].kd", white1);
    pt_shader.setUniform("primitives[7].le", glm::vec3(0));

    pt_shader.setUniform("primitives[8].id", 8);
    pt_shader.setUniform("primitives[8].type", 1);
    pt_shader.setUniform("primitives[8].leftCornerPoint",
                         glm::vec3(82, 0, 225));
    pt_shader.setUniform("primitives[8].right", glm::vec3(0, 165, 0));
    pt_shader.setUniform("primitives[8].up", glm::vec3(48, 0, -160));
    pt_shader.setUniform("primitives[8].brdf_type", 0);
    pt_shader.setUniform("primitives[8].kd", white1);
    pt_shader.setUniform("primitives[8].le", glm::vec3(0));

    pt_shader.setUniform("primitives[9].id", 9);
    pt_shader.setUniform("primitives[9].type", 1);
    pt_shader.setUniform("primitives[9].leftCornerPoint",
                         glm::vec3(240, 0, 272));
    pt_shader.setUniform("primitives[9].right", glm::vec3(0, 165, 0));
    pt_shader.setUniform("primitives[9].up", glm::vec3(-158, 0, -47));
    pt_shader.setUniform("primitives[9].brdf_type", 0);
    pt_shader.setUniform("primitives[9].kd", white1);
    pt_shader.setUniform("primitives[9].le", glm::vec3(0));

    pt_shader.setUniform("primitives[10].id", 10);
    pt_shader.setUniform("primitives[10].type", 1);
    pt_shader.setUniform("primitives[10].leftCornerPoint",
                         glm::vec3(423, 330, 247));
    pt_shader.setUniform("primitives[10].right", glm::vec3(-158, 0, 49));
    pt_shader.setUniform("primitives[10].up", glm::vec3(49, 0, 159));
    pt_shader.setUniform("primitives[10].brdf_type", 0);
    pt_shader.setUniform("primitives[10].kd", white1);
    pt_shader.setUniform("primitives[10].le", glm::vec3(0));

    pt_shader.setUniform("primitives[11].id", 11);
    pt_shader.setUniform("primitives[11].type", 1);
    pt_shader.setUniform("primitives[11].leftCornerPoint",
                         glm::vec3(423, 0, 247));
    pt_shader.setUniform("primitives[11].right", glm::vec3(0, 330, 0));
    pt_shader.setUniform("primitives[11].up", glm::vec3(49, 0, 159));
    pt_shader.setUniform("primitives[11].brdf_type", 0);
    pt_shader.setUniform("primitives[11].kd", white1);
    pt_shader.setUniform("primitives[11].le", glm::vec3(0));

    pt_shader.setUniform("primitives[12].id", 12);
    pt_shader.setUniform("primitives[12].type", 1);
    pt_shader.setUniform("primitives[12].leftCornerPoint",
                         glm::vec3(472, 0, 406));
    pt_shader.setUniform("primitives[12].right", glm::vec3(0, 330, 0));
    pt_shader.setUniform("primitives[12].up", glm::vec3(-158, 0, 50));
    pt_shader.setUniform("primitives[12].brdf_type", 0);
    pt_shader.setUniform("primitives[12].kd", white1);
    pt_shader.setUniform("primitives[12].le", glm::vec3(0));

    pt_shader.setUniform("primitives[13].id", 13);
    pt_shader.setUniform("primitives[13].type", 1);
    pt_shader.setUniform("primitives[13].leftCornerPoint",
                         glm::vec3(314, 0, 456));
    pt_shader.setUniform("primitives[13].right", glm::vec3(0, 330, 0));
    pt_shader.setUniform("primitives[13].up", glm::vec3(-49, 0, -160));
    pt_shader.setUniform("primitives[13].brdf_type", 0);
    pt_shader.setUniform("primitives[13].kd", white1);
    pt_shader.setUniform("primitives[13].le", glm::vec3(0));

    pt_shader.setUniform("primitives[14].id", 14);
    pt_shader.setUniform("primitives[14].type", 1);
    pt_shader.setUniform("primitives[14].leftCornerPoint",
                         glm::vec3(265, 0, 296));
    pt_shader.setUniform("primitives[14].right", glm::vec3(0, 330, 0));
    pt_shader.setUniform("primitives[14].up", glm::vec3(158, 0, -49));
    pt_shader.setUniform("primitives[14].brdf_type", 0);
    pt_shader.setUniform("primitives[14].kd", white1);
    pt_shader.setUniform("primitives[14].le", glm::vec3(0));

    pt_shader.setUniform("primitives[15].id", 15);
    pt_shader.setUniform("primitives[15].type", 1);
    pt_shader.setUniform("primitives[15].leftCornerPoint",
                         glm::vec3(343, 548.6, 227));
    pt_shader.setUniform("primitives[15].right", glm::vec3(-130, 0, 0));
    pt_shader.setUniform("primitives[15].up", glm::vec3(0, 0, 105));
    pt_shader.setUniform("primitives[15].brdf_type", 0);
    pt_shader.setUniform("primitives[15].kd", white1);
    pt_shader.setUniform("primitives[15].le", light);
  }
};

#endif