#ifndef WORLD_H
#define WORLD_H

#include "chunk.h"
#include "entity.h"
#include "input.h"
#include "mvmath.h"
#include "shader.h"
#include "sockets.h"
#include "noise.h"

#define ULONG_MAX 0xFFFFFFFFUL

#define WORLD_TO_CHUNK(x) (x < 0 ? x % CHUNK_SIZE == 0 ? 0 : CHUNK_SIZE + x % CHUNK_SIZE : x % CHUNK_SIZE)
#define CHUNK_FROM_WORLD_COORDS(w, p) ((p / CHUNK_SIZE < 0 ? p + 1 : p) / CHUNK_SIZE + ((w)->size.x) / 2 - (p < 0 ? 1 : 0))
#define GET_CURRENT_HOTBAR(w) (((w)->selected_block - 1) / 9)

typedef struct
{
    vec3 position;
    vec3 smoothed_position;
    vec3 prev_position;
    unsigned char id;
    char nickname[31];
} network_player;

struct World
{
    ivec2 size;

    struct Chunk *chunks;
    int generating_chunks;

    float window_width;
    float window_height;

    network_player players[MAX_PLAYERS];
    unsigned char num_players;
    entity player;
    int fly_mode;
    int noclip_mode;

    uint64_t seed;

    vec3 camera_position;
    vec2 camera_rotation;
    int destroying_block;
    int placing_block;
    block_id selected_block;
    int selected_block_x;
    int selected_block_y;
    int selected_block_z;
    int selected_face_x;
    int selected_face_y;
    int selected_face_z;
    int block_in_range;

    int block_changed;
    block_id new_block;

    block_vertex *chunk_data_buffer;

    mat4 blocks_model;
    mat4 world_view;
    mat4 world_projection;

    GLuint blocks_texture;
    shader blocks_shader;

    shader lines_shader;
    
    GLuint frame_vao;
    GLuint frame_vbo;
};

void world_init(struct World *w);
void world_generate(struct World *w);
void world_handle_input(struct World *w, input *i);
void world_tick(struct World *w);
void world_draw(struct World *w, double delta_time, double time_since_tick);
void world_destroy(struct World *w);

block_id world_get_block(struct World *w, int x, int y, int z);
void world_set_block(struct World *w, int x, int y, int z, block_id new_block);

struct Chunk* world_get_chunk(struct World *w, int x, int z);

#endif