#include <math.h>
#include "vector.h"

// vector 2D functions
float vec2_length(vec2_t v) {
  return sqrtf(v.x * v.x + v.y * v.y);
}

vec2_t vec2_add(vec2_t a, vec2_t b) {
  vec2_t result = { a.x + b.x, a.y + b.y };
  return result;
}

vec2_t vec2_sub(vec2_t a, vec2_t b) {
  vec2_t result = { a.x - b.x, a.y - b.y };
  return result;
}

vec2_t vec2_mul(vec2_t v, float s) {
  vec2_t result = { v.x * s, v.y * s };
  return result;
}

vec2_t vec2_div(vec2_t v, float s) {
  vec2_t result = { v.x / s, v.y / s };
  return result;
}

float vec2_dot(vec2_t a, vec2_t b) {
  return a.x * b.x + a.y * b.y;
}

void vec2_normalize(vec2_t *v) {
  float length = {v->x * v->x + v->y * v->y};
  v->x /= length;
  v->y /= length;
}

// vector 3D functions
float vec3_length(vec3_t v) {
  return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

vec3_t vec3_add(vec3_t a, vec3_t b) {
  vec3_t result = { a.x + b.x, a.y + b.y, a.z + b.z };
  return result;
}

vec3_t vec3_sub(vec3_t a, vec3_t b) {
  vec3_t result = { a.x - b.x, a.y - b.y, a.z - b.z };
  return result;
}

vec3_t vec3_mul(vec3_t v, float s) {
  vec3_t result = { v.x * s, v.y * s, v.z * s };
  return result;
}

vec3_t vec3_div(vec3_t v, float s) {
  vec3_t result = { v.x / s, v.y / s, v.z / s };
  return result;
}

vec3_t vec3_cross(vec3_t a, vec3_t b) {
  vec3_t result = {
    a.y * b.z - a.z * b.y,
    a.z * b.x - a.x * b.z,
    a.x * b.y - a.y * b.x
  };
  return result;
}

float vec3_dot(vec3_t a, vec3_t b) {
  return a.x * b.x + a.y * b.y + a.z * b.z;
}

void vec3_normalize(vec3_t* v) {
  float length = sqrt(v->x * v->x + v->y * v->y + v->z * v->z);
  v->x /= length;
  v->y /= length;
  v->z /= length;
}

vec3_t rotatex(vec3_t v, float angle) {
  vec3_t rotated_vector = {
    .x = v.x, 
    .y = v.y * cos(angle) - v.z * sin(angle), 
    .z = v.y * sin(angle) + v.z * cos(angle)
  };
  
  return rotated_vector;
}

vec3_t rotatey(vec3_t v, float angle) {
  vec3_t rotated_vector = {
    .x = v.x * cos(angle) - v.z * sin(angle), 
    .y = v.y, 
    .z = v.x * sin(angle) + v.z * cos(angle)
  };
  
  return rotated_vector;
}

vec3_t rotatez(vec3_t v, float angle) {
  vec3_t rotated_vector = {
    .x = v.x * cos(angle) - v.y * sin(angle), 
    .y = v.x * sin(angle) + v.y * cos(angle), 
    .z = v.z
  };
 
  return rotated_vector;
}

// vector 4D functions
vec4_t vec4_from_vec3(vec3_t v) {
  vec4_t result = { v.x, v.y, v.z, 1.0 };
  return result;
}

vec3_t vec3_from_vec4(vec4_t v) {
  vec3_t result = { v.x, v.y, v.z };
  return result;
}
