float fresnel(in vec3 v, in float n1, in float n2) {
    float F0 = pow((n1 - n2) / (n1 + n2), 2.0);
    return F0 + (1.0 - F0) * pow(1.0 - abs(v.y), 5.0);
}

vec3 BRDF(in vec3 wo, in vec3 wi, in int brdf_type, in vec3 kd) {
    switch(brdf_type) {
        // lambert
        case 0:
        return kd * PI_INV;
        break;
        // mirror
        case 1:
        return kd / abs(wi.y);
        break;
        // glass
        case 2:
        return kd / abs(wi.y);
        break;
    }
}

vec3 sampleBRDF(in vec3 wo, in int brdf_type, out float pdf) {
    switch(brdf_type) {
        // lambert
        case 0:
        return sampleCosineHemisphere(random(), random(), pdf);

        // mirror
        case 1:
        pdf = 1.0;
        return reflect(-wo, vec3(0, 1, 0));

        // glass
        case 2:
        pdf = 1.0;

        // set appropriate normal and ior
        vec3 n = vec3(0, 1, 0);
        float ior1 = 1.0;
        float ior2 = 1.5;
        if(wo.y < 0.0) {
            n = vec3(0, -1, 0);
            ior1 = 1.5;
            ior2 = 1.0;
        }
        float eta = ior1 / ior2;

        // fresnel
        float fr = fresnel(wo, ior1, ior2);

        // reflection
        vec3 ret;
        if(random() < fr) {
            ret = reflect(-wo, n);
        }
        // refract
        else {
            ret = refract(-wo, n, eta);
            // total reflection
            if(ret == vec3(0)) {
                ret = reflect(-wo, n);
            }
        }
        return ret;
    }
}