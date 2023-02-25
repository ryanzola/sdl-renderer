#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL.h>

bool is_running = false;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* color_buffer_texture = NULL;

uint32_t* color_buffer = NULL;

int window_width = 800;
int window_height = 600;

bool initialize_window(void) {
  if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    fprintf(stderr, "Error initializing SDL:\n");
    return false;
  }

  // use SDL to query what is the fullscreen width and height
  SDL_DisplayMode display_mode;
  SDL_GetCurrentDisplayMode(0, &display_mode);

  window_width = display_mode.w;
  window_height = display_mode.h;

  // create an SDL window
  window = SDL_CreateWindow(
      "My Window",
      SDL_WINDOWPOS_CENTERED,
      SDL_WINDOWPOS_CENTERED,
      window_width,
      window_height,
      SDL_WINDOW_BORDERLESS
    );

  if(!window) {
    fprintf(stderr, "Error creating SDL window:\n");
    return false;
  }

  // create an SDL renderer
  renderer = SDL_CreateRenderer(window, -1, 0);

  if(!renderer) {
    fprintf(stderr, "Error creating SDL renderer:\n");
    return false;
  }

  SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

  return true;
}

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

void render_color_buffer(void) {
  SDL_UpdateTexture(
      color_buffer_texture,
      NULL,
      color_buffer,
      (int)(window_width * sizeof(uint32_t))
    );

  SDL_RenderCopy(
      renderer,
      color_buffer_texture,
      NULL,
      NULL
    );
}

void clear_color_buffer(uint32_t color) {
  for(int y = 0; y < window_height; y++) {
    for(int x = 0; x < window_width; x++) {
      color_buffer[(window_width * y) + x] = color;
    }
  }
}

void draw_grid() {
  for(int y = 0; y < window_height; y += 10) {
    for(int x = 0; x < window_width; x += 10) {
      color_buffer[(window_width * y) + x] = 0xFFFF0000;
    }
  }
}

void draw_rect(int x, int y, int width, int height, uint32_t color) {
  for(int i = 0; i < width; i++) {
    for(int j = 0; j < height; j++) {
      int current_x = x + i;
      int current_y = y + j;
      color_buffer[(window_width * current_y) + current_x] = color;
    }
  }
}

void render() {
  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
  SDL_RenderClear(renderer);

  draw_grid();

  draw_rect(800, 100, 300, 300, 0xFFFF0055);

  // render game objects
  render_color_buffer();
  
  clear_color_buffer(0x00000000);

  SDL_RenderPresent(renderer);
}

void destroy_window(void) {
  free(color_buffer);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
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
