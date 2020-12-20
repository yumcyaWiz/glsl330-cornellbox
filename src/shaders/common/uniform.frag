uniform uvec2 resolution;
uniform float resolutionYInv;

uniform sampler2D accumTexture;
uniform usampler2D stateTexture;

layout(std140) uniform CameraBlock {
  vec3 camPos;
  vec3 camForward;
  vec3 camRight;
  vec3 camUp;
  float a;
} camera;

const int MAX_N_MATERIALS = 100;
layout(std140) uniform MaterialBlock {
  Material materials[MAX_N_MATERIALS];
};

const int MAX_N_PRIMITIVES = 100;
layout(std140) uniform PrimitiveBlock {
  Primitive primitives[MAX_N_PRIMITIVES];
};