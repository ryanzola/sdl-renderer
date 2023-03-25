#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL.h>
#include "array.h"
#include "display.h"
#include "vector.h"
#include "mesh.h"
#include "matrix.h"

triangle_t* triangles_to_render = NULL;


vec3_t camera_position = {0, 0, 0};
mat4_t proj_matrix;

bool is_running = false;
int previous_frame_time = 0;

void setup(void) {
  render_method = RENDER_WIRE;
  cull_method = CULL_BACKFACE;

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

  // initialize the projection matrix
  float fov = M_PI / 3.0f; // 60 degrees
  float aspect = (float)window_height / (float)window_width;
  float znear = 0.1;
  float zfar = 100.0;
  proj_matrix = mat4_make_perspective(fov, aspect, znear, zfar);

  load_cube_mesh_data();
  //load_obj_file_data("./assets/cube.obj");
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
      if(event.key.keysym.sym == SDLK_1)
        render_method = RENDER_WIRE_VERTEX;
      if(event.key.keysym.sym == SDLK_2)
        render_method = RENDER_WIRE;
      if(event.key.keysym.sym == SDLK_3)
        render_method = RENDER_FILL_TRIANGLE;
      if(event.key.keysym.sym == SDLK_4)
        render_method = RENDER_FILL_TRIANGLE_WIRE;
      if(event.key.keysym.sym == SDLK_c)
        cull_method = CULL_BACKFACE;
      if(event.key.keysym.sym == SDLK_d)
        cull_method = CULL_NONE;
      break;
  }
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

  // change the mesh rotation and scale per animation frame
  mesh.rotation.x += 0.02;
  // mesh.rotation.y += 0.02;
  // mesh.rotation.z += 0.02;

  // mesh.scale.x += 0.002;
  //mesh.scale.y += 0.002;
  //mesh.scale.z += 0.002;

  // mesh.translation.x += 0.01;
  mesh.translation.z = 5.0;

  // create a scale matrix that will be used to multiply the vertices of the mesh
  mat4_t scale_matrix = mat4_make_scale(mesh.scale.x, mesh.scale.y, mesh.scale.z);
  mat4_t translation_matrix = mat4_make_translation(mesh.translation.x, mesh.translation.y, mesh.translation.z);
  mat4_t rotation_matrix_x = mat4_make_rotation_x(mesh.rotation.x);
  mat4_t rotation_matrix_y = mat4_make_rotation_y(mesh.rotation.y);
  mat4_t rotation_matrix_z = mat4_make_rotation_z(mesh.rotation.z);

  // loop over all faces
  int num_faces = array_length(mesh.faces);
  for(int i = 0; i < num_faces; i++) {
    face_t mesh_face = mesh.faces[i];

    vec3_t face_vertices[3];
    face_vertices[0] = mesh.vertices[mesh_face.a - 1];
    face_vertices[1] = mesh.vertices[mesh_face.b - 1];
    face_vertices[2] = mesh.vertices[mesh_face.c - 1];

    vec4_t transformed_vertices[3];

    // loop over all vertices of the current face and apply transformations
    for(int j = 0; j < 3; j++) {
      vec4_t transformed_vertex = vec4_from_vec3(face_vertices[j]);

      // use a matrix to transform vertices
      // order matters: scale, rotate, translate

      // create a world matrix to combine scale, rotation, and translation
      mat4_t world_matrix = mat4_identity();
      world_matrix = mat4_mul_mat4(scale_matrix, world_matrix);
      world_matrix = mat4_mul_mat4(rotation_matrix_x, world_matrix);
      world_matrix = mat4_mul_mat4(rotation_matrix_y, world_matrix);
      world_matrix = mat4_mul_mat4(rotation_matrix_z, world_matrix);
      world_matrix = mat4_mul_mat4(translation_matrix, world_matrix);
      

      // multiply the world matrix by the original vector
      transformed_vertex = mat4_mul_vec4(world_matrix, transformed_vertex);

      // store the transformed vertex
      transformed_vertices[j] = transformed_vertex;
    }
   
    // backface culling test to see if the current face should be projected
    if(cull_method == CULL_BACKFACE) {
      vec3_t vector_a = vec3_from_vec4(transformed_vertices[0]); /*   A   */
      vec3_t vector_b = vec3_from_vec4(transformed_vertices[1]); /*  / \  */
      vec3_t vector_c = vec3_from_vec4(transformed_vertices[2]); /* B---C */

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
    
    vec4_t projected_points[3];

    for(int j = 0; j < 3; j++) {
      projected_points[j] = mat4_mul_vec4_project(proj_matrix, transformed_vertices[j]);

      // scale the projected point
      projected_points[j].x *= window_width / 2.0;
      projected_points[j].y *= window_height / 2.0;
      
      // transalate the projected point
      projected_points[j].x += window_width / 2.0;
      projected_points[j].y += window_height / 2.0;
    }

    // calculate the average depth of the face after translation
    // this is used to sort the faces in the render queue
    // so that the faces that are furthest away are rendered first
    // and the faces that are closest are rendered last
    // this is done to ensure that the faces that are closest to the camera
    // are rendered on top of the faces that are further away
    // this is called painter's algorithm
    // https://en.wikipedia.org/wiki/Painter%27s_algorithm
    // https://www.youtube.com/watch?v=ZBdE8DElQQU
    float avg_depth = (transformed_vertices[0].z + transformed_vertices[1].z + transformed_vertices[2].z) / 3.0;

    triangle_t projected_triangle = {
      .points = {
        {projected_points[0].x, projected_points[0].y},
        {projected_points[1].x, projected_points[1].y},
        {projected_points[2].x, projected_points[2].y}
      },
      .color = mesh_face.color,
      .avg_depth = avg_depth
    };

    // triangles_to_render[i] = projected_triangle;
    array_push(triangles_to_render, projected_triangle);

    // sort the triangles in the render queue by their average depth
    // this is done to ensure that the faces that are closest to the camera
    // are rendered on top of the faces that are further away
    // this is called painter's algorithm
    int num_triangles = array_length(triangles_to_render);
    for(int i = 0; i < num_triangles; i++) {
      for(int j = i; j < num_triangles; j++) {
        if(triangles_to_render[i].avg_depth < triangles_to_render[j].avg_depth) {
          triangle_t temp = triangles_to_render[i];
          triangles_to_render[i] = triangles_to_render[j];
          triangles_to_render[j] = temp;
        }
      }
    }
  }
}

void render(void) {
  //  draw_grid();

  // loop through all the points and draw them
  int num_triangles = array_length(triangles_to_render);
  for(int i = 0; i < num_triangles; i++) {
    triangle_t triangle = triangles_to_render[i];

    // draw filled triangle
    if(render_method == RENDER_FILL_TRIANGLE || render_method == RENDER_FILL_TRIANGLE_WIRE) {
      draw_filled_triangle(
        triangle.points[0].x, triangle.points[0].y, // vertex A
        triangle.points[1].x, triangle.points[1].y, // vertex B
        triangle.points[2].x, triangle.points[2].y, // vertex C
        triangle.color
      );
    }

    // draw triangle wireframe
    if(render_method == RENDER_WIRE || render_method == RENDER_WIRE_VERTEX || render_method == RENDER_FILL_TRIANGLE_WIRE) {
      draw_triangle(
        triangle.points[0].x, triangle.points[0].y, // vertex A
        triangle.points[1].x, triangle.points[1].y, // vertex B
        triangle.points[2].x, triangle.points[2].y, // vertex C
        0xFFFFFFFF
      );
    }

    // draw triangle vertices
    if(render_method == RENDER_WIRE_VERTEX) {
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
