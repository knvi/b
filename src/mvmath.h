#ifndef MVMATH_H
#define MVMATH_H

#define PI 3.14159265359f
#define DEGREES(radians) ((radians) * 180.0f / PI)
#define RADIANS(degrees) ((degrees) * PI / 180.0f)

typedef struct
{
    float value[16];
} mat4;

typedef struct
{
    float x;
    float y;
    float z;
} vec3;

typedef struct
{
    int x;
    int y;
    int z;
} ivec3;

typedef struct
{
    float x;
    union {
        float y;
        float z;
    };
} vec2;

typedef struct
{
    int x;
    union {
        int y;
        int z;
    };
} ivec2;

#define VEC3S2V(v) (vec3){v.x, v.y, v.z}
#define VEC2S2V(v) (vec2){v.x, v.y}
#define VEC3I2V(v) (ivec3){v.x, v.y, v.z}
#define VEC2I2V(v) (ivec2){v.x, v.y}

static vec3 AXIS_UP = {0.0f, 1.0f, 0.0f};
static vec3 AXIS_RIGHT = {1.0f, 0.0f, 0.0f};
static vec3 AXIS_FRONT = {0.0f, 0.0f, 1.0f};
static vec3 VEC3_ONE = {1.0f, 1.0f, 1.0f};

static mat4 TEMP_MAT;

void normalize(vec3 *v);

void identity(mat4 *m);
void translate(mat4 *m, vec3 *v);
void translate_v2(mat4 *m, vec2 *v);
void rotate(mat4 *m, vec3 *axis, float angle);

void add_v3(vec3 *v, vec3 *v1, vec3 *v2);
void add_v2(vec2 *v, vec2 *v1, vec2 *v2);

void subtract_v2(vec2 *v, vec2 *v1, vec2 *v2);
vec2 subtract_v2r(vec2 v1, vec2 v2);
vec3 subtract_v3(vec3 v1, vec3 v2);
ivec3 subtract_v3i(ivec3 v1, ivec3 v2);

float vec3_norm(vec3 v);
float vec2_norm(vec2 v);

void multiply(mat4 *m, mat4 *m1, mat4 *m2);
void scale(mat4 *m, vec3* v);
void multiply_v3f(vec3 *v, vec3 *v1, float v2);
void multiply_v2f(vec2 *v, vec2 *v1, float v2);

void frustum(mat4 *m, float left, float right, float bottom, float top, float znear, float zfar);
void perspective(mat4 *m, float fov, float aspect, float znear, float zfar);
void ortho(mat4 *m, float left, float right, float bottom, float top, float near, float far);

float lerp(float a, float b, float t);
void v3_lerp(vec3 *v, vec3 *a, vec3 *b, float t);

#define max(a, b) ((a) > (b) ? (a) : (b))

#endif