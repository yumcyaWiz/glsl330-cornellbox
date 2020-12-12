#define PI 3.14159265358979323846

#define RAY_TMIN 0.001
#define RAY_TMAX 10000.0
#define MAX_DEPTH 100

struct Ray {
    vec3 origin;
    vec3 direction;
};

struct Hit {
    bool hit;
    float t;
    vec3 hitPos;
    vec3 hitNormal;
    vec3 dpdu;
    vec3 dpdv;
    float u;
    float v;
    int primID;
};

struct Primitive {
    int id;
    int type;
    vec3 center;
    float radius;
    vec3 leftCornerPoint;
    vec3 up;
    vec3 right;
    vec3 kd;
    vec3 le;
};

struct XORShift32_state {
    uint a;
};

XORShift32_state RNG_STATE;