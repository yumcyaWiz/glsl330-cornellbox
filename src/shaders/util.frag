float atan2(in float y, in float x) {
    return x == 0.0 ? sign(y) * PI / 2.0 : atan(y, x);
}

vec3 worldToLocal(in vec3 v, in vec3 lx, in vec3 ly, in vec3 lz) {
    return vec3(dot(v, lx), dot(v, ly), dot(v, lz));
}
vec3 localToWorld(in vec3 v, in vec3 lx, in vec3 ly, in vec3 lz) {
    return vec3(dot(v, vec3(lx.x, ly.x, lz.x)), dot(v, vec3(lx.y, ly.y, lz.y)), dot(v, vec3(lx.z, ly.z, lz.z)));
}