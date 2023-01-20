#ifndef WORLDGEN_H
#define WORLDGEN_H

#include "chunk.h"
#include "world.h"

void worldgen_noise(struct Chunk *chunk);
void worldgen_init(struct World *world);

#endif