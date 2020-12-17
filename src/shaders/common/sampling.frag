vec3 sampleCosineHemisphere(in float u, in float v, out float pdf) {
    float theta = 0.5 * acos(clamp(1.0 - 2.0 * u, -1.0, 1.0));
    float phi = 2.0 * PI * v;
    float y = cos(theta);
    pdf = y * PI_INV;
    return vec3(cos(phi) * sin(theta), y, sin(phi) * sin(theta));
}
