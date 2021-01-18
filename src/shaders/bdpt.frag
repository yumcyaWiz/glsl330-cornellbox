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
  int material_id; // material ID
};

VertexInfo lightSubpath[MAX_DEPTH + 1]; // subpath from light
VertexInfo eyeSubpath[MAX_DEPTH + 1]; // subpath from eye

// generate subpath from light
// return: number of vertices of generated subpath
int generateLightSubpath() {
  // choose a light randomly
  Light light = lights[int(n_lights * random())];

  // sample point on light
  float pdf_area;
  vec3 normal;
  vec3 dpdu;
  vec3 dpdv;
  vec3 x0 = samplePointOnPrimitive(primitives[light.primID], normal, dpdu, dpdv, pdf_area);

  lightSubpath[0].x = x0;
  lightSubpath[0].n = normal;
  lightSubpath[0].alpha = (n_lights * light.le) / pdf_area;

  // sample direction from light
  float pdf_solid;
  vec3 w0 = localToWorld(sampleHemisphere(random(), random(), pdf_solid), dpdu, normal, dpdv);

  Ray ray = Ray(x0, w0);
  float rr_prob = 1; // russian roulette probability
  int n_L = 1; // number of vertices of light subpath
  vec3 brdf = vec3(1); // BRDF

  // generate path
  for(int i = 1; i <= MAX_DEPTH; ++i) {
    // russian roulette
    if(random() >= rr_prob) {
      break;
    }

    IntersectInfo info;
    if(intersect(ray, info)) {
      // update number of vertices
      n_L++;

      // hit surface info
      Primitive hitPrimitive = primitives[info.primID];
      Material hitMaterial = materials[hitPrimitive.material_id];

      // set vertex info
      lightSubpath[i].x = info.hitPos;
      lightSubpath[i].n = info.hitNormal;
      lightSubpath[i].material_id = hitPrimitive.material_id;

      // compute alpha
      lightSubpath[i].alpha = abs(dot(lightSubpath[i - 1].n, ray.direction)) / (rr_prob * pdf_solid) * brdf * lightSubpath[i - 1].alpha;

      // BRDF sampling
      vec3 wi_local = worldToLocal(-ray.direction, info.dpdu, info.hitNormal, info.dpdv);
      vec3 wo_local;
      brdf = sampleBRDF(wi_local, wo_local, hitMaterial, pdf_solid);
      vec3 wo = localToWorld(wo_local, info.dpdu, info.hitNormal, info.dpdv);

      // update russian roulette probability
      rr_prob *= 0.99;

      // set next ray
      ray = Ray(info.hitPos, wo);
    }
    else {
      break;
    }
  }

  return n_L;
}

// generate subpath from eye
// ij: pixel coordinate (i, j)
// return: number of vertices of generated subpath
int generateEyeSubpath(in vec2 ij) {
  // sample point on film
    vec2 uv = (2.0*(ij + vec2(random(), random())) - resolution) * resolutionYInv;
    uv.y = -uv.y;

  // sample point on lens(special case: pinhole camera)
  eyeSubpath[0].x = camera.camPos;
  eyeSubpath[0].n = camera.camForward;
  eyeSubpath[0].alpha = vec3(1);

  // sample direction from eye 
  float pdf_solid;
  Ray ray = rayGen(uv, pdf_solid);

  float rr_prob = 1; // russian roulette probability
  int n_E = 1; // number of vertices of eye subpath
  vec3 brdf = vec3(1); // BRDF

  // generate path
  for(int i = 1; i <= MAX_DEPTH; ++i) {
    // russian roulette
    if(random() >= rr_prob) {
      break;
    }

    IntersectInfo info;
    if(intersect(ray, info)) {
      // update number of vertices
      n_E++;

      // hit surface info
      Primitive hitPrimitive = primitives[info.primID];
      Material hitMaterial = materials[hitPrimitive.material_id];

      // set vertex info
      eyeSubpath[i].x = info.hitPos;
      eyeSubpath[i].n = info.hitNormal;
      eyeSubpath[i].material_id = hitPrimitive.material_id;

      // compute alpha
      eyeSubpath[i].alpha = abs(dot(eyeSubpath[i - 1].n, ray.direction)) / (rr_prob * pdf_solid) * brdf * eyeSubpath[i - 1].alpha;

      // BRDF sampling
      vec3 wo_local = worldToLocal(-ray.direction, info.dpdu, info.hitNormal, info.dpdv);
      vec3 wi_local;
      brdf = sampleBRDF(wo_local, wi_local, hitMaterial, pdf_solid);
      vec3 wi = localToWorld(wi_local, info.dpdu, info.hitNormal, info.dpdv);

      // update russian roulette probability
      rr_prob *= 0.99;

      // set next ray
      ray = Ray(info.hitPos, wi);
    }
    else {
      break;
    }
  }

  return n_E;
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