uint xorshift32(inout XORShift32_state state) {
    uint x = state.a;
    x ^= x << 13u;
    x ^= x >> 17u;
    x ^= x << 5u;
    state.a = x;
    return x;
}

float random() {
    return float(xorshift32(RNG_STATE)) * 2.3283064e-10;
}

void setSeed(vec2 uv) {
    RNG_STATE.a = texture(stateTexture, uv).x;
}