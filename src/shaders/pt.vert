#version 330 core
layout (location = 0) in vec3 vPos;
layout (location = 1) in vec2 vtexCoord;

out vec2 texCoord;

void main() {
  texCoord = vtexCoord;
  gl_Position = vec4(vPos, 1.0);
}