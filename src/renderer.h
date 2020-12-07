#ifndef _RENDERER_H
#define _RENDERER_H

#include "glad/glad.h"
#include "rectangle.h"
#include "shader.h"

class Renderer {
 private:
  unsigned int width;
  unsigned int height;
  unsigned int samples;

  GLuint accumTexture;
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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA,
                 GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    // setup accumulate FBO
    glGenFramebuffers(1, &accumFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, accumFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           accumTexture, 0);
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