bool intersect_each(in Ray ray, in Primitive primitive, out IntersectInfo info) {
    switch(primitive.type) {
    // Sphere
    case 0:
        return intersectSphere(primitive.center, primitive.radius, ray, info);
    // Plane
    case 1:
        return intersectPlane(primitive.leftCornerPoint, primitive.right, primitive.up, ray, info);
    }
}

bool intersect(in Ray ray, out IntersectInfo info) {
    bool hit = false;
    info.t = RAY_TMAX;

    for(int i = 0; i < n_primitives; ++i) {
        IntersectInfo temp;
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