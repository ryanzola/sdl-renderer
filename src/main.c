#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL.h>
#include "display.h"

bool is_running = false;

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
      break;
  }
}

void update(void) {
  // update game state
}

void render(void) {
  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
  SDL_RenderClear(renderer);

  draw_grid();

  draw_rect(800, 100, 300, 300, 0xFFFF0055);

  draw_pixel(100, 100, 0xFF00FF00);

  // render game objects
  render_color_buffer();
  
  clear_color_buffer(0x00000000);

  SDL_RenderPresent(renderer);
}

int main() {
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

  return 0;
}
