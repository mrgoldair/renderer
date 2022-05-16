#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "display.h"

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
uint32_t *colour_buffer = NULL;
SDL_Texture *colour_buffer_texture = NULL;
int window_width = 800;
int window_height = 600;

bool initialize_window(void)
{
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
  {
    fprintf(stderr, "Error initializing SDL.\n");
    return false;
  }

  // Use SDL to query full screen dimensions
  SDL_DisplayMode display_mode;
  SDL_GetCurrentDisplayMode(0, &display_mode);
  window_width = display_mode.w;
  window_height = display_mode.h;

  window = SDL_CreateWindow(
      NULL,
      SDL_WINDOWPOS_CENTERED,
      SDL_WINDOWPOS_CENTERED,
      window_width,
      window_height,
      SDL_WINDOW_BORDERLESS);

  if (!window)
  {
    fprintf(stderr, "Error creating SDL window.\n");
    return false;
  }

  renderer = SDL_CreateRenderer(window, -1 /* Don't care; get default */, 0);

  if (!renderer)
  {
    fprintf(stderr, "Error creating SDL window.\n");
    return false;
  }

  SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

  return true;
}

void render_colour_buffer(void)
{
  SDL_UpdateTexture(
      colour_buffer_texture,
      NULL,
      colour_buffer,
      window_width * sizeof(u_int32_t));

  SDL_RenderCopy(
      renderer,
      colour_buffer_texture,
      NULL,
      NULL);
}

void draw_pixel(int x, int y, u_int32_t colour)
{
  if (x >= 0 && x < window_width && y >= 0 && y < window_height)
  {
    colour_buffer[(window_width * y) + x] = colour;
  }
}

void draw_grid(void)
{
  // Draw horizontal grid lines
  for (int y = 0; y < window_height; y++)
  {
    for (int x = 0; x < window_width; x++)
    {
      if (x % 10 == 0 || y % 10 == 0)
      {
        colour_buffer[(window_width * y) + x] = 0xFF000000;
      }
    }
  }
}

void draw_rect(int x, int y, int width, int height, u_int32_t colour)
{
  for (int ypos = y; ypos < (y + height); ypos++)
  {
    for (int xpos = x; xpos < (x + width); xpos++)
    {
      draw_pixel(xpos, ypos, colour);
    }
  }
}

void clear_colour_buffer(u_int32_t colour)
{
  for (int y = 0; y < window_height; y++)
  {
    for (int x = 0; x < window_width; x++)
    {
      colour_buffer[(window_width * y) + x] = colour;
    }
  }
}

void destroy_window(void)
{
  free(colour_buffer);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}