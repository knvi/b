#include "bounding_box.h"

int bounding_box_update(bounding_box *box, vec3 *pos) {
    box->min.x = pos->x - box->size.x * 0.5f;
    box->min.y = pos->y;
    box->min.z = pos->z - box->size.z * 0.5f;
    box->max.x = pos->x + box->size.x * 0.5f;
    box->max.y = pos->y + box->size.y;
    box->max.z = pos->z + box->size.z * 0.5f;
}

int is_colliding(bounding_box *b1, bounding_box *b2)
{
    return b1->max.x > b2->min.x && b1->max.z > b2->min.z && b1->max.y > b2->min.y
        && b1->min.x < b2->max.x && b1->min.z < b2->max.z && b1->min.y < b2->max.y;
}

int is_touching(bounding_box *b1, bounding_box *b2) {
    return b1->max.x >= b2->min.x && b1->max.z >= b2->min.z && b1->max.y >= b2->min.y
        && b1->min.x <= b2->max.x && b1->min.z <= b2->max.z && b1->min.y <= b2->max.y;
}