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

bool sampleLight(in Light light, in IntersectInfo info, out vec3 wi, out float pdf) {
  // sample point on light primitive
  Primitive primitive = primitives[light.primID];
  float pdf_area;
  vec3 sampledPos = samplePointOnPrimitive(primitive, pdf_area);

  // test visibility
  wi = normalize(sampledPos - info.hitPos);
  if(dot(wi, info.hitNormal) < 0.0) {
    return false;
  }

  Ray shadowRay = Ray(info.hitPos, wi);
  IntersectInfo shadowInfo;
  if(intersect(shadowRay, shadowInfo) && shadowInfo.primID == light.primID && distance(shadowInfo.hitPos, sampledPos) < 0.1) {
    // convert area p.d.f. to solid angle p.d.f.
    float r = shadowInfo.t;
    float cos_term = abs(dot(-wi, shadowInfo.hitNormal));
    pdf = r*r / cos_term * pdf_area;
    return true;
  }

  return false;
}

vec3 computeRadiance(in Ray ray_in) {
    Ray ray = ray_in;

    const float russian_roulette_prob = 0.99;
    vec3 color = vec3(0);
    vec3 throughput = vec3(1);
    bool is_previous_specular = false;
    for(int i = 0; i < MAX_DEPTH; ++i) {
        // russian roulette
        if(random() >= russian_roulette_prob) {
            break;
        }
        throughput /= russian_roulette_prob;

        IntersectInfo info;
        if(intersect(ray, info)) {
            Primitive hitPrimitive = primitives[info.primID];
            Material hitMaterial = materials[hitPrimitive.material_id];
            vec3 wo = -ray.direction;
            vec3 wo_local = worldToLocal(wo, info.dpdu, info.hitNormal, info.dpdv);

            // Le 
            if((is_previous_specular || i == 0) && any(greaterThan(hitMaterial.le, vec3(0)))) {
                color += throughput * hitMaterial.le;
                break;
            }

            // Light Sampling
            if(hitMaterial.brdf_type == 0) {
              for(int k = 0; k < 1; ++k) {
                Light light = lights[k];
                vec3 wi_light;
                float pdf_light;
                if(sampleLight(light, info, wi_light, pdf_light)) {
                  vec3 wi_light_local = worldToLocal(wi_light, info.dpdu, info.hitNormal, info.dpdv);
                  vec3 brdf = BRDF(wo_local, wi_light_local, hitMaterial);
                  float cos_term = abs(wi_light_local.y);
                  // prevent firefly
                  if(pdf_light > 0.01) {
                    color += throughput * brdf * cos_term * light.le / pdf_light;
                  }
                }
              }
            }

            // BRDF Sampling
            float pdf_brdf;
            vec3 wi_local;
            vec3 brdf = sampleBRDF(wo_local, wi_local, hitMaterial, pdf_brdf);
            // prevent NaN
            if(pdf_brdf == 0.0) {
                break;
            }
            vec3 wi = localToWorld(wi_local, info.dpdu, info.hitNormal, info.dpdv);

            // update throughput
            float cos_term = abs(wi_local.y);
            throughput *= brdf * cos_term / pdf_brdf;

            // set next ray
            ray = Ray(info.hitPos, wi);

            is_previous_specular = (hitMaterial.brdf_type != 0);
        }
        else {
            color += throughput * vec3(0);
            break;
        }
    }

    return color;
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