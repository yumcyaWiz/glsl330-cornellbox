#ifndef _SHADER_H
#define _SHADER_H
#include <fstream>
#include <iostream>
#include <sstream>
#include <variant>
#include <vector>

#include "Shadinclude.hpp"
#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

class Shader {
 private:
  const std::string vertex_shader_filepath;
  std::string vertex_shader_source;
  const std::string fragment_shader_filepath;
  std::string fragment_shader_source;
  GLuint vertex_shader;
  GLuint fragment_shader;
  GLuint program;

  void compileShader() {
    // compile vertex shader
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    vertex_shader_source = Shadinclude::load(vertex_shader_filepath);
    const char* vertex_shader_source_c_str = vertex_shader_source.c_str();
    glShaderSource(vertex_shader, 1, &vertex_shader_source_c_str, nullptr);
    glCompileShader(vertex_shader);

    // handle compilation error
    GLint success = 0;
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
      std::cerr << "failed to compile vertex shader" << std::endl;

      GLint logSize = 0;
      glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &logSize);
      std::vector<GLchar> errorLog(logSize);
      glGetShaderInfoLog(vertex_shader, logSize, &logSize, &errorLog[0]);
      std::string errorLogStr(errorLog.begin(), errorLog.end());
      std::cerr << errorLogStr << std::endl;

      glDeleteShader(vertex_shader);
      return;
    }

    // compile fragment shader
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    fragment_shader_source = Shadinclude::load(fragment_shader_filepath);
    const char* fragment_shader_source_c_str = fragment_shader_source.c_str();
    glShaderSource(fragment_shader, 1, &fragment_shader_source_c_str, nullptr);
    glCompileShader(fragment_shader);

    // handle compilation error
    success = 0;
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
      std::cerr << "failed to compile fragment shader" << std::endl;

      GLint logSize = 0;
      glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &logSize);
      std::vector<GLchar> errorLog(logSize);
      glGetShaderInfoLog(fragment_shader, logSize, &logSize, &errorLog[0]);
      std::string errorLogStr(errorLog.begin(), errorLog.end());
      std::cerr << errorLogStr << std::endl;

      glDeleteShader(fragment_shader);
      return;
    }
  }

  void linkShader() {
    // Link Shader Program
    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    glDetachShader(program, vertex_shader);
    glDetachShader(program, fragment_shader);

    // handle link error
    int success = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (success == GL_FALSE) {
      std::cerr << "failed to link shaders " << std::endl;

      GLint logSize = 0;
      glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logSize);
      std::vector<GLchar> errorLog(logSize);
      glGetProgramInfoLog(program, logSize, &logSize, &errorLog[0]);
      std::string errorLogStr(errorLog.begin(), errorLog.end());
      std::cerr << errorLogStr << std::endl;

      glDeleteProgram(program);
      return;
    }
  }

 public:
  Shader() {}
  Shader(const std::string& _vertex_shader_filepath,
         const std::string& _fragment_shader_filepath)
      : vertex_shader_filepath(_vertex_shader_filepath),
        fragment_shader_filepath(_fragment_shader_filepath) {
    compileShader();
    linkShader();
  }

  void destroy() {
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    glDeleteProgram(program);
  }

  void activate() const { glUseProgram(program); }
  void deactivate() const { glUseProgram(0); }

  void setUniform(const std::string& uniform_name, GLint value) const {
    activate();
    const GLint location = glGetUniformLocation(program, uniform_name.c_str());
    glUniform1i(location, value);
    deactivate();
  }
  void setUniform(const std::string& uniform_name, GLuint value) const {
    activate();
    const GLint location = glGetUniformLocation(program, uniform_name.c_str());
    glUniform1ui(location, value);
    deactivate();
  }
  void setUniform(const std::string& uniform_name, GLfloat value) const {
    activate();
    const GLint location = glGetUniformLocation(program, uniform_name.c_str());
    glUniform1f(location, value);
    deactivate();
  }
  void setUniform(const std::string& uniform_name,
                  const glm::vec2& value) const {
    activate();
    const GLint location = glGetUniformLocation(program, uniform_name.c_str());
    glUniform2fv(location, 1, glm::value_ptr(value));
    deactivate();
  }
  void setUniform(const std::string& uniform_name,
                  const glm::uvec2& value) const {
    activate();
    const GLint location = glGetUniformLocation(program, uniform_name.c_str());
    glUniform2uiv(location, 1, glm::value_ptr(value));
    deactivate();
  }
  void setUniform(const std::string& uniform_name,
                  const glm::vec3& value) const {
    activate();
    const GLint location = glGetUniformLocation(program, uniform_name.c_str());
    glUniform3fv(location, 1, glm::value_ptr(value));
    deactivate();
  }

  void setUniformTexture(const std::string& uniform_name, GLuint texture,
                         GLuint texture_unit_number) const {
    activate();
    const GLint location = glGetUniformLocation(program, uniform_name.c_str());
    glUniform1i(location, texture_unit_number);
    glActiveTexture(GL_TEXTURE0 + texture_unit_number);
    glBindTexture(GL_TEXTURE_2D, texture);
    deactivate();
  }

  void setUBO(const std::string& block_name, GLuint binding_number) const {
    const GLuint index = glGetUniformBlockIndex(program, block_name.c_str());
    glUniformBlockBinding(program, index, binding_number);
  }
};

#endif