#version 330 core

#include global.glsl
#include uniform.glsl

layout (location = 0) out vec3 color;
layout (location = 1) out uint state;

float atan2(float y, float x) {
    return x == 0.0 ? sign(y) * PI / 2.0 : atan(y, x);
}

Ray rayGen(vec2 uv) {
    vec3 camPos = vec3(278, 273, -900);
    vec3 camForward = vec3(0, 0, 1);
    vec3 camRight = vec3(1, 0, 0);
    vec3 camUp = vec3(0, 1, 0);
    vec3 pinholePos = camPos + 1.7071067811865477 * camForward;
    vec3 sensorPos = camPos + uv.x * camRight + uv.y * camUp;

    Ray ray;
    ray.origin = camPos;
    ray.direction = normalize(pinholePos - sensorPos);
    return ray;
}

Hit intersect_each(Ray ray, Primitive primitive) {
    Hit ret;

    // Sphere
    if(primitive.type == 0) {
        Hit ret;

        float b = dot(ray.origin - primitive.center, ray.direction);
        float len = length(ray.origin - primitive.center);
        float c = len*len - primitive.radius*primitive.radius;
        float D = b*b - c;
        if(D < 0.0) {
            ret.hit = false;
            return ret;
        }

        float t0 = -b - sqrt(D);
        float t1 = -b + sqrt(D);
        float t = t0;
        if(t < RAY_TMIN || t > RAY_TMAX) {
            t = t1;
            if(t < RAY_TMIN || t > RAY_TMAX) { 
                ret.hit = false;
                return ret;
            }
        }

        ret.hit = true;
        ret.t = t;
        ret.hitPos = ray.origin + t*ray.direction;

        vec3 r = ret.hitPos - primitive.center;
        ret.hitNormal = normalize(r);
        ret.dpdu = normalize(vec3(-r.z, 0, r.x));

        float phi = atan2(r.z, r.x);
        if(phi < 0.0) {
            phi += 2.0 * PI;
        }
        float theta = acos(clamp(r.y / primitive.radius, -1.0, 1.0));
        ret.dpdv = normalize(vec3(cos(phi) * r.y, -primitive.radius * sin(theta), sin(phi) * r.y));

        ret.u = phi / (2.0 * PI);
        ret.v = theta / PI;

        return ret;
    }
    // Plane
    else if(primitive.type == 1) {
        vec3 normal = normalize(cross(primitive.right, primitive.up));
        vec3 center = primitive.leftCornerPoint + 0.5 * primitive.right + 0.5 * primitive.up;
        vec3 rightDir = normalize(primitive.right);
        float rightLength = length(primitive.right);
        vec3 upDir = normalize(primitive.up);
        float upLength = length(primitive.up);

        Hit ret;
        float t = -dot(ray.origin - center, normal) / dot(ray.direction, normal);
        if(t < RAY_TMIN || t > RAY_TMAX) {
            ret.hit = false;
            return ret;
        }

        vec3 hitPos = ray.origin + t*ray.direction;
        float dx = dot(hitPos - primitive.leftCornerPoint, rightDir);
        float dy = dot(hitPos - primitive.leftCornerPoint, upDir);
        if(dx < 0.0 || dx > rightLength || dy < 0.0 || dy > upLength) {
            ret.hit = false;
            return ret;
        }

        ret.hit = true;
        ret.t = t;
        ret.hitPos = hitPos;
        if(dot(-ray.direction, normal) > 0.0) {
            ret.hitNormal = normal;
        }
        else {
            ret.hitNormal = -normal;
        }
        ret.dpdu = rightDir;
        ret.dpdv = upDir;
        ret.u = dx / rightLength;
        ret.v = dy / upLength;
        return ret;
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

uint xorshift32(inout XORShift32_state state) {
    uint x = state.a;
    x ^= x << 13u;
    x ^= x >> 17u;
    x ^= x << 5u;
    state.a = x;
    return x;
}

float random() {
    return float(xorshift32(RNG_STATE)) * 2.3283064e-10;
}

uint hash( uint x ) {
    x += ( x << 10u );
    x ^= ( x >>  6u );
    x += ( x <<  3u );
    x ^= ( x >> 11u );
    x += ( x << 15u );
    return x;
}

void setSeed(vec2 uv) {
    // RNG_STATE.a = hash(uint(gl_FragCoord.x + resolution.x * gl_FragCoord.y)) + hash(uint(samples));
    RNG_STATE.a = texture(stateTexture, uv).x;
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