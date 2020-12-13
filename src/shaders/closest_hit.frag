bool intersect_each(in Ray ray, in Primitive primitive, out Hit info) {
    // Sphere
    if(primitive.type == 0) {
        return intersectSphere(primitive.center, primitive.radius, ray, info);
    }
    // Plane
    else if(primitive.type == 1) {
        return intersectPlane(primitive.leftCornerPoint, primitive.right, primitive.up, ray, info);
    }
}

bool intersect(in Ray ray, out Hit info) {
    bool hit = false;
    info.t = RAY_TMAX;

    for(int i = 0; i < 16; ++i) {
        Hit temp;
        if(intersect_each(ray, primitives[i], temp)) {
            if(temp.t < info.t) {
                hit = true;
                temp.primID = primitives[i].id;
                info = temp;
            }
        }
    }

    return hit;
}