#version 330 core

#include common/global.frag
#include common/uniform.frag
#include common/raygen.frag
#include common/util.frag
#include common/intersect.frag
#include common/closest_hit.frag

in vec2 texCoord;
out vec4 fragColor;

void main() {
    // generate initial ray
    vec2 uv = (2.0*gl_FragCoord.xy - resolution) * resolutionYInv;
    uv.y = -uv.y;
    Ray ray = rayGen(uv);

    vec3 color = vec3(0);
    IntersectInfo info;
    if(intersect(ray, info)) {
      Primitive hitPrimitive = primitives[info.primID];
      Material hitMaterial = materials[hitPrimitive.material_id];
      color = hitMaterial.kd;
    }

    fragColor = vec4(color, 1.0);
}