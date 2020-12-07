#ifndef _RECTANGLE_H
#define _RECTANGLE_H

#include "glad/glad.h"
#include "shader.h"

class Rectangle {
 private:
  GLuint VAO;

 public:
  Rectangle() {
    // setup VAO
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
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat),
                          (GLvoid*)0);

    // unbind VAO, VBO, EBO
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  }

  void draw(const Shader& shader) const {
    shader.activate();
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    shader.deactivate();
  }
};

#endif