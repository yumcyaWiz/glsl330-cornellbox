Ray rayGen(in vec2 uv) {
    vec3 pinholePos = camPos + 1.7071067811865477 * camForward;
    vec3 sensorPos = camPos + uv.x * camRight + uv.y * camUp;

    Ray ray;
    ray.origin = camPos;
    ray.direction = normalize(pinholePos - sensorPos);
    return ray;
}