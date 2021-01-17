#version 330 core

#include common/global.frag
#include common/uniform.frag
#include common/rng.frag
#include common/raygen.frag
#include common/util.frag
#include common/intersect.frag
#include common/closest_hit.frag
#include common/sampling.frag
#include common/brdf.frag

in vec2 texCoord;

layout (location = 0) out vec3 color;
layout (location = 1) out uint state;

struct VertexInfo {
  vec3 x; // position
  vec3 n; // normal
  vec3 alpha; // throughput
  int material_id;
};

VertexInfo eyeSubpath[MAX_DEPTH]; // subpath from eye
VertexInfo lightSubpath[MAX_DEPTH]; // subpath from light

// generate subpath from light
void generateLightSubpath() {
  // choose a light randomly
  Light light = lights[int(n_lights * random())];

  // sample point on light
  float pdf_area;
  vec3 x0 = samplePointOnPrimitive(primitives[light.primID], pdf_area);

  lightSubpath[0].x = x0;
  lightSubpath[0].n = 

  // generate path
  float russian_roulette_prob = 1;
  vec3 throughput = vec3(1);

  for(int i = 0; i < MAX_DEPTH; ++i) {
  }
}

// generate subpath from eye
void generateEyeSubpath(in vec2 uv) {
}

vec3 computeRadiance(in Ray ray_in) {
  return vec3(0);
}

void main() {
    // set RNG seed
    setSeed(texCoord);

    // generate initial ray
    vec2 uv = (2.0*(gl_FragCoord.xy + vec2(random(), random())) - resolution) * resolutionYInv;
    uv.y = -uv.y;
    float pdf;
    Ray ray = rayGen(uv, pdf);
    float cos_term = dot(camera.camForward, ray.direction);

    // accumulate sampled color on accumTexture
    vec3 radiance = computeRadiance(ray) / pdf;
    color = texture(accumTexture, texCoord).xyz + radiance * cos_term;

    // save RNG state on stateTexture
    state = RNG_STATE.a;
}