Ray rayGen(in vec2 uv, out float pdf) {
    vec3 pinholePos = camPos + 1.7071067811865477 * camForward;
    vec3 sensorPos = camPos + uv.x * camRight + uv.y * camUp;

    Ray ray;
    ray.origin = camPos;
    ray.direction = normalize(pinholePos - sensorPos);
    pdf = 1.0 / pow(dot(ray.direction, camForward), 3.0);
    return ray;
}