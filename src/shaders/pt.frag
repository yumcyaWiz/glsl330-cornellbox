#version 330 core

#include global.glsl
#include uniform.glsl
#include rng.glsl
#include raygen.glsl
#include intersect.frag

layout (location = 0) out vec3 color;
layout (location = 1) out uint state;

Hit intersect_each(Ray ray, Primitive primitive) {
    Hit ret;

    // Sphere
    if(primitive.type == 0) {
        intersectSphere(primitive.center, primitive.radius, ray, ret);
    }
    // Plane
    else if(primitive.type == 1) {
        intersectPlane(primitive.leftCornerPoint, primitive.right, primitive.up, ray, ret);
    }
    
    return ret;
}

const vec3 white1 = vec3(0.8);
const vec3 white2 = vec3(0.99);
const vec3 red = vec3(0.8, 0.05, 0.05);
const vec3 green = vec3(0.05, 0.8, 0.05);
const vec3 light = vec3(34, 19, 10);

const Primitive primitives[16] = Primitive[16](
    // floor
    Primitive(
        0,
        1, 
        vec3(0), 0.0, 
        vec3(0), vec3(0, 0, 559.2), vec3(556, 0, 0),
        white1,
        vec3(0)
    ),
    // right wall
    Primitive(
        1,
        1, 
        vec3(0), 0.0, 
        vec3(0), vec3(0, 548.8, 0), vec3(0, 0, 559.2),
        green,
        vec3(0)
    ),
    // left wall
    Primitive(
        2,
        1, 
        vec3(0), 0.0, 
        vec3(556, 0, 0), vec3(0, 0, 559.2), vec3(0, 548.8, 0),
        red,
        vec3(0)
    ),
    // ceil
    Primitive(
        3,
        1, 
        vec3(0), 0.0, 
        vec3(0, 548.8, 0), vec3(556, 0, 0), vec3(0, 0, 559.2),
        white1,
        vec3(0)
    ),
    // forward wall
    Primitive(
        4,
        1, 
        vec3(0), 0.0, 
        vec3(0, 0, 559.2), vec3(0, 548.8, 0), vec3(556, 0, 0),
        white1,
        vec3(0)
    ),
    // shortblock 1
    Primitive(
        5,
        1, 
        vec3(0), 0.0, 
        vec3(130, 165, 65), vec3(-48, 0, 160), vec3(160, 0, 49),
        white1,
        vec3(0)
    ),
    // shortblock 2
    Primitive(
        6,
        1, 
        vec3(0), 0.0, vec3(290, 0, 114), vec3(0, 165, 0), vec3(-50, 0, 158),
        white1,
        vec3(0)
    ),
    // shortblock 3
    Primitive(
        7,
        1, 
        vec3(0), 0.0, 
        vec3(130, 0, 65), vec3(0, 165, 0), vec3(160, 0, 49),
        white1,
        vec3(0)
    ),
    // shortblock 4
    Primitive(
        8,
        1, 
        vec3(0), 0.0, 
        vec3(82, 0, 225), vec3(0, 165, 0), vec3(48, 0, -160),
        white1,
        vec3(0)
    ),
    // shortblock 5
    Primitive(
        9,
        1, 
        vec3(0), 0.0, 
        vec3(240, 0, 272), vec3(0, 165, 0), vec3(-158, 0, -47),
        white1,
        vec3(0)
    ),
    // tallblock 1
    Primitive(
        10,
        1, 
        vec3(0), 0.0, 
        vec3(423, 330, 247), vec3(-158, 0, 49), vec3(49, 0, 159),
        white1,
        vec3(0)
    ),
    // tallblock 2
    Primitive(
        11,
        1, 
        vec3(0), 0.0, 
        vec3(423, 0, 247), vec3(0, 330, 0), vec3(49, 0, 159),
        white1,
        vec3(0)
    ),
    // tallblock 3
    Primitive(
        12,
        1, 
        vec3(0), 0.0, 
        vec3(472, 0, 406), vec3(0, 330, 0), vec3(-158, 0, 50),
        white1,
        vec3(0)
    ),
    // tallblock 4
    Primitive(
        13,
        1, 
        vec3(0), 0.0, 
        vec3(314, 0, 456), vec3(0, 330, 0), vec3(-49, 0, -160),
        white1,
        vec3(0)
    ),
    // tallblock 5
    Primitive(
        14,
        1, 
        vec3(0), 0.0, 
        vec3(265, 0, 296), vec3(0, 330, 0), vec3(158, 0, -49),
        white1,
        vec3(0)
    ),
    // light
    Primitive(
        15,
        1, 
        vec3(0), 0.0, 
        vec3(343, 548.6, 227), vec3(-130, 0, 0), vec3(0, 0, 105),
        white1,
        light
    )
);

Hit intersect(Ray ray) {
    Hit ret;
    ret.hit = false;
    ret.t = RAY_TMAX;

    for(int i = 0; i < 16; ++i) {
        Hit temp = intersect_each(ray, primitives[i]);
        if(temp.hit) {
            if(temp.t < ret.t) {
                temp.primID = primitives[i].id;
                ret = temp;
            }
        }
    }

    return ret;
}

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