#ifndef _RENDERER_H
#define _RENDERER_H
#include <cstdint>
#include <limits>
#include <random>
#include <vector>

#include "glad/glad.h"
#include "rectangle.h"
#include "shader.h"

class Renderer {
 private:
  unsigned int width;
  unsigned int height;
  unsigned int samples;

  GLuint accumTexture;
  GLuint stateTexture;
  GLuint accumFBO;

  Rectangle rectangle;

  Shader pt_shader;
  Shader output_shader;

 public:
  Renderer(unsigned int width, unsigned int height)
      : width(width),
        height(height),
        samples(0),
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
    for (int i = 0; i < seed.size(); ++i) {
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

    // compile shaders
    pt_shader.compileShader();
    pt_shader.linkShader();
    output_shader.compileShader();
    output_shader.linkShader();
  }

  void render() {
    const glm::vec2 resolution = glm::vec2(width, height);

    // path tracing
    pt_shader.setUniform("time", static_cast<float>(glfwGetTime()));
    pt_shader.setUniform("samples", samples);
    pt_shader.setUniform("resolution", resolution);
    pt_shader.setUniformTexture("accumTexture", accumTexture, 0);
    pt_shader.setUniformTexture("stateTexture", stateTexture, 1);
    glBindFramebuffer(GL_FRAMEBUFFER, accumFBO);
    rectangle.draw(pt_shader);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // update samples
    samples++;

    // output
    output_shader.setUniform("time", static_cast<float>(glfwGetTime()));
    output_shader.setUniform("samples", samples);
    output_shader.setUniform("resolution", resolution);
    output_shader.setUniformTexture("accumTexture", accumTexture, 0);
    rectangle.draw(output_shader);
  }

  void clear() {
    // clear accumTexture
    glBindTexture(GL_TEXTURE_2D, accumTexture);
    std::vector<GLfloat> data(4 * width * height);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_FLOAT,
                    data.data());
    glBindTexture(GL_TEXTURE_2D, 0);

    // reset samples
    samples = 0;
  }
};

#endif