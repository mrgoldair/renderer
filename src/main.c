#include "array.h"
#include "display.h"
#include "mesh.h"
#include "triangle.h"
#include "vector.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

triangle_t *triangles_to_render = NULL;

vec3_t camera_position = {.x = 0, .y = 0, .z = -5};
float fov_factor = 640;

bool is_running = false;
int previous_frame_time = 0;

void setup(void) {
  // Allocate the required memory in bytes to hold colour buffer
  colour_buffer = malloc(window_width * window_height * sizeof(u_int32_t));

  // Creating SDL texture that is used to display
  colour_buffer_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                                            SDL_TEXTUREACCESS_STREAMING,
                                            window_width, window_height);

  // Load cube data into the mesh data structure
  load_obj_file_data("assets/cube.obj");
}

void process_input(void) {
  SDL_Event event;
  SDL_PollEvent(&event);

  switch (event.type) {
  case SDL_QUIT:
    is_running = false;
    break;
  case SDL_KEYDOWN:
    if (event.key.keysym.sym == SDLK_ESCAPE)
      is_running = false;
    break;
  }
}

vec2_t project(vec3_t point) {
  vec2_t projected = {.x = (fov_factor * point.x) / point.z,
                      .y = (fov_factor * point.y) / point.z};

  return projected;
}

/**
 * @brief We keep track of the change in motion through
 * `mesh.rotation.x/y/z` etc and these accumulated changes
 * are applied to the "identity" mesh; the effective change
 * in mesh verts are never persisted frame-to-frame but instead
 * the changes being applied to the original mesh.
 */
void update(void) {
  //
  int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);

  if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME) {
    SDL_Delay(time_to_wait);
  }

  previous_frame_time = SDL_GetTicks();

  // Every frame init the triangles to render
  triangles_to_render = NULL;

  // Rotation gets updated every frame
  // Rotation in radians
  mesh.rotation.x += 0.01;
  mesh.rotation.y += 0.005;
  mesh.rotation.z += 0.0025;

  int num_mesh_faces = array_length(mesh.faces);

  // Process each face in turn for faces [ A, B, C ]
  for (int i = 0; i < num_mesh_faces; i++) {
    // ...
    face_t mesh_face = mesh.faces[i];

    // Createa vector of dimension 3 for the vertices of the current face we're
    // processing
    vec3_t face_vertices[3];

    // Perform lookup – mesh_face stores an _index_ of a vertex within
    // mesh_vertices .a - 1 because the faces are not 0th indexed
    face_vertices[0] = mesh.vertices[mesh_face.a - 1];
    face_vertices[1] = mesh.vertices[mesh_face.b - 1];
    face_vertices[2] = mesh.vertices[mesh_face.c - 1];

    // Create a variable to hold our projected triangle
    triangle_t projected_triangle;

    // Holds our the transformed vertices of our face
    vec3_t transformed_vertices[3];

    // TRANSFORMATION :: Each face's vertices
    for (int j = 0; j < 3; j++) {
      vec3_t transformed_vertex = face_vertices[j];
      // NOTE: Accumulate the transformation across all 3 axis
      transformed_vertex = vec3_rotate_x(transformed_vertex, mesh.rotation.x);
      transformed_vertex = vec3_rotate_y(transformed_vertex, mesh.rotation.y);
      transformed_vertex = vec3_rotate_z(transformed_vertex, mesh.rotation.z);
      // Simulate moving the camera back by subtracting z-pos of camera from
      // every point NOTE: right-hand rule states that -ve z index goes into
      // _into_ the screen hence we subtract the camera position in order to
      // push the point away
      transformed_vertex.z += 10;

      // Save transformed vertex
      transformed_vertices[j] = transformed_vertex;
    }

    // BACKFACE CULLING ::
    // Determine if this face faces away from the camera. If
    // so then we don't project it.
    //
    // To do so we compare the face's normal to the vector
    // representing the camera looking at the face.

    // The face's normal is derived from it's three vertices
    vec3_t a = transformed_vertices[0];
    vec3_t b = transformed_vertices[1];
    vec3_t c = transformed_vertices[2];

    // ... which are used to construct two vectors
    // that sit on the plane of the face
    // AB = B - A
    // AC = C - A
    vec3_t ab = vec3_sub(b, a);
    vec3_t ac = vec3_sub(c, a);
    // ... their cross product is the normal of the face
    vec3_t face_normal = vec3_cross(ab, ac);

    // The second vector representing the direction of the camera
    vec3_t camera_ray = vec3_sub(camera_position, a);

    // The dot proudtc of two vectors tells us how aligned they are,
    // with 1 being aligned, 0 orthogonal and -1 opposite. Here we're
    // interseted in values zero or greater which means the face is either
    // orthogonal (0) so we can't see it because it's like we're looking
    // right across the top of it; or > 0 which means the face is facing
    // the same direction as the camera aka it's making up the back of the
    // cube with it's normal pointing in the direction of the camera.
    float alignment = vec3_dot(camera_ray, face_normal);

    if (alignment < 0) {
      continue;
    }

    // PROJECTION :: Each face which have not been culled -> [ A, C ]
    for (int j = 0; j < 3; j++) {
      // A projected vertex is a point
      vec2_t projected_point = project(transformed_vertices[j]);

      // Coordinates of mesh vertices are in the range -1/1, which
      // without translation would obviously be -1/1 of the window – the top
      // left.
      projected_point.x += (window_width / 2);
      projected_point.y += (window_height / 2);

      // When all points of a face are transformed and projected we have a
      // triangle ready to render
      projected_triangle.points[j] = projected_point;
    }

    // Add projected triangle to list of triangles ready to render
    array_push(triangles_to_render, projected_triangle);
  }
}

void render(void) {
  draw_grid();

  int num_mesh_triangles = array_length(triangles_to_render);

  // Loop all projected and render
  for (int i = 0; i < num_mesh_triangles; i++) {
    triangle_t triangle = triangles_to_render[i];

    draw_filled_triangle(triangle.points[0].x, triangle.points[0].y, // vertex A
                         triangle.points[1].x, triangle.points[1].y, // vertex B
                         triangle.points[2].x, triangle.points[2].y, // vertex C
                         0xFFFFFF00);

    draw_triangle(triangle.points[0].x, triangle.points[0].y, // vertex A
                  triangle.points[1].x, triangle.points[1].y, // vertex B
                  triangle.points[2].x, triangle.points[2].y, // vertex C
                  0xFFFFFF00);
  }

  array_free(triangles_to_render);

  // colour buffer -> texture
  // texture -> renderer
  render_colour_buffer();
  clear_colour_buffer(0xFF000000);
  SDL_RenderPresent(renderer);
}

void free_resources(void) {
  array_free(mesh.faces);
  array_free(mesh.vertices);
  free(colour_buffer);
}

int main(void) {
  is_running = initialize_window();

  setup();

  while (is_running) {
    process_input();
    update();
    render();
  }

  destroy_window();

  free_resources();

  return true;
}
