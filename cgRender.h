#ifndef _CG_RENDER_H

#define _CG_RENDER_H

#include <GL/glut.h>
#include <cstdlib>
#include <cmath>

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
void idle();
void display();
void reshape (int w, int h);
void keyboard(unsigned char key, int x, int y);
void keyboardSpecial (int key, int x, int y);
void loadData();

// Overload to allow cout << Vertex
template <typename T=float> std::ostream& operator<< (std::ostream &output, const Vertex<T> &obj){
	output << "(" << obj.x << "," << obj.y << "," << obj.z << ")";

	return output;
}

#endif
