#ifndef VECTOR_H
#define VECTOR_H

typedef struct {
    float x;
    float y;
} vec2_t;

typedef struct {
    float x;
    float y;
    float z;
} vec3_t;

// TODO: Add functions to manipulate vectors 2D and 3D
vec3_t rotatex(vec3_t v, float angle);
vec3_t rotatey(vec3_t v, float angle);
vec3_t rotatez(vec3_t v, float angle);

#endif // !VECTOR_H
#define VECTOR_HVECTOR_H
