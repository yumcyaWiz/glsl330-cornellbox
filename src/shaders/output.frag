#version 330 core

uniform float samplesInv;
uniform sampler2D accumTexture;

in vec2 texCoord;
out vec4 fragColor;

void main() {
  vec3 color = texture(accumTexture, texCoord).xyz * samplesInv;
  fragColor = vec4(pow(color, vec3(0.4545)), 1.0);
}