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