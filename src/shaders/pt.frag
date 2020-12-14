#version 330 core

#include global.frag
#include uniform.frag
#include rng.frag
#include raygen.frag
#include util.frag
#include intersect.frag
#include closest_hit.frag
#include sampling.frag

in vec2 texCoord;

layout (location = 0) out vec3 color;
layout (location = 1) out uint state;

vec3 computeRadiance(in Ray ray_in) {
    Ray ray = ray_in;

    const float russian_roulette_prob = 0.99;
    vec3 color = vec3(0);
    vec3 throughput = vec3(1);
    for(int i = 0; i < MAX_DEPTH; ++i) {
        // russian roulette
        if(random() >= russian_roulette_prob) {
            break;
        }
        throughput /= russian_roulette_prob;

        IntersectInfo info;
        if(intersect(ray, info)) {
            Primitive hitPrimitive = primitives[info.primID];

            // Le 
            color += throughput * hitPrimitive.le;

            // BRDF Sampling
            float pdf_solid;
            vec3 next_direction_local = sampleCosineHemisphere(random(), random(), pdf_solid);
            // prevent NaN
            if(pdf_solid == 0.0) {
                break;
            }
            vec3 next_direction = localToWorld(next_direction_local, info.dpdu, info.hitNormal, info.dpdv);

            // update throughput
            vec3 BRDF = hitPrimitive.kd * PI_INV;
            float cos_term = abs(dot(next_direction, info.hitNormal));
            throughput *= BRDF * cos_term / pdf_solid;

            ray = Ray(info.hitPos, next_direction);
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
    Ray ray = rayGen(uv);

    // accumulate sampled color on accumTexture
    vec3 radiance = computeRadiance(ray);
    color = texture(accumTexture, texCoord).xyz + radiance;

    // save RNG state on stateTexture
    state = RNG_STATE.a;
}