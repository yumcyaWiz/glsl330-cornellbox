#ifndef _SHADER_H
#define _SHADER_H
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <variant>

#include "glad/glad.h"

const std::string fileToString(const std::string& filename) {
  std::ifstream file(filename);
  if (!file) {
    std::cerr << "failed to open " << filename << std::endl;
    std::exit(EXIT_FAILURE);
  }

  // convert file content to string
  std::stringstream ss;
  ss << file.rdbuf();
  file.close();
  return ss.str();
}

class Shader {
 public:
  Shader(const std::string& _vertex_shader_filepath,
         const std::string& _fragment_shader_filepath)
      : vertex_shader_filepath(_vertex_shader_filepath),
        fragment_shader_filepath(_fragment_shader_filepath) {}

  void compileShader() {
    // compile vertex shader
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    vertex_shader_source = fileToString(vertex_shader_filepath);
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
    fragment_shader_source = fileToString(fragment_shader_filepath);
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

  void useShader() const { glUseProgram(program); }

  void updateUniform(const std::string& uniform_name, const std::variant<GLint ,GLfloat>& v) {
    GLint location =
        glGetUniformLocation(program, uniform_name.c_str());

    struct Visitor {
      Visitor(GLint _location) : location(_location) {}

      void operator()(GLint value) {
        glUniform1i(location, value);
      }
      void operator()(GLfloat value) {
        glUniform1f(location, value);
      }

      GLint location;
    };

    std::visit(Visitor{location}, v);
  }

 private:
  const std::string vertex_shader_filepath;
  std::string vertex_shader_source;
  const std::string fragment_shader_filepath;
  std::string fragment_shader_source;
  GLuint vertex_shader;
  GLuint fragment_shader;
  GLuint program;
};

#endif