
bool intersectSphere(in vec3 center, in float radius, in Ray ray, out IntersectInfo info) {
    float b = dot(ray.origin - center, ray.direction);
    float len = length(ray.origin - center);
    float c = len*len - radius*radius;
    float D = b*b - c;
    if(D < 0.0) {
        return false;
    }

    float t0 = -b - sqrt(D);
    float t1 = -b + sqrt(D);
    float t = t0;
    if(t < RAY_TMIN || t > RAY_TMAX) {
        t = t1;
        if(t < RAY_TMIN || t > RAY_TMAX) { 
            return false;
        }
    }

    info.t = t;
    info.hitPos = ray.origin + t*ray.direction;

    vec3 r = info.hitPos - center;
    info.hitNormal = normalize(r);
    info.dpdu = normalize(vec3(-r.z, 0, r.x));

    float phi = atan2(r.z, r.x);
    if(phi < 0.0) {
        phi += 2.0 * PI;
    }
    float theta = acos(clamp(r.y / radius, -1.0, 1.0));
    info.dpdv = normalize(vec3(cos(phi) * r.y, -radius * sin(theta), sin(phi) * r.y));

    info.u = phi / (2.0 * PI);
    info.v = theta / PI;
    return true;
}

bool intersectPlane(in vec3 leftCornerPoint, in vec3 right, in vec3 up, in Ray ray, out IntersectInfo info) {
    vec3 normal = normalize(cross(right, up));
    vec3 center = leftCornerPoint + 0.5 * right + 0.5 * up;
    vec3 rightDir = normalize(right);
    float rightLength = length(right);
    vec3 upDir = normalize(up);
    float upLength = length(up);

    float t = -dot(ray.origin - center, normal) / dot(ray.direction, normal);
    if(t < RAY_TMIN || t > RAY_TMAX) {
        return false;
    }

    vec3 hitPos = ray.origin + t*ray.direction;
    float dx = dot(hitPos - leftCornerPoint, rightDir);
    float dy = dot(hitPos - leftCornerPoint, upDir);
    if(dx < 0.0 || dx > rightLength || dy < 0.0 || dy > upLength) {
        return false;
    }

    info.t = t;
    info.hitPos = hitPos;
    info.hitNormal = normal;
    info.dpdu = rightDir;
    info.dpdv = upDir;
    info.u = dx / rightLength;
    info.v = dy / upLength;
    return true;
}