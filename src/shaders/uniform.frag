uniform uvec2 resolution;
uniform float resolutionYInv;

uniform sampler2D accumTexture;
uniform usampler2D stateTexture;

uniform Primitive primitives[16];

uniform vec3 camPos = vec3(278, 273, -900);
uniform vec3 camForward = vec3(0, 0, 1);
uniform vec3 camRight = vec3(1, 0, 0);
uniform vec3 camUp = vec3(0, 1, 0);