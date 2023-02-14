#ifndef SKY_H
#define SKY_H

#include "mvmath.h"
#include <glad/glad.h>

struct Sky {
    vec3 curr_color;
    int ticks;
};

void sky_init(struct Sky *sky);
void sky_tick(struct Sky *sky);
void sky_render(struct Sky *sky);

#endif