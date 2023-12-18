#include "display.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
uint32_t *colour_buffer = NULL;
SDL_Texture *colour_buffer_texture = NULL;
int window_width = 800;
int window_height = 600;

bool initialize_window(void) {
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    fprintf(stderr, "Error initializing SDL.\n");
    return false;
  }

  // Use SDL to query full screen dimensions
  SDL_DisplayMode display_mode;
  SDL_GetCurrentDisplayMode(0, &display_mode);
  window_width = display_mode.w;
  window_height = display_mode.h;

  window =
      SDL_CreateWindow(NULL, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       window_width, window_height, SDL_WINDOW_BORDERLESS);

  if (!window) {
    fprintf(stderr, "Error creating SDL window.\n");
    return false;
  }

  renderer = SDL_CreateRenderer(window, -1 /* Don't care; get default */, 0);

  if (!renderer) {
    fprintf(stderr, "Error creating SDL window.\n");
    return false;
  }

  SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

  return true;
}

void render_colour_buffer(void) {
  SDL_UpdateTexture(colour_buffer_texture, NULL, colour_buffer,
                    window_width * sizeof(u_int32_t));

  SDL_RenderCopy(renderer, colour_buffer_texture, NULL, NULL);
}

void draw_pixel(int x, int y, u_int32_t colour) {
  if (x >= 0 && x < window_width && y >= 0 && y < window_height) {
    colour_buffer[(window_width * y) + x] = colour;
  }
}

void draw_grid(void) {
  for (int y = 0; y < window_height; y += 10) {
    for (int x = 0; x < window_width; x += 10) {
      colour_buffer[(window_width * y) + x] = 0xFF444444;
    }
  }
}

void draw_rect(int x, int y, int width, int height, u_int32_t colour) {
  for (int ypos = y; ypos < (y + height); ypos++) {
    for (int xpos = x; xpos < (x + width); xpos++) {
      draw_pixel(xpos, ypos, colour);
    }
  }
}

void draw_line(int x0, int y0, int x1, int y1, uint32_t color) {
  // Calculate delta of each dimension
  int delta_x = (x1 - x0);
  int delta_y = (y1 - y0);

  int longest_side =
      (abs(delta_x) >= abs(delta_y)) ? abs(delta_x) : abs(delta_y);

  // The largest dimension simply increments by 1 but this
  // could obviously be x or y. Dividing either delta by `longest_side`
  // will yield one of the variables as 1 with the other some fraction
  float inc_x = delta_x / (float)longest_side;
  float inc_y = delta_y / (float)longest_side;

  float current_x = x0;
  float current_y = y0;

  for (int step = 0; step < longest_side; step++) {
    draw_pixel(round(current_x), round(current_y), color);
    current_x += inc_x;
    current_y += inc_y;
  }
}

void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2,
                   u_int32_t colour) {
  draw_line(x0, y0, x1, y1, colour);
  draw_line(x1, y1, x2, y2, colour);
  draw_line(x2, y2, x0, y0, colour);
}

void clear_colour_buffer(u_int32_t colour) {
  for (int y = 0; y < window_height; y++) {
    for (int x = 0; x < window_width; x++) {
      colour_buffer[(window_width * y) + x] = colour;
    }
  }
}

void destroy_window(void) {
  free(colour_buffer);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}
