#ifndef CHUNK_H
#define CHUNK_H

#include "block.h"
#include "mesh.h"
#include "shader.h"
#include "world.h"

#include <glad/glad.h>

#define CHUNK_SIZE 16
#define WORLD_HEIGHT 128

struct Chunk
{
    block_id blocks[CHUNK_SIZE][WORLD_HEIGHT][CHUNK_SIZE];
    int x;
    int z;
    int dirty;
    GLuint vao;
    GLuint vbo;
    GLuint vert_count;
    GLuint water_offset;
    GLuint water_count;

    struct World *world;

};

void chunk_build_buffer(struct Chunk *c, block_vertex *data_buffer);
void chunk_init(struct Chunk *c, struct World *w, int x, int z, shader *blocks_shader);
void chunk_destroy(struct Chunk *c);
void chunk_set_block(struct Chunk *c, int x, int y, int z, block_id b);
block_id chunk_get_block(struct Chunk *c, int x, int y, int z);
int chunk_in_bounds(int x, int y, int z);


#endif