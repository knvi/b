#ifndef NOISE_H
#define NOISE_H

#include "world.h"
#include "chunk.h"

#include <noise1234.h>
#include <stdlib.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef float f32;
typedef double f64;

#define SRAND(seed) srand(seed)
#define RAND(min, max) ((rand() % (max - min + 1)) + min)
#define RANDCHANCE(chance) ((RAND(0, 100) / 100.0f) <= chance)

#define WATER_LEVEL 64

typedef f32 (*FNoise)(void *p, f32 s, f32 x, f32 z);

struct Noise {
    u8 params[512]; // either Octave or Combined
    FNoise compute;
};

// Octave noise with n octaves and seed offset o
// Maximum amplitude is 2^0 + 2^1 + 2^2 ... 2^n = 2^(n+1) - 1
// i.e. for octave 8, values range between [-511, 511]
struct Octave {
    s32 n, o;
};

// Combined noise where compute(x, z) = n.compute(x + m.compute(x, z), z)
struct Combined {
    struct Noise *n, *m;
};

f32 octave_compute(struct Octave *p, f32 seed, f32 x, f32 z) ;
struct Noise octave(s32 n, s32 o);
f32 combined_compute(struct Combined *p, f32 seed, f32 x, f32 z);
struct Noise combined(struct Noise *n, struct Noise *m);

#endif
