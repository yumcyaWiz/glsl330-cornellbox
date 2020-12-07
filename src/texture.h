#ifndef _TEXTURE_H
#define _TEXTURE_H
#include <vector>

#include "glad/glad.h"

class Texture {
 private:
  unsigned int width;
  unsigned int height;
  GLuint texture;

 public:
  Texture(unsigned int width, unsigned int height)
      : width(width), height(height) {
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA,
                 GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           texture, 0);
  }

  void clear() {
    glBindTexture(GL_TEXTURE_2D, texture);
    std::vector<GLfloat> data(4 * width * height);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_FLOAT,
                    data.data());
    glBindTexture(GL_TEXTURE_2D, 0);
  }
};

#endif