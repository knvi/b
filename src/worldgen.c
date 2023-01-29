#include "worldgen.h"
#include "noise.h"
#include "mvmath.h"

enum Biome
{
    OCEAN,
    PLAINS,
    BEACH,
    MOUNTAIN
};

typedef void (*FSet)(struct Chunk *, int, int, int, block_id);
typedef block_id (*FGet)(struct Chunk *, int, int, int);

// make a function that takes c, r, v and returns a float in a RADIAL3I way

#define RADIAL2I(c, r, v) \
    (vec2_norm(subtract_v2r((c), (v))) / vec2_norm(r))

#define RADIAL3I(c, r, v) \
    (vec3_norm(subtract_v3((c), (v))) / vec3_norm(r))

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

    for (int xx = (x - 2); xx <= (x + 2); xx++)
    {
        for (int zz = (z - 2); zz <= (z + 2); zz++)
        {
            for (int yy = (y + h); yy <= (y + h + 1); yy++)
            {
                int c = 0;
                c += xx == (x - 2) || xx == (x + 2); // if it's on the edge of the leaves
                c += zz == (z - 2) || zz == (z + 2); // if it's on the edge of the leaves
                int corner = c == 2;                 // if it's a corner

                if ((!(xx == x && zz == z) || yy > (y + h)) &&
                    !(corner && yy == (y + h + 1) && RANDCHANCE(0.4)))
                {
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

void orevein(struct Chunk *chunk, FGet get, FSet set, int x, int y, int z, block_id block)
{
    int h = RAND(1, y - 4);

    if (h < 0 || h > y - 4)
        return;

    int s;

    switch (block)
    {
    case IRON_ORE:
        s = RAND(3, 6);
        break;
    case GOLD_ORE:
        s = RAND(2, 4);
        break;
    case COAL_ORE:
        s = RAND(4, 8);
        break;
    default:
        s = RAND(1, 3);
        break;
    }

    int l = RAND(s - 1, s + 1);
    int w = RAND(s - 1, s + 1);
    int i = RAND(s - 1, s + 1);

    for (int xx = (x - l); xx <= (x + l); xx++)
    {
        for (int zz = (z - w); zz <= (z + w); zz++)
        {
            for (int yy = (y - i); yy <= (y + i); yy++)
            {
                vec3 c = {x, y, z};
                vec3 r = {l + 1, i + 1, w + 1};
                vec3 v = {xx, yy, zz};

                float d = 1.0f - RADIAL3I(
                                     c, r, v);

                if (get(chunk, xx, yy, zz) == STONE && RANDCHANCE(0.2 + d * 0.7))
                {
                    set(chunk, xx, yy, zz, block);
                }
            }
        }
    }
}

void flowers(struct Chunk *chunk, FGet get, FSet set, int x, int y, int z)
{
    block_id flower = RANDCHANCE(0.6) ? ROSE : BUTTERCUP;

    int s = RAND(2, 6);
    int l = RAND(s - 1, s + 1);
    int h = RAND(s - 1, s + 1);

    for (int xx = (x - l); xx <= (x + l); xx++)
    {
        for (int zz = (z - h); zz <= (z + h); zz++)
        {
            if (get(chunk, xx, y, zz) == GRASS && RANDCHANCE(0.5))
            {
                set(chunk, xx, y + 1, zz, flower);
            }
        }
    }
}

void lavapool(struct Chunk *chunk, FGet get, FSet set, int x, int y, int z)
{
    int h = y - 1;

    int s = RAND(1, 5);
    int l = RAND(s - 1, s + 1);
    int w = RAND(s - 1, s + 1);

    for (int xx = (x - l); xx <= (x + l); xx++)
    {
        for (int zz = (z - w); zz <= (z + w); zz++)
        {
            vec2 c = {x, z};
            vec2 r = {l + 1, w + 1};
            vec2 v = {xx, zz};

            float d = 1.0f - RADIAL2I(
                c, r, v
            );

            int allow = 1; // all border blocks have to be solid or lava to be place lava

            for(int i = -1; i <= 1; i++) {
                for(int j = -1; j <= 1; j++) {
                    block_id block = get(chunk, xx + i, h, zz + j);
                    if(block != STILL_LAVA && !block_is_obstacle(block)) {
                        allow = 0;
                        break;
                    }
                }
            }

            if(!allow) continue;

            if(RANDCHANCE(0.2 + d * 0.95)) {
                set(chunk, xx, h, zz, STILL_LAVA);
            }
        }
    }
}

int hash(int x, int y, int z)
{
    int h = 0;
    h ^= x + 0x9e3779b9 + (h << 6) + (h >> 2);
    h ^= y + 0x9e3779b9 + (h << 6) + (h >> 2);
    h ^= z + 0x9e3779b9 + (h << 6) + (h >> 2);
    return h;
}

// Generate chunk block positions using noise
void worldgen_noise(struct Chunk *chunk)
{
    SRAND(chunk->world->seed + hash(chunk->x, 1, chunk->z));

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
            int hl = (cs[0].compute(&cs[0].params, chunk->world->seed, wx * base_scale, wz * base_scale) / 6.0f) - 4.0f;
            int hh = (cs[1].compute(&cs[1].params, chunk->world->seed, wx * base_scale, wz * base_scale) / 5.0f) + 6.0f;

            // Sample the "biome" noise
            f32 t = n.compute(&n.params, chunk->world->seed, wx, wz);
            f32 r = m.compute(&m.params, chunk->world->seed, wx / 4.0f, wz / 4.0f) / 32.0f;

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

                chunk_set_block(chunk, x, y, z, block);
            }

            for (int y = h; y < WATER_LEVEL; y++)
            {
                chunk_set_block(chunk, x, y, z, STILL_WATER);
            }

            if (biome == PLAINS && RANDCHANCE(0.005))
            {
                tree(chunk, _get, _set, x, h, z);
            }

            if (biome == PLAINS && RANDCHANCE(0.0085))
            {
                flowers(chunk, _get, _set, x, h, z);
            }

            if (biome != OCEAN && h <= (WATER_LEVEL + 3) && t < 0.1f && RANDCHANCE(0.001)) {
                lavapool(chunk, _get, _set, x, h, z);
            }

            if (RANDCHANCE(0.02))
            {
                orevein(chunk, _get, _set, x, h, z, IRON_ORE);
            }

            if (RANDCHANCE(0.02))
            {
                orevein(chunk, _get, _set, x, h, z, GOLD_ORE);
            }

            if (RANDCHANCE(0.02))
            {
                orevein(chunk, _get, _set, x, h, z, COAL_ORE);
            }
        }
    }
}

void worldgen_init(struct World *world)
{
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