#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#define FPS 200
#define FRAME_TARGET_TIME (1000/FPS)

extern SDL_Window *window;
extern SDL_Renderer *renderer;
extern uint32_t *colour_buffer;
extern SDL_Texture *colour_buffer_texture;
extern int window_width;
extern int window_height;

bool initialize_window(void);

void draw_pixel(int x, int y, u_int32_t colour);
void draw_line(int x0, int y0, int x1, int y1, u_int32_t colour);
void draw_rect(int x, int y, int width, int height, u_int32_t colour);
void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, u_int32_t colour);
void draw_grid(void);

void render_colour_buffer(void);
void clear_colour_buffer(u_int32_t colour);
void destroy_window(void);

#endif