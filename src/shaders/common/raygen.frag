Ray rayGen(in vec2 uv, out float pdf) {
    vec3 pinholePos = camera.camPos + camera.a * camera.camForward;
    vec3 sensorPos = camera.camPos + uv.x * camera.camRight + uv.y * camera.camUp;

    Ray ray;
    ray.origin = camera.camPos;
    ray.direction = normalize(pinholePos - sensorPos);
    pdf = 1.0 / pow(dot(ray.direction, camera.camForward), 3.0);
    return ray;
}