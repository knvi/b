#include "chunk.h"
#include "world.h"
#include "noise.h"
#include "stdio.h"

enum Biome
{
    OCEAN,
    PLAINS,
    BEACH,
    MOUNTAIN
};

typedef void (*FSet)(struct Chunk *, int, int, int, block_id);
typedef block_id (*FGet)(struct Chunk *, int, int, int);

block_id _get(struct Chunk *chunk, int x, int y, int z)
{
    if (chunk_in_bounds(x, y, z))
    {
        return chunk_get_block(chunk, x, y, z);
    }
    else
    {
        return world_get_block(chunk->world, x + chunk->x * CHUNK_SIZE, y, z + chunk->z * CHUNK_SIZE);
        return 0;
    }
}

void _set(struct Chunk *chunk, int x, int y, int z, block_id b)
{
    if (chunk_in_bounds(x, y, z))
    {
        chunk_set_block(chunk, x, y, z, b);
    }
    else
    {
        world_set_block(chunk->world, x + chunk->x * CHUNK_SIZE, y, z + chunk->z * CHUNK_SIZE, b);
    }
}

void tree(struct Chunk *chunk, FGet get, FSet set, int x, int y, int z)
{
    block_id under = get(chunk, x, y - 1, z); // get the block under spawn
    if (under != GRASS && under != DIRT)
        return; // if it's not grass or dirt, don't spawn

    int h = RAND(3, 5); // generate a random height of logs

    for (int i = y; i < (y + h); i++)
    {
        set(chunk, x, i, z, WOOD); // set the logs
    }

    int lh = RAND(2, 3); // generate a random height of leaves

    for(int xx = (x - 2); xx <= (x + 2); xx++) {
        for(int zz = (z - 2); zz <= (z + 2); zz++) {
            for(int yy = (y + h); yy <= (y+h+1); yy++) {
                int c = 0;
                c += xx == (x - 2) || xx == (x + 2); // if it's on the edge of the leaves
                c += zz == (z - 2) || zz == (z + 2); // if it's on the edge of the leaves
                int corner = c == 2; // if it's a corner

                if ((!(xx == x && zz == z) || yy > (y + h)) &&
                    !(corner && yy == (y + h + 1) && RANDCHANCE(0.4))) {
                    set(chunk, xx, yy, zz, LEAVES);
                } // set the leaves
            }
        }
    }

    for (int xx = (x - 1); xx <= (x + 1); xx++)
    {
        for (int zz = (z - 1); zz <= (z + 1); zz++)
        {
            for (int yy = (y + h + 2); yy <= (y + h + lh); yy++)
            {
                int c = 0;
                c += xx == (x - 1) || xx == (x + 1);
                c += zz == (z - 1) || zz == (z + 1);
                int corner = c == 2;

                if (!(corner && yy == (y + h + lh) && RANDCHANCE(0.8)))
                {
                    set(chunk, xx, yy, zz, LEAVES);
                }
            }
        }
    }
}

// Generate chunk block positions using noise
void worldgen_noise(struct Chunk *chunk)
{
    const u64 seed = 1;

    // Base noise
    struct Noise n = octave(6, 0);

    // ore noise
    struct Noise m = octave(6, 1);

    // Different offsets of octave noise functions
    struct Noise os[] = {
        octave(8, 1),
        octave(8, 2),
        octave(8, 3),
        octave(8, 4),
        octave(8, 5),
        octave(8, 6),
    };

    // Two separate combined noise functions, each combining two different
    // octave noise functions
    struct Noise cs[] = {
        combined(&os[0], &os[1]),
        combined(&os[2], &os[3]),
        combined(&os[4], &os[5]),
    };

    for (int x = 0; x < CHUNK_SIZE; x++)
    {
        for (int z = 0; z < CHUNK_SIZE; z++)
        {
            int wx = chunk->x * CHUNK_SIZE + x;
            int wz = chunk->z * CHUNK_SIZE + z;

            // Sample each combined noise function for high/low results
            const f32 base_scale = 1.3f;
            int hr;
            int hl = (cs[0].compute(&cs[0].params, seed, wx * base_scale, wz * base_scale) / 6.0f) - 4.0f;
            int hh = (cs[1].compute(&cs[1].params, seed, wx * base_scale, wz * base_scale) / 5.0f) + 6.0f;

            // Sample the "biome" noise
            f32 t = n.compute(&n.params, seed, wx, wz);
            f32 r = m.compute(&m.params, seed, wx / 4.0f, wz / 4.0f) / 32.0f;

            if (t > 0)
            {
                hr = hl;
            }
            else
            {
                hr = max(hh, hl);
            }

            // offset by water level and determine biome
            int h = hr + WATER_LEVEL;

            // beach is anything close-ish to water in biome AND height
            enum Biome biome;
            if (h < WATER_LEVEL)
            {
                biome = OCEAN;
            }
            else if (t < 0.08f && h < WATER_LEVEL + 2)
            {
                biome = BEACH;
            }
            else if (0)
            {
                biome = MOUNTAIN;
            }
            else
            {
                biome = PLAINS;
            }

            if (biome == MOUNTAIN)
            {
                h += (r + (-t / 12.0f)) * 2 + 2;
            }

            s32 d = r * 1.4f + 5.0f; // dirt or sand depth

            block_id top_block;
            switch (biome)
            {
            case OCEAN:
                if (r > 0.8f)
                {
                    top_block = GRAVEL;
                }
                else if (r > 0.3f)
                {
                    top_block = SAND;
                }
                else
                {
                    top_block = DIRT;
                }
                break;
            case BEACH:
                top_block = SAND;
                break;
            case PLAINS:
                top_block = (t > 4.0f && r > 0.78f) ? GRAVEL : GRASS;
                break;
            case MOUNTAIN:
                if (r > 0.8f)
                {
                    top_block = GRAVEL;
                }
                else if (r > 0.7f)
                {
                    top_block = DIRT;
                }
                else
                {
                    top_block = STONE;
                }
                break;
            }

            for (int y = 0; y < h; y++)
            {
                block_id block;
                if (y == (h - 1))
                {
                    block = top_block;
                }
                else if (y > (h - d))
                {
                    if (top_block == GRASS)
                    {
                        block = DIRT;
                    }
                    else
                    {
                        block = top_block;
                    }
                }
                else
                {
                    block = STONE;
                }

                _set(chunk, x, y, z, block);
            }

            for (int y = h; y < WATER_LEVEL; y++)
            {
                _set(chunk, x, y, z, STILL_WATER);
            }

            if (biome == PLAINS && RANDCHANCE(0.005))
            {
                tree(chunk, _get, _set, x, h, z);
            } 
        }
    }
}

void worldgen_init(struct World *world) {
    for (int x = 0; x < WORLD_SIZE; x++)
    {
        for (int z = 0; z < WORLD_SIZE; z++)
        {
            struct Chunk *c = &world->chunks[x * WORLD_SIZE + z];

            // for (int x = 0; x < CHUNK_SIZE; x++)
            // {
            //     for (int y = 0; y < WORLD_HEIGHT; y++)
            //     {
            //         for (int z = 0; z < CHUNK_SIZE; z++)
            //         {
            //             if (y > GRASS_LEVEL)
            //                 c->blocks[x][y][z] = AIR;
            //             else if (y == GRASS_LEVEL)
            //                 c->blocks[x][y][z] = GRASS;
            //             else if (y == 0)
            //                 c->blocks[x][y][z] = BEDROCK;
            //             else if (y < 25)
            //                 c->blocks[x][y][z] = STONE;
            //             else if (y < GRASS_LEVEL)
            //                 c->blocks[x][y][z] = DIRT;
            //         }
            //     }
            // }

            worldgen_noise(c);
        }
    }
}