#include "cgRender.h"

// Global variables
vector< Vertex<float> > vertices;
vector< vector< int > > polygons;
int main(int argc, char** argv)
{
  // Load data to memory
  loadData();
  
  // Initialize graphics window
  glutInit(&argc, argv);
  glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH); 
  //  Or, can use double buffering
  //  glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); 

  glutInitWindowSize (256, 256); 
  glutInitWindowPosition (0, 0);
  glutCreateWindow (argv[0]);

  // Initialize OpenGL
  init();

  // Initialize callback functions
  glutDisplayFunc(display); 
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);

  // Start rendering 
  glutMainLoop();
}

// Loads the VTK file and texture into memory
void loadData()
{
  cout << "Loading VTK" << endl;
  ifstream vtk(VTK_PATH);	// Open the VTK file
  if (vtk.fail())
  {	// File opening has failed
    cout << "Unable to open VTK file \n";
    exit(1);    
  }
  
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
  }
  
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

void display(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  cout << "display" << endl;

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
  //  or, if double buffering is used,
  //  glutSwapBuffers();
  */
}

void reshape (int w, int h)
{
  cout << "reshape" << endl;

  glViewport (0, 0, (GLsizei) w, (GLsizei) h); 
  /*
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(fovy, aspect, near, far);
  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(eyex, eyey, eyez, centerx, centery, centerz, upx, upy, upz);
  */
}

void keyboard(unsigned char key, int x, int y)
{
  switch (key) {
  case 27: // ESC
    exit(0);
    break;
  }
}