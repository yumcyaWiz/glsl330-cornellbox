#version 330 core

uniform int samples;
uniform vec2 resolution;
uniform sampler2D accumTexture;

void main() {
  vec2 uv = gl_FragCoord.xy / resolution;
  vec3 color = texture(accumTexture, uv).xyz / float(samples);
  gl_FragColor = vec4(vec3(uv.x, uv.y, 0), 1.0);
}