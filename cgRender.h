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
template <typename T=float> struct Vertex{		// struct to make all the methods public by default
   T x, y, z;	// Vertex coordinates
   T textureX, textureY;	// Texture coordinates

   // Constructor
   Vertex(): x(0), y(0), z(0), textureX(0), textureY(0){}
   Vertex(T x, T y, T z): x(x), y(y), z(z), textureX(0), textureY(0){}
   Vertex(const Vertex<T> &obj): x(obj.x), y(obj.y), z(obj.z), textureX(obj.textureX), textureY(obj.textureY) {}	// Copy constructor

   // Assignment Operator
   Vertex<T> &operator=(const Vertex<T> &obj){
	   if (this == &obj)
		   return *this;
	   x = obj.x;
	   y = obj.y;
	   z = obj.z;
	   textureX = obj.textureX;
	   textureY = obj.textureY;

	   return *this;
   }
   // Dot product
   T operator|(const Vertex<T> &obj){
	   return obj.x*x + obj.y*y + obj.z*z;
   }
   // Cross Product
   Vertex<T> operator*(const Vertex<T> &obj){
	   Vertex<T> result;
	   result.x = y*obj.z - z*obj.y;
	   result.y = obj.x*z - x*obj.z;
	   result.z = x*obj.y-obj.x*y;

	   return result;
   }
   // Scalar Product
   Vertex<T> operator*(const T scalar){
	   return Vertex<T>(scalar*x, scalar*y, scalar*z);
   }

   // Addition
   Vertex<T> operator+(const Vertex<T> &obj){
	   return Vertex<T>(x+obj.x, y+obj.y, z+obj.z);

   }
   // Subtraction
   Vertex<T> operator-(const Vertex<T> &obj){
	   return Vertex<T>(x-obj.x, y-obj.y, z-obj.z);
   }

   T magnitude(){
	   return sqrt(x*x+y*y+z*z);
   }

   Vertex<T> normalise(){
	   T mag = magnitude();
	   return Vertex<T>(x/mag, y/mag, z/mag);
   }
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
