#version 330 core

uniform uint samples;
uniform sampler2D accumTexture;

in vec2 texCoord;

void main() {
  vec3 color = texture(accumTexture, texCoord).xyz / samples;
  gl_FragColor = vec4(pow(color, vec3(0.4545)), 1.0);
}