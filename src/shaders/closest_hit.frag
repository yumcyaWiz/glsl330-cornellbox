Hit intersect_each(in Ray ray, in Primitive primitive) {
    Hit ret;

    // Sphere
    if(primitive.type == 0) {
        intersectSphere(primitive.center, primitive.radius, ray, ret);
    }
    // Plane
    else if(primitive.type == 1) {
        intersectPlane(primitive.leftCornerPoint, primitive.right, primitive.up, ray, ret);
    }
    
    return ret;
}

Hit intersect(in Ray ray) {
    Hit ret;
    ret.hit = false;
    ret.t = RAY_TMAX;

    for(int i = 0; i < 16; ++i) {
        Hit temp = intersect_each(ray, primitives[i]);
        if(temp.hit) {
            if(temp.t < ret.t) {
                temp.primID = primitives[i].id;
                ret = temp;
            }
        }
    }

    return ret;
}