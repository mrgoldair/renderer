#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "array.h"
#include "display.h"
#include "vector.h"
#include "mesh.h"
#include "triangle.h"

triangle_t* triangles_to_render = NULL;

vec3_t camera_position = { .x = 0, .y = 0, .z = -5 };
vec3_t cube_rotation = { .x = 0, .y = 0, .z = 0 };
float fov_factor = 640;

bool is_running = false;
int previous_frame_time = 0;

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
      .x = (fov_factor * point.x) / point.z,
      .y = (fov_factor * point.y) / point.z
    };

  return projected;
}

/**
 * @brief We keep track of the change in motion through
 * `cube_rotation.x/y/z` etc and these accumulated changes
 * are applied to the "identity" mesh; the effective change
 * in mesh verts are never persisted frame-to-frame but the changes
 * are applied to the original mesh.
 */
void update(void)
{
  //
  int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);

  if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME){
    SDL_Delay(time_to_wait);
  }

  previous_frame_time = SDL_GetTicks();

  // Every frame init the triangles to render
  triangles_to_render = NULL;

  // Rotation in radians
  cube_rotation.z += 0.001;
  cube_rotation.y += 0.001;
  cube_rotation.z += 0.001;

  for (int i = 0; i < N_MESH_FACES; i++){
    face_t mesh_face = mesh_faces[i];
    
    vec3_t face_vertices[3];
    // Perform lookup – mesh_face stores an _index_ of a vertex within mesh_vertices
    // .a - 1 because the faces are not 0th indexed
    face_vertices[0] = mesh_vertices[mesh_face.a - 1];
    face_vertices[1] = mesh_vertices[mesh_face.b - 1];
    face_vertices[2] = mesh_vertices[mesh_face.c - 1];

    triangle_t projected_triangle;

    // Transform and project each face's vertices
    for (int j = 0; j < 3; j++){
      vec3_t transformed_vertex = face_vertices[j];
      // NOTE: Accumulate the transformation across all 3 axis
      transformed_vertex = vec3_rotate_x(transformed_vertex, cube_rotation.x);
      transformed_vertex = vec3_rotate_y(transformed_vertex, cube_rotation.y);
      transformed_vertex = vec3_rotate_z(transformed_vertex, cube_rotation.z);
      // Simulate moving the camera back by subtracting z-pos of camera from every point 
      // NOTE: right-hand rule states that -ve z index goes into _into_
      // the screen hence we subtract the camera position in order to push the point away
      transformed_vertex.z -= camera_position.z;

      // A projected vertex is a point
      vec2_t projected_point = project(transformed_vertex);
      
      projected_point.x += (window_width / 2);
      projected_point.y += (window_height / 2);
      // When all points of a face are transformed and projected we have a triangle ready to render
      projected_triangle.points[j] = projected_point;
    }

    // Add projected triangle to list of triangles ready to render
    array_push(triangles_to_render, projected_triangle);
  }
}

void render(void)
{
  draw_grid();

  int num_mesh_triangles = array_length(triangles_to_render);

  // // Loop all projected and render
  for (int i = 0; i < num_mesh_triangles; i++)
  {
    triangle_t triangle = triangles_to_render[i];
    draw_rect(triangle.points[0].x, triangle.points[0].y, 3, 3, 0xFFFFFF00);
    draw_rect(triangle.points[1].x, triangle.points[1].y, 3, 3, 0xFFFFFF00);
    draw_rect(triangle.points[2].x, triangle.points[2].y, 3, 3, 0xFFFFFF00);

    draw_triangle(
      triangle.points[0].x,
      triangle.points[0].y,
      triangle.points[1].x,
      triangle.points[1].y,
      triangle.points[2].x,
      triangle.points[2].y,
      0xFFFFFF00);
  }

  array_free(triangles_to_render);
  
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
