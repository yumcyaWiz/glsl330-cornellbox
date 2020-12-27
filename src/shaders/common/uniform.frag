uniform sampler2D accumTexture;
uniform usampler2D stateTexture;

layout(std140) uniform GlobalBlock {
  uvec2 resolution;
  float resolutionYInv;
};

layout(std140) uniform CameraBlock {
  vec3 camPos;
  vec3 camForward;
  vec3 camRight;
  vec3 camUp;
  float a;
} camera;

const int MAX_N_MATERIALS = 100;
const int MAX_N_PRIMITIVES = 100;
const int MAX_N_LIGHTS = 100;
layout(std140) uniform SceneBlock {
  int n_materials;
  int n_primitives;
  int n_lights;
  Material materials[MAX_N_MATERIALS];
  Primitive primitives[MAX_N_PRIMITIVES];
  Light lights[MAX_N_LIGHTS];
};