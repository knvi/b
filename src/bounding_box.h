#ifndef BOUNDING_BOX_H
#define BOUNDING_BOX_H

#include "mvmath.h"

typedef struct {
    vec3 size;
    vec3 min;
    vec3 max;
} bounding_box;

int bounding_box_update(bounding_box *box, vec3 *position);
int is_colliding(bounding_box *b1, bounding_box *b2);
int is_touching(bounding_box *b1, bounding_box *b2);

#endif // BOUNDING_BOX_H