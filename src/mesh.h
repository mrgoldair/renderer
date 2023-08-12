#ifndef MESH_H
#define MESH_H

#include "array.h"
#include "vector.h"
#include "triangle.h"

#define N_CUBE_VERTICES 8
#define N_CUBE_FACES (6 * 2)
extern vec3_t cube_vertices[N_CUBE_VERTICES];
extern face_t cube_faces[N_CUBE_FACES];
void load_cube_mesh_data(void);
void load_obj_file_data(char* filename);

/**
 * @brief Defines a struct for dynamic sized meshes
 * https://abstractexpr.com/2023/06/29/structures-in-c-from-basics-to-memory-alignment/
 */
typedef struct {
  vec3_t* vertices; // dynamic array of vertices
  face_t* faces;    // dynamic array of vertices
  vec3_t rotation;  // Current rotation of mesh in x,y and z
} mesh_t;

extern mesh_t mesh;

#endif