#include "noise.h"
#include "string.h"


f32 octave_compute(struct Octave *p, f32 seed, f32 x, f32 z) {
    f32 u = 1.0f, v = 0.0f;
    for (int i = 0; i < p->n; i++) {
        v += noise3(x / u, z / u, seed + i + (p->o * 32)) * u;
        u *= 2.0f;
    }
    return v;
}

struct Noise octave(s32 n, s32 o) {
    struct Noise result = { .compute = (FNoise) octave_compute };
    struct Octave params = { n, o };
    memcpy(&result.params, &params, sizeof(struct Octave));
    return result;
}

f32 combined_compute(struct Combined *p, f32 seed, f32 x, f32 z) {
    return p->n->compute(&p->n->params, seed, x + p->m->compute(&p->m->params, seed, x, z), z);
}

struct Noise combined(struct Noise *n, struct Noise *m) {
    struct Noise result = { .compute = (FNoise) combined_compute };
    struct Combined params = { n, m };
    memcpy(&result.params, &params, sizeof(struct Combined));
    return result;
}
