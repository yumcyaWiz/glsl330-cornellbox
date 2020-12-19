const float PI = 3.14159265358979323846;
const float PI_INV = 1.0 / PI;

const float RAY_TMIN =  0.001;
const float RAY_TMAX = 10000.0;
const int MAX_DEPTH = 100;

struct Ray {
    vec3 origin;
    vec3 direction;
};

struct IntersectInfo {
    float t;
    vec3 hitPos;
    vec3 hitNormal;
    vec3 dpdu;
    vec3 dpdv;
    float u;
    float v;
    int primID;
};

struct Material {
    int brdf_type;
    vec3 kd;
    vec3 le;
};

struct Primitive {
    int id;
    int type;
    vec3 center;
    float radius;
    vec3 leftCornerPoint;
    vec3 up;
    vec3 right;
    int material_id;
};

struct XORShift32_state {
    uint a;
};

XORShift32_state RNG_STATE;