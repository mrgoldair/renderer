#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "display.h"
#include "vector.h"

const int N_POINTS = 9 * 9 * 9;
vec3_t cube_points[N_POINTS];
vec2_t projected_points[N_POINTS];

float fov_factor = 128;

bool is_running = false;

void setup(void)
{
  // Allocate the required memory in bytes to hold colour buffer
  colour_buffer = malloc(window_width * window_height * sizeof(u_int32_t));

  // Creating SDL texture that is used to display
  colour_buffer_texture = SDL_CreateTexture(
      renderer,
      SDL_PIXELFORMAT_ARGB8888,
      SDL_TEXTUREACCESS_STREAMING,
      window_width,
      window_height);

  int point_count = 0;
  for (float x = -1; x <= 1; x += 0.25)
  {
    for (float y = -1; y <= 1; y += 0.25)
    {
      for (float z = -1; z <= 1; z += 0.25)
      {
        vec3_t new_point = {x,
                            y,
                            z};

        cube_points[point_count++] = new_point;
      }
    }
  }
}

void process_input(void)
{
  SDL_Event event;
  SDL_PollEvent(&event);

  switch (event.type)
  {
  case SDL_QUIT:
    is_running = false;
    break;
  case SDL_KEYDOWN:
    if (event.key.keysym.sym == SDLK_ESCAPE)
      is_running = false;
    break;
  }
}

vec2_t project(vec3_t point)
{
  vec2_t projected = {
      .x = (fov_factor * point.x),
      .y = (fov_factor * point.y)};

  return projected;
}

void update(void)
{
  for (int i = 0; i < N_POINTS; i++)
  {
    vec3_t point = cube_points[i];

    vec2_t projected = project(point);

    projected_points[i] = projected;
  }
}

void render(void)
{
  // Loop all projected and render
  for (int i = 0; i < N_POINTS; i++)
  {
    vec2_t projected_point = projected_points[i];
    draw_rect(
        projected_point.x + (window_width / 2),
        projected_point.y + (window_height / 2),
        4,
        4,
        0xFFFFFF00);
  }

  // colour buffer -> texture
  // texture -> renderer
  render_colour_buffer();
  clear_colour_buffer(0xFF000000);
  SDL_RenderPresent(renderer);
}

int main(void)
{

  is_running = initialize_window();

  setup();

  while (is_running)
  {
    process_input();
    update();
    render();
  }

  destroy_window();

  return true;
}
