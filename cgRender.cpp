#include "cgRender.h"
using namespace std;

// Global variables
vector< Vertex<float> > vertices;
vector< vector< int > > polygons;

float angle = 0.0f;

// Calculated values
Vertex<float> minVertex, maxVertex, centreVertex;	// min, max, and centre (mean) of all the vertices


int main(int argc, char** argv)
{
	// Load data to memory
	loadData();

	// Initialize graphics window
	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); // double buffering
	// c.f. http://en.wikipedia.org/wiki/Depth_buffer for info on depth buffering

	glutInitWindowSize (1024, 1024);
	glutInitWindowPosition (-1, -1);	// set to -1 to let window manager decide
	glutCreateWindow ("Graphics Coursework 1 (ywc110)");

	// Initialize OpenGL
	init();

	// Initialize callback functions
	glutDisplayFunc(display);	// render display
	glutReshapeFunc(reshape);	// window resize
	glutIdleFunc(idle);		// window idle
	glutKeyboardFunc(keyboard);	// keyboard press events
	glutSpecialFunc(keyboardSpecial);	// keyboard special keys

	// Start rendering
  glutMainLoop();
}




void init()
{
  glClearColor (0.0, 0.0, 0.0, 0.0);
  cout << "init" << endl;

  /*
  glShadeModel (GL_SMOOTH);

  // Enable lighting
  glEnable (GL_LIGHTING);
  glEnable (GL_LIGHT0);
  glLightfv(GL_LIGHT0, GL_POSITION, LightPosition);
  glLightfv(GL_LIGHT0, GL_AMBIENT,  LightAmbient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE,  LightDiffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, LightSpecular);

  // Set material parameters
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  MaterialSpecular);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, MaterialShininess);

  // Enable Z-buffering
  glEnable(GL_DEPTH_TEST);
  */
}

void idle(){
	angle += 0.1f;
	glutPostRedisplay();
}

// Render the scene
void display(void)
{

	// Clear Color and Depth Buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//cout << "display" << endl;

	// Reset transformation
	glLoadIdentity();


	// Set the Camera
	// gluLookAt(eyex, eyey, eyez, centerx, centery, centerz, upx, upy, upz);
	gluLookAt(	0.0f, 0.0f, 10.0f,
			0.0f, 0.0f,  0.0f,
			0.0f, 1.0f,  0.0f);

	glRotatef(angle, 0.0f, 1.0f, 0.0f);

	glBegin(GL_TRIANGLES);
		glVertex3f(-2,-2,-5.0);
		glVertex3f(2,0.0,-5.0);
		glVertex3f(0.0,2,-5.0);
	glEnd();

	glutSwapBuffers();
  /*
  for (all polygons)
    glBegin(GL_POLYGON);
    for (all vertices of polygon)
      // Define texture coordinates of vertex
      glTexCoord2f(...);
      // Define normal of vertex
      glNormal3f(...);
      // Define coordinates of vertex
      glVertex3f(...);
    }
    glEnd();
  }
  glFlush ();
  //  swap buffer
  glutSwapBuffers();
  */
}

// cf http://www.lighthouse3d.com/tutorials/glut-tutorial/preparing-the-window-for-a-reshape/
void reshape (int w, int h)
{
	//cout << "reshape" << endl;

	// Prevent a divide by zero, when window is too short
	if(h == 0)
		h = 1;
	float ratio = 1.0* w / h;

	// Use the Projection Matrix
	glMatrixMode(GL_PROJECTION);

	// Reset Matrix -- identity matrix
	glLoadIdentity();

	// Set viewport to be the entire window
	glViewport (0, 0, (GLsizei) w, (GLsizei) h);

	// Set the correct perspective.
	// fov, aspect ratio, near clipping planes, far clipping planes
	gluPerspective(45,ratio,1,1000);

	// set the camera view
	// gluLookAt(eyex, eyey, eyez, centerx, centery, centerz, upx, upy, upz);

	// Get back to the ModelView
	glMatrixMode (GL_MODELVIEW);

	glLoadIdentity(); // Identity matrix
}

// "Normal" key presses
void keyboard(unsigned char key, int x, int y)
{
  switch (key) {
  case 27: // ESC
    exit(0);
    break;
  }
}

// Special key presses
void keyboardSpecial (int key, int x, int y)
{
	switch (key){

	}
}

// Loads the VTK file and texture into memory
// Also populates some variables
void loadData()
{
	cout << "Loading VTK" << endl;
	ifstream vtk(VTK_PATH);	// Open the VTK file
	if (vtk.fail())
	{	// File opening has failed
		cout << "Unable to open VTK file \n";
		exit(1);
	}

	// Initialise variables
	minVertex.x = INFINITY;
	minVertex.y = INFINITY;
	minVertex.z = INFINITY;

	maxVertex.x = -INFINITY;
	maxVertex.y = -INFINITY;
	maxVertex.z = -INFINITY;

	centreVertex.x = 0;
	centreVertex.y = 0;
	centreVertex.z = 0;

	string buffer;	// String buffer
	stringstream bufferStream;	// StringStream object

	// The first four lines are essentially useless. Let's get rid of them
	getline(vtk, buffer);		// # vtk DataFile Version 3.0
	getline(vtk, buffer);		// Somebody's face
	getline(vtk, buffer);		// ASCII
	getline(vtk, buffer);		// DATASET POLYDATA

	int n;	// Store the number of points

	// Now let's get the number of points
	// Loop to ensure that we read correctly.
	do{
		getline(vtk, buffer);		// The line with the number of points
		bufferStream.clear();
		bufferStream.str(buffer);	// Put line in to a stringstream

		// Extract the POINTS bit
		bufferStream >> buffer;


	} while(buffer != "POINTS" && !vtk.eof());

	if (vtk.eof()){
		cout << "File ended unexpectedly (POINTS) \n";
		exit(1);
	}
	// Get the number of points
	bufferStream >> n;
	// Now let's load the vertices
	vertices.reserve(n);

	for (int i = 0; i < n; i++){
		if (vtk.eof()){
			cout << "File ended unexpectedly (VERTICES) \n";
			exit(1);
		}
		Vertex<float> current;
		vtk >> current.x >> current.y >> current.z;
		vertices.push_back(current);

		// Determine min coordinates
		minVertex.x = (current.x < minVertex.x) ? current.x : minVertex.x;
		minVertex.y = (current.y < minVertex.y) ? current.y : minVertex.y;
		minVertex.z = (current.z < minVertex.z) ? current.z : minVertex.z;

		// Determine max coordinates
		maxVertex.x = (current.x > maxVertex.x) ? current.x : maxVertex.x;
		maxVertex.y = (current.y > maxVertex.y) ? current.y : maxVertex.y;
		maxVertex.z = (current.z > maxVertex.z) ? current.z : maxVertex.z;

		// Centre coordinate
		centreVertex.x += current.x/n;
		centreVertex.y += current.y/n;
		centreVertex.z += current.z/n;
	}
	cout << "Min: " << minVertex << endl;
	cout << "Max: " << maxVertex << endl;
	cout << "Centre: " << centreVertex << endl;
	cout << vertices.size() << " vertices loaded" << endl;

	// Now we might get some leftover garbage like new line delimiter.
	// Use a loop to rid fhe buffer of extraneous new line characters, for example
	do{
	  getline(vtk, buffer);		// The line with the number of points
	  bufferStream.clear();
	  bufferStream.str(buffer);	// Put line in to a stringstream

	  // Extract the POLYGONS bit
	  bufferStream >> buffer;


	} while(buffer != "POLYGONS" && !vtk.eof());

	if (vtk.eof()){
		cout << "File ended unexpectedly (POLYGONS) \n";
		exit(1);
	}

	// Get the number of polygons
	bufferStream >> n;

	// Get the number of cells
	int cell;
	bufferStream >> cell;

	int cellCount = 0;	// track and count the number of cells
	// Load polygons
	for (int i = 0; i < n; i++){
		// Get the number of vertices for the current polygon
		int numVertices;
		vtk >> numVertices;
		cellCount++;
		// Load the vertices for the current polygon
		vector<int> current(numVertices);
		for (int j = 0; j < numVertices; j++){
			int index;	// Index of the vertex
			vtk >> index;
			current.push_back(index);
			cellCount++;
		}
		polygons.push_back(current);
	}
	if (cellCount != cell){
		cout << "Cell count mismatch " << cell << " vs " << cellCount << endl;
		exit(1);
	}

	cout << polygons.size() << " polygons loaded \n";

	// Get texture data
	do{
	  getline(vtk, buffer);		// The line with the number of points
	  bufferStream.clear();
	  bufferStream.str(buffer);	// Put line in to a stringstream

	  // Extract the POINT_DATA bit
	  bufferStream >> buffer;


	} while(buffer != "POINT_DATA" && !vtk.eof());

	if (vtk.eof()){
		cout << "File ended unexpectedly (POINT_DATA) \n";
		exit(1);
	}

	// Get number of data points
	bufferStream >> n;
	// Next line can be discarded
	getline(vtk, buffer);

	for (int i = 0; i < n; i++){
		vtk >> vertices.at(i).textureX >> vertices.at(i).textureY;
	}

	cout << n << " texture data points loaded.\n";

	cout << "VTK Load complete" << endl;
}
