#version 330 core

#include global.frag
#include uniform.frag
#include rng.frag
#include raygen.frag
#include intersect.frag
#include closest_hit.frag

layout (location = 0) out vec3 color;
layout (location = 1) out uint state;

vec3 sampleCosineHemisphere(float u, float v, out float pdf) {
    float theta = 0.5 * acos(clamp(1.0 - 2.0 * u, -1.0, 1.0));
    float phi = 2.0 * PI * v;
    float y = cos(theta);
    pdf = y / PI;
    return vec3(cos(phi) * sin(theta), y, sin(phi) * sin(theta));
}

vec3 worldToLocal(vec3 v, vec3 lx, vec3 ly, vec3 lz) {
    return vec3(dot(v, lx), dot(v, ly), dot(v, lz));
}
vec3 localToWorld(vec3 v, vec3 lx, vec3 ly, vec3 lz) {
    return vec3(dot(v, vec3(lx.x, ly.x, lz.x)), dot(v, vec3(lx.y, ly.y, lz.y)), dot(v, vec3(lx.z, ly.z, lz.z)));
}

vec3 computeRadiance(Ray ray_in) {
    Ray ray = ray_in;

    float russian_roulette_prob = 0.99;
    vec3 color = vec3(0);
    vec3 throughput = vec3(1);
    for(int i = 0; i < MAX_DEPTH; ++i) {
        // russian roulette
        if(random() >= russian_roulette_prob) {
            break;
        }
        throughput /= russian_roulette_prob;

        Hit info = intersect(ray);
        if(info.hit) {
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
            vec3 BRDF = hitPrimitive.kd / PI;
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
    vec2 texUV = gl_FragCoord.xy / resolution;
    setSeed(texUV);

    // generate initial ray
    vec2 uv = (2.0*(gl_FragCoord.xy + vec2(random(), random())) - resolution) / resolution;
    uv.y = -uv.y;
    Ray ray = rayGen(uv);

    // accumulate sampled color on accumTexture
    vec3 radiance = computeRadiance(ray);
    color = texture(accumTexture, texUV).xyz + radiance;

    // save RNG state on stateTexture
    state = RNG_STATE.a;
}