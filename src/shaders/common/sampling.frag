vec3 sampleHemisphere(in float u, in float v, out float pdf) {
    float theta = acos(clamp(1.0 - u, -1.0, 1.0));
    float phi = 2.0 * PI * v;
    float y = cos(theta);
    pdf = PI_2_INV;
    return vec3(cos(phi) * sin(theta), y, sin(phi) * sin(theta));
}

vec3 sampleCosineHemisphere(in float u, in float v, out float pdf) {
    float theta = 0.5 * acos(clamp(1.0 - 2.0 * u, -1.0, 1.0));
    float phi = 2.0 * PI * v;
    float y = cos(theta);
    pdf = y * PI_INV;
    return vec3(cos(phi) * sin(theta), y, sin(phi) * sin(theta));
}

vec3 samplePlane(in float u, in float v, in vec3 leftCornerPoint, in vec3 right, in vec3 up, out vec3 normal, out vec3 dpdu, out vec3 dpdv, out float pdf_area) {
    normal = normalize(cross(right, up));
    dpdu = normalize(right);
    dpdv = normalize(up);
    pdf_area = 1.0 / (length(right) * length(up));
    return leftCornerPoint + u*right + v*up;
}

vec3 sampleSphere(in float u, in float v, in vec3 center, in float radius, out vec3 normal, out vec3 dpdu, out vec3 dpdv, out float pdf_area) {
    pdf_area = 1.0 / (4.0 * PI * radius * radius);
    float theta = acos(1.0 - 2.0 * u);
    float phi = 2.0 * PI * v;
    float sinPhi = sin(phi);
    float cosPhi = cos(phi);
    float sinTheta = sin(theta);
    vec3 r = radius * vec3(cosPhi*sinTheta, cos(theta), sinPhi*sinTheta);
    normal = normalize(r);
    dpdu = normalize(vec3(-r.z, 0, r.x));
    dpdv = normalize(vec3(cosPhi * r.y, -radius * sinTheta, sinPhi * r.y));
    return center + r;
}

vec3 samplePointOnPrimitive(in Primitive primitive, out vec3 normal, out vec3 dpdu, out vec3 dpdv, out float pdf_area) {
    switch(primitive.type) {
        // Sphere
        case 0:
        return sampleSphere(random(), random(), primitive.center, primitive.radius, normal, dpdu, dpdv, pdf_area);
        // Plane
        case 1:
        return samplePlane(random(), random(), primitive.leftCornerPoint, primitive.right, primitive.up, normal, dpdu, dpdv, pdf_area);
    }
}