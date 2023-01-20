#include "chunk.h"
#include "world.h"
#include "block_data.h"
#include "noise.h"
#include "assert.h"
#include <stdio.h>

#include <glad/glad.h>
#include <stddef.h>

void chunk_build_buffer(struct Chunk *c, block_vertex *data_buffer)
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
                    neighbours[0] = world_get_block(c->world, x + x_off, y, z + z_off + 1);
                    neighbours[1] = world_get_block(c->world, x + x_off, y, z + z_off - 1);
                    neighbours[2] = world_get_block(c->world, x + x_off + 1, y, z + z_off);
                    neighbours[3] = world_get_block(c->world, x + x_off - 1, y, z + z_off);
                    neighbours[4] = world_get_block(c->world, x + x_off, y + 1, z + z_off);
                    neighbours[5] = world_get_block(c->world, x + x_off, y - 1, z + z_off);

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
                    neighbours[0] = world_get_block(c->world, x + x_off, y, z + z_off + 1);
                    neighbours[1] = world_get_block(c->world, x + x_off, y, z + z_off - 1);
                    neighbours[2] = world_get_block(c->world, x + x_off + 1, y, z + z_off);
                    neighbours[3] = world_get_block(c->world, x + x_off - 1, y, z + z_off);
                    neighbours[4] = world_get_block(c->world, x + x_off, y + 1, z + z_off);
                    if(y > 0) {
                        neighbours[5] = world_get_block(c->world, x + x_off, y - 1, z + z_off);
                    } else {
                        neighbours[5] = AIR;
                    }

                    c->water_count += make_block(data_buffer + c->vert_count + c->water_count, (vec3){x, y, z}, b, neighbours);
                }
            }
        }
    }

    glBufferData(GL_ARRAY_BUFFER, (c->vert_count + c->water_count) * sizeof(block_vertex), data_buffer, GL_STATIC_DRAW);

    c->dirty = 0;
}

void chunk_init(struct Chunk *c, struct World *w, int x, int z, shader *blocks_shader)
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
    c->world = w;

    c->vert_count = 0;
    c->dirty = 0;
}

void chunk_destroy(struct Chunk *c)
{
    glDeleteBuffers(1, &c->vbo);
    glDeleteVertexArrays(1, &c->vao);
}

// returns true if pos is on chunk boundaries (borders another chunk)
int chunk_on_bounds(int x, int y, int z) {
    return x == 0 || z == 0 || x == (CHUNK_SIZE - 1) || z == (CHUNK_SIZE - 1);
}

int chunk_in_bounds(int x, int y, int z) {
    return x >= 0 && y >= 0 && z >= 0 &&
        x < CHUNK_SIZE && y < WORLD_HEIGHT && z < CHUNK_SIZE;
}

block_id chunk_get_block(struct Chunk *c, int x, int y, int z)
{
    assert(chunk_in_bounds(x,y,z));

    return c->blocks[x][y][z];
}

void chunk_set_block(struct Chunk *c, int x, int y, int z, block_id b)
{
    assert(chunk_in_bounds(x,y,z));

    c->blocks[x][y][z] = b;
    c->dirty = 1;

    if(x == 0) {
        struct Chunk *c1 = world_get_chunk(c->world, c->x - 1, c->z);
        if(c1) c1->dirty = 1;
    }

    if(x == CHUNK_SIZE - 1) {
        struct Chunk *c1 = world_get_chunk(c->world, c->x + 1, c->z);
        if(c1) c1->dirty = 1;
    }

    if(z == 0) {
        struct Chunk *c1 = world_get_chunk(c->world, c->x, c->z - 1);
        if(c1) c1->dirty = 1;
    }

    if(z == CHUNK_SIZE - 1) {
        struct Chunk *c1 = world_get_chunk(c->world, c->x, c->z + 1);
        if(c1) c1->dirty = 1;
    }
}