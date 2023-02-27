#include <math.h>
#include "vector.h"

// TODO: Implementation of all vector functions
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
