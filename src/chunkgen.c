#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "threading.h"
#include "world.h"
#include "worldgen.h"

void world_generate_new_chunks(struct World* w, int new_x, int new_z)
{
    int current_chunks = w->size.x * w->size.z;
    int new_chunks = (w->size.x + new_x) * (w->size.z + new_z);

    struct Chunk* p_old_chunks = w->chunks;
    w->chunks = calloc(new_chunks, sizeof(struct Chunk));
#ifdef _WIN32
    memcpy_s(w->chunks, new_chunks * sizeof(struct Chunk), p_old_chunks, current_chunks * sizeof(struct Chunk));
#else
    memcpy(w->chunks, p_old_chunks, current_chunks * sizeof(struct Chunk));
#endif
    free(p_old_chunks);

    assert(w->chunks != NULL);
    struct Chunk* chunk = w->chunks + current_chunks;
    struct Chunk* end = w->chunks + new_chunks;
    while (chunk < end) {
        chunk_init(chunk, w, (chunk - w->chunks) / w->size.z, (chunk - w->chunks) % w->size.z, &w->blocks_shader);
        start_thread(worldgen_noise, chunk);

        chunk++;
    }
    w->size.x += new_x;
    w->size.z += new_z;
}

void *world_check_chunk_generation(struct World* w)
{

    vec2 playerPos = { w->player.position.x, w->player.position.z };
    int safe_x_min = w->size.x / 4;
    int safe_x_max = safe_x_min + w->size.x / 2;
    int safe_z_min = w->size.z / 4;
    int safe_z_max = safe_z_min + w->size.z / 2;

    if(playerPos.x > safe_x_max && !w->generating_chunks)
    {
        w->generating_chunks = 1;
        world_generate_new_chunks(w, 1, 0);
    }
    if(playerPos.y > safe_z_max && !w->generating_chunks)
    {
        w->generating_chunks = 1;
        world_generate_new_chunks(w, 0, 1);
    }
    w->generating_chunks = 0;
}

void world_check_chunk_management(struct World* w)
{
	world_check_chunk_generation(w);
}

void start_chunk_generation_thread(struct World* w) {
    // start_thread(world_check_chunk_management, w);
    world_check_chunk_management(w);
}

