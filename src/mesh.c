#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "array.h"
#include "mesh.h"

mesh_t mesh = {
  .vertices = NULL,
  .faces = NULL,
  .rotation = { 0, 0, 0 }
};

vec3_t cube_vertices[N_CUBE_VERTICES] = {
  { .x = -1, .y = -1, .z = -1 },
  { .x = -1, .y =  1, .z = -1 },
  { .x =  1, .y =  1, .z = -1 },
  { .x =  1, .y = -1, .z = -1 },
  { .x =  1, .y =  1, .z =  1 },
  { .x =  1, .y = -1, .z =  1 },
  { .x = -1, .y =  1, .z =  1 },
  { .x = -1, .y = -1, .z =  1 }
};

face_t cube_faces[N_CUBE_FACES] = {
  // front
  { .a = 1, .b = 2, .c = 3 },
  { .a = 1, .b = 3, .c = 4 },
  // right
  { .a = 4, .b = 3, .c = 5 },
  { .a = 4, .b = 5, .c = 6 },
  // back
  { .a = 6, .b = 5, .c = 7 },
  { .a = 6, .b = 7, .c = 8 },
  // left
  { .a = 8, .b = 7, .c = 2 },
  { .a = 8, .b = 2, .c = 1 },
  // top
  { .a = 2, .b = 7, .c = 5 },
  { .a = 2, .b = 5, .c = 3 },
  // bottom
  { .a = 6, .b = 8, .c = 1 },
  { .a = 6, .b = 1, .c = 4 },
};

void load_cube_mesh_data(void){
  for(int i = 0; i < N_CUBE_VERTICES; i++){
      array_push(mesh.vertices, cube_vertices[i])
  }

  for(int i = 0; i < N_CUBE_FACES; i++){
      array_push(mesh.faces, cube_faces[i])
  }
}

void pull_vector(){
  // Pull `x` component
  // v 1.000000 1.000000 -1.000000
  //   --------
  float x = atof(strtok(NULL, " "));
  // Pull `y` component
  // v 1.000000 1.000000 -1.000000
  //            --------
  float y = atof(strtok(NULL, " "));
  // Pull `z` component
  // v 1.000000 1.000000 -1.000000
  //                     ---------
  float z = atof(strtok(NULL, " "));

  vec3_t vertex = { .x = x, .y = y, .z = z };
  array_push(mesh.vertices, vertex);

  printf("Pulling vector -> x %f, y %f, z %f\n", x, y, z);
}

void pull_face(){
  // At this point we've pulled the "f" from `f 1/2/3 4/5/6 7/8/9`
  // the internal state of strtok is at 1/2/3 4/5/6 7/8/9
  //                                    -----
  char* tok;
  // `a`,`b`,`c` hold our vertex indices
  // `strtok` -> 1/2/3, [0] -> 1
  tok = strtok(NULL, " ");
  int a = tok[0] - '0';
  // `strtok` -> 4/5/6, [0] -> 4
  tok = strtok(NULL, " ");
  int b = tok[0] - '0';
  // `strtok` -> 7/8/9, [0] -> 7
  tok = strtok(NULL, " ");
  int c = tok[0] - '0';

  face_t face = { .a = a, .b = b, .c = c };
  printf("Pulling face -> a %i, b %i, c %i\n", a, b, c);
  array_push(mesh.faces, face);
}

void load_obj_file_data(char* filename){
  //  open file
  //  https://linux.die.net/man/3/getline
  FILE *fp;
  char *line = NULL;
  size_t len = 0;
  ssize_t read;

  fp = fopen(filename, "r");
  if (fp == NULL)
    exit (EXIT_FAILURE);

  while ((read = getline(&line, &len, fp)) != -1) {
    char* tok;
    // The strategy here is to split each line up by spaces
    // using `strtok` and based on the type of line (vertex, normal, face etc)
    // we use the appropriate function to extract the components
  
    // The first thing we do for each new line is determine what
    // type we're dealing with. This is specified by the first letter or pair of letters.
    // e.g. for vertex lines (v 1.000000 1.000000 -1.000000) `tok` will be a "v"
    tok = strtok(line, " ");

    // Process the remainder of the line
    while(tok != NULL){
      // `tok` is a vertex identifier
      if (strcmp(tok,"v") == 0) {
        // Grab the vertex components
        // e.g. 0.00 1.00 1.00
        pull_vector();
      }
      // `tok` is a face identifier
      else if (strcmp(tok,"f") == 0) {
        // Grab the face components
        // e.g. f 1/2/3 4/5/6 7/8/9
        pull_face();
      }
      // Ensure we find the end of the line; NULL terminator
      tok = strtok(NULL, " ");
    }
  }

  free(line);
}