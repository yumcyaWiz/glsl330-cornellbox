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