uniform uvec2 resolution;
uniform float resolutionYInv;

uniform sampler2D accumTexture;
uniform usampler2D stateTexture;

// uniform Primitive primitives[16];

uniform vec3 camPos;
uniform vec3 camForward;
uniform vec3 camRight;
uniform vec3 camUp;

const int MAX_N_PRIMITIVES = 100;
layout(std140) uniform PrimitiveBlock {
  Primitive primitives[MAX_N_PRIMITIVES];
};