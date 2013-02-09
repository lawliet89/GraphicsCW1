#ifndef _CG_RENDER_H

#define _CG_RENDER_H

#include <GL/glut.h>
#include <stdlib.h>

using namespace std;

#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>

// Path to VTK file
#define VTK_PATH "data/face.vtk"
// Path to texture file
#define TEXTURE_PATH "data/face.ppm"

// Define the various data structure for storing the image data
template <typename T=float> struct Vertex{
   T x, y, z;	// Vertex coordinates
   T textureX, textureY;	// Texture coordinates
};

// Function prototypes
void init();
void display(void);
void reshape (int w, int h);
void keyboard(unsigned char key, int x, int y);
void loadData();

#endif
