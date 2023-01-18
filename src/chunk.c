#include "chunk.h"
#include "world.h"
#include "block_data.h"
#include "noise.h"

#include <glad/glad.h>
#include <stddef.h>

void chunk_build_buffer(chunk *c, void *w, block_vertex *data_buffer)
{
    c->vert_count = 0;

    block_id neighbours[6];

    int x_off = CHUNK_SIZE * c->x;
    int z_off = CHUNK_SIZE * c->z;

    for (int x = 0; x < CHUNK_SIZE; x++)
    {
        for (int y = 0; y < WORLD_HEIGHT; y++)
        {
            for (int z = 0; z < CHUNK_SIZE; z++)
            {
                block_id b = c->blocks[x][y][z];
                if (b != AIR && b != STILL_WATER && b != FLOWING_WATER)
                {
                    neighbours[0] = world_get_block((world *)w, x + x_off, y, z + z_off + 1);
                    neighbours[1] = world_get_block((world *)w, x + x_off, y, z + z_off - 1);
                    neighbours[2] = world_get_block((world *)w, x + x_off + 1, y, z + z_off);
                    neighbours[3] = world_get_block((world *)w, x + x_off - 1, y, z + z_off);
                    neighbours[4] = world_get_block((world *)w, x + x_off, y + 1, z + z_off);
                    neighbours[5] = world_get_block((world *)w, x + x_off, y - 1, z + z_off);

                    c->vert_count += make_block(data_buffer + c->vert_count, (vec3){x, y, z}, b, neighbours);
                }
            }
        }
    }

    c->water_offset = c->vert_count;
    c->water_count = 0;

    for (int x = 0; x < CHUNK_SIZE; x++)
    {
        for (int y = 0; y < WORLD_HEIGHT; y++)
        {
            for (int z = 0; z < CHUNK_SIZE; z++)
            {
                block_id b = c->blocks[x][y][z];
                if (b == STILL_WATER || b == FLOWING_WATER)
                {
                    neighbours[0] = world_get_block((world *) w, x + x_off, y, z + z_off + 1);
                    neighbours[1] = world_get_block((world *) w, x + x_off, y, z + z_off - 1);
                    neighbours[2] = world_get_block((world *) w, x + x_off + 1, y, z + z_off);
                    neighbours[3] = world_get_block((world *) w, x + x_off - 1, y, z + z_off);
                    neighbours[4] = world_get_block((world *) w, x + x_off, y + 1, z + z_off);
                    neighbours[5] = world_get_block((world *) w, x + x_off, y - 1, z + z_off);

                    c->water_count += make_block(data_buffer + c->vert_count + c->water_count, (vec3){x, y, z}, b, neighbours);
                }
            }
        }
    }

    glBufferData(GL_ARRAY_BUFFER, (c->vert_count + c->water_count) * sizeof(block_vertex), data_buffer, GL_STATIC_DRAW);

    c->dirty = 0;
}

void chunk_init(chunk *c, int x, int z, shader *blocks_shader)
{
    glGenBuffers(1, &c->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, c->vbo);

    glGenVertexArrays(1, &c->vao);
    glBindVertexArray(c->vao);
    glEnableVertexAttribArray(blocks_shader->position_location);
    glVertexAttribPointer(blocks_shader->position_location, 3, GL_FLOAT, GL_FALSE, sizeof(block_vertex), NULL);
    glEnableVertexAttribArray(blocks_shader->normal_location);
    glVertexAttribPointer(blocks_shader->normal_location, 3, GL_FLOAT, GL_FALSE, sizeof(block_vertex), (GLvoid *)sizeof(vec3));
    glEnableVertexAttribArray(blocks_shader->tex_coord_location);
    glVertexAttribPointer(blocks_shader->tex_coord_location, 2, GL_FLOAT, GL_FALSE, sizeof(block_vertex), (GLvoid *)(sizeof(vec3) * 2));

    c->x = x;
    c->z = z;

    c->vert_count = 0;
    c->dirty = 0;
}

void chunk_destroy(chunk *c)
{
    glDeleteBuffers(1, &c->vbo);
    glDeleteVertexArrays(1, &c->vao);
}

float noise(int x, int y, int z)
{
    int X = (x >> 13) ^ x;
    int Y = (y >> 13) ^ y;
    int Z = (z >> 13) ^ z;

    X = (X * (X * X * 15731 + 789221) + 1376312589);
    Y = (Y * (Y * Y * 15731 + 789221) + 1376312589);
    Z = (Z * (Z * Z * 15731 + 789221) + 1376312589);

    return 1.0 - ((X ^ Y ^ Z) & 0x7fffffff) / 1073741824.0;
}

float noise3d(int x, int y, int z, float scale, int octaves)
{
    float total = 0;
    float frequency = scale;
    float amplitude = 1;
    float maxValue = 0;  // Used for normalizing result to 0.0 - 1.0
    for (int i = 0; i < octaves; i++)
    {
        total += noise(x * frequency, y * frequency, z * frequency) * amplitude;

        maxValue += amplitude;

        amplitude *= 0.5;
        frequency *= 2;
    }

    return total / maxValue;
}

// Generate chunk block positions using noise
void worldgen_gen(chunk *chunk)
{
    const u64 seed = 1;

    // Base noise
    struct Noise n = octave(6, 0);

    // Different offsets of octave noise functions
    struct Noise os[] = {
        octave(8, 1),
        octave(8, 2),
        octave(8, 3),
        octave(8, 4),
    };

    // Two separate combined noise functions, each combining two different
    // octave noise functions
    struct Noise cs[] = {
        combined(&os[0], &os[1]),
        combined(&os[2], &os[3]),
    };

    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            int wx = chunk->x * CHUNK_SIZE + x;
            int wz = chunk->z * CHUNK_SIZE + z;

            // Sample each combined noise function for high/low results
            const f32 base_scale = 1.3f;
            int hr;
            int hl = (cs[0].compute(&cs[0].params, seed, wx * base_scale, wz * base_scale) / 6.0f) - 4.0f;
            int hh = (cs[1].compute(&cs[1].params, seed, wx * base_scale, wz * base_scale) / 5.0f) + 6.0f;

            // Sample the "biome" noise
            f32 t = n.compute(&n.params, seed, wx, wz);

            if (t > 0) {
                hr = hl;
            } else {
                hr = max(hh, hl);
            }

            // offset by water level and determine biome
            int h = hr + WATER_LEVEL;

            // beach is anything close-ish to water in biome AND height
            enum Biome biome = (h < WATER_LEVEL ?
                OCEAN :
                ((t < 0.08f && h < WATER_LEVEL + 2) ? BEACH : PLAINS));

            for (int y = 0; y < h; y++) {
                int block;
                if (y == (h - 1)) {
                    // Determine top block according to biome
                    switch (biome) {
                        case OCEAN:
                            // put sand floors in some places in the ocean
                            block = (t > 0.03f ? DIRT : SAND);
                            break;
                        case BEACH:
                            block = SAND;
                            break;
                        case PLAINS:
                            block = GRASS;
                            break;
                    }
                } else if (y > (h - 4)) {
                    // TODO: varying dirt layer height
                    block = biome == BEACH ? SAND : DIRT;
                } else {
                    block = STONE;
                }

                chunk->blocks[x][y][z] = block;
                
            }
            // TODO: water
            // TODO: trees
            // TODO: flowers
        }
    }
    chunk->dirty = 1;
}