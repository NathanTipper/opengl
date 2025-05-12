#ifndef MODEL_LOADER_H
#define MODEL_LOADER_H

#include "nmath.h"
#define MODEL_MAX_VERTICES 10000
#define MODEL_MAX_TEX_COORDS 10000

struct Face
{
  int vertexIndices[10];
  int normalIndices[10];
  int textureIndices[10];
};

struct Model
{
  v4 vertices[MODEL_MAX_VERTICES];
  v3 vertexNormals[MODEL_MAX_VERTICES];
  v2 textureCoordinates[MODEL_MAX_TEX_COORDS];
  Face faces[MODEL_MAX_VERTICES];
};

struct LineReadBuffer
{
  char* line;
  int lineSize;
  int currentPosition;
};

bool 
LoadModel(char* filename, Model* out_model);

bool 
ReadNextLine(char* buffer, int buffer_size, LineReadBuffer* line_buffer);

bool
ReadNextElement(char* buffer, int buffer_size, char* element, int* position);
#endif
