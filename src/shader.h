#ifndef _SHADER_H
#define _SHADER_H
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "glad/glad.h"

const std::string fileToString(const std::string& filename) {
  std::ifstream file(filename);
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

  void compileShader() const {
    // compile vertex shader
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    const char* vertex_shader_source =
        fileToString(vertex_shader_filepath).c_str();
    glShaderSource(vertex_shader, 1, &vertex_shader_source, nullptr);
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
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fragment_shader_source =
        fileToString(fragment_shader_filepath).c_str();
    glShaderSource(fragment_shader, 1, &fragment_shader_source, nullptr);
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

 private:
  const std::string vertex_shader_filepath;
  const std::string fragment_shader_filepath;
};

#endif