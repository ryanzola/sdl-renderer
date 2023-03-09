#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL.h>
#include "array.h"
#include "display.h"
#include "vector.h"
#include "mesh.h"

triangle_t* triangles_to_render = NULL;


vec3_t camera_position = {0, 0, 0};

float fov_factor = 640;

bool is_running = false;
bool show_wireframe = true;
bool show_faces = true;
bool show_vertices = false;
bool culled = true;

int previous_frame_time = 0;

void setup(void) {
  // allocate the required memory in bytes to hold the color buffer
  color_buffer = (uint32_t*)malloc(sizeof(uint32_t) * window_width * window_height);

  // create an SDL texture that is used to disply the color buffer
  color_buffer_texture = SDL_CreateTexture(
    renderer,
    SDL_PIXELFORMAT_ARGB8888,
    SDL_TEXTUREACCESS_STREAMING,
    window_width,
    window_height
  );

  //load_cube_mesh_data();
  load_obj_file_data("./assets/cube.obj");
}

void process_input(void) {
  SDL_Event event;
  SDL_PollEvent(&event);

  switch(event.type) {
    case SDL_QUIT:
      is_running = false;
      break;
    case SDL_KEYDOWN:
      if(event.key.keysym.sym == SDLK_ESCAPE)
        is_running = false;
      // press the 1 key to show the wireframe and vertices
      if(event.key.keysym.sym == SDLK_1) {
        show_wireframe = true;
        show_vertices = true;
        show_faces = false;
      }

      // press the 2 key to show the only the wireframes
      if(event.key.keysym.sym == SDLK_2) {
        show_faces = false;
        show_wireframe = true;
        show_vertices = false;
      }

      // press the 3 key to show the only the faces 
      if(event.key.keysym.sym == SDLK_3) {
        show_faces = true;
        show_wireframe = false;
        show_vertices = false;
      }

      // press the 4 key to show the faces and wireframes
      if(event.key.keysym.sym == SDLK_4) {
        show_faces = true;
        show_wireframe = true;
        show_vertices = false;
      }

      // press the c key to enable backface culling
      if(event.key.keysym.sym == SDLK_c) {
        culled = true;
      }

      // press the d key to disable backface culling
      if(event.key.keysym.sym == SDLK_d) {
        culled = false;
      }
      break;
  }
}

vec2_t project(vec3_t point) {
  vec2_t projected_point = { 
    .x = (fov_factor * point.x) / point.z,
    .y = (fov_factor * point.y) / point.z
  };

  return projected_point;
}

void update(void) {
  // while(!SDL_TICKS_PASSED(SDL_GetTicks(), previous_frame_time + FRAME_TARGET_TIME));
  // replace above with SDL_Delay
  int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);

  if(time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME) {
    SDL_Delay(time_to_wait);
  }

  previous_frame_time = SDL_GetTicks();

  // initialize the array of triangles to render
  triangles_to_render = NULL;

  // cube rotation y increases by 0.01
  mesh.rotation.x += 0.01;
  mesh.rotation.y += 0.01;

  // loop over all faces
  int num_faces = array_length(mesh.faces);
  for(int i = 0; i < num_faces; i++) {
    face_t mesh_face = mesh.faces[i];

    vec3_t face_vertices[3];
    face_vertices[0] = mesh.vertices[mesh_face.a - 1];
    face_vertices[1] = mesh.vertices[mesh_face.b - 1];
    face_vertices[2] = mesh.vertices[mesh_face.c - 1];

    vec3_t transformed_vertices[3];

    // loop over all vertices of the current face and apply transformations
    for(int j = 0; j < 3; j++) {
      vec3_t transformed_vertex = face_vertices[j];

      transformed_vertex = rotatex(transformed_vertex, mesh.rotation.x);
      transformed_vertex = rotatey(transformed_vertex, mesh.rotation.y);
      transformed_vertex = rotatez(transformed_vertex, mesh.rotation.z);

      // translate the vertex away from the camera on z axis
      transformed_vertex.z += 5.0;

      // store the transformed vertex
      transformed_vertices[j] = transformed_vertex;
    }
    
    if(culled) {
      vec3_t vector_a = transformed_vertices[0]; /*   A   */
      vec3_t vector_b = transformed_vertices[1]; /*  / \  */
      vec3_t vector_c = transformed_vertices[2]; /* B---C */

      vec3_t vector_ab = vec3_sub(vector_b, vector_a);
      vec3_t vector_ac = vec3_sub(vector_c, vector_a);
      vec3_normalize(&vector_ab);
      vec3_normalize(&vector_ac);

      // compute the face normal (using cross product to find perpendicular)
      vec3_t normal = vec3_cross(vector_ab, vector_ac);

      // normalize the face normal vector
      vec3_normalize(&normal);

      // find the vector between a point in the triangle and the camera
      vec3_t camera_ray = vec3_sub(camera_position, vector_a);

      // calculate how aligned the camera is with the face normal using the dot product
      float dot_normal_camera = vec3_dot(normal, camera_ray);

      // if the dot product is negative, the face is facing away from the camera
      if(dot_normal_camera < 0) {
        continue;
      }
    }
    
    triangle_t projected_triangle;

    for(int j = 0; j < 3; j++) {
      vec2_t projected_point = project(transformed_vertices[j]);

      // scale and transalate the projected point
      projected_point.x += window_width / 2;
      projected_point.y += window_height / 2;

      projected_triangle.points[j] = projected_point;
    }

    // triangles_to_render[i] = projected_triangle;
    array_push(triangles_to_render, projected_triangle);
  }
}

void render(void) {
  //  draw_grid();

  // loop through all the points and draw them
  int num_triangles = array_length(triangles_to_render);
  for(int i = 0; i < num_triangles; i++) {
    triangle_t triangle = triangles_to_render[i];

    if(show_faces) {
      draw_filled_triangle(
        triangle.points[0].x, triangle.points[0].y, // vertex A
        triangle.points[1].x, triangle.points[1].y, // vertex B
        triangle.points[2].x, triangle.points[2].y, // vertex C
        0xFFFFFFFF
      );
    }

    if(show_wireframe) {
      draw_triangle(
        triangle.points[0].x, triangle.points[0].y, // vertex A
        triangle.points[1].x, triangle.points[1].y, // vertex B
        triangle.points[2].x, triangle.points[2].y, // vertex C
        0xFFFF0000
      );
    }

    if(show_vertices) {
      for(int j = 0; j < 3; j++) {
        draw_rect(triangle.points[j].x - 3, triangle.points[j].y - 3, 6, 6, 0xFFFF0000);
      }
    }
  }

  // draw_filled_triangle(300, 100, 50, 400, 500, 700, 0xFF00FF00);

  // clear the array of triangles to render every frame loop
  array_free(triangles_to_render);
  

  // render game objects
  render_color_buffer();
  
  clear_color_buffer(0x00000000);

  SDL_RenderPresent(renderer);
}

void free_resources(void) {
  array_free(mesh.vertices);
  array_free(mesh.faces);
  free(color_buffer);
}

int main(void) {
  /* create an SDL window */
  is_running = initialize_window();

  setup();

  /* main game loop */
  while(is_running) {
    process_input();
    update();
    render();
  }

  destroy_window();
  free_resources();

  return 0;
}
