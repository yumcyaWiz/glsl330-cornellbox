vec3 sampleCosineHemisphere(in float u, in float v, out float pdf) {
    float theta = 0.5 * acos(clamp(1.0 - 2.0 * u, -1.0, 1.0));
    float phi = 2.0 * PI * v;
    float y = cos(theta);
    pdf = y * PI_INV;
    return vec3(cos(phi) * sin(theta), y, sin(phi) * sin(theta));
}

vec3 samplePlane(in float u, in float v, in vec3 leftCornerPoint, in vec3 right, in vec3 up, out float pdf_area) {
    pdf_area = 1.0 / (length(right) * length(up));
    return leftCornerPoint + u*right + v*up;
}

vec3 samplePointOnPrimitive(in Primitive primitive, out float pdf_area) {
    switch(primitive.type) {
        // Sphere
        case 0:
        break;
        // Plane
        case 1:
        return samplePlane(random(), random(), primitive.leftCornerPoint, primitive.right, primitive.up, pdf_area);
    }
}