/**
 * Yong Wen Chua (ywc110)
 *
 * Resources:
 *  - Lighting: http://www.movesinstitute.org/~mcdowell/mv4202/notes/lect13.pdf
 *  - General Tutorial: http://www.lighthouse3d.com/tutorials/glut-tutorial/
 *  - OpenGL Matrix Maths: http://unspecified.wordpress.com/2012/06/21/calculating-the-gluperspective-matrix-and-other-opengl-matrix-maths/
 *  - Display List: http://www.songho.ca/opengl/gl_displaylist.html
 */

/*************** Includes *******************/
#include <GL/glut.h>
#include <cstdlib>
#include <cmath>

#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>

using namespace std;

/*************** Macros *******************/
// Path to VTK file
#define VTK_PATH "data/face.vtk"
// Path to texture file
#define TEXTURE_PATH "data/face.ppm"

// Rotating angle step
#define ROTATION_STEP 2.f

// Zooming step
#define ZOOM_STEP 0.1f

// Translation step
#define TRANSLATE_STEP 0.001f

// Factor to multiply for rotation
#define ROTATION_CLOCKWISE -1.f
#define ROTATION_ANTICLOCKWISE 1.f

/*************** Classes *******************/
template <typename T=float> class Coordinate{
    T x, y, z;  // Components
public:
    Coordinate(): x(0), y(0), z(0){}
    Coordinate(T x, T y, T z): x(x), y(y), z(z){}
    Coordinate(const Coordinate<T> &obj): x(obj.x), y(obj.y), z(obj.z) {}

    // Assignment Operator
    Coordinate<T> &operator=(const Coordinate<T> &obj){
       if (this == &obj)
           return *this;
       x = obj.x;
       y = obj.y;
       z = obj.z;

       return *this;
    }

    // Dot product
    T operator|(const Coordinate<T> &obj) const{
       return obj.x*x + obj.y*y + obj.z*z;
    }
    // Cross Product
    Coordinate<T> operator*(const Coordinate<T> &obj) const{
        Coordinate<T> result;
       result.x = y*obj.z - z*obj.y;
       result.y = obj.x*z - x*obj.z;
       result.z = x*obj.y-obj.x*y;

       return result;
    }
    // Scalar Product
    Coordinate<T> operator*(const T scalar) const{
       return Coordinate<T>(scalar*x, scalar*y, scalar*z);
    }

    // Addition
    Coordinate<T> operator+(const Coordinate<T> &obj) const{
       return Coordinate<T>(x+obj.x, y+obj.y, z+obj.z);

    }
    // Subtraction
    Coordinate<T> operator-(const Coordinate<T> &obj) const{
       return Coordinate<T>(x-obj.x, y-obj.y, z-obj.z);
    }

    // magnitude of coordinate
    T magnitude() const{
       return sqrt(x*x+y*y+z*z);
    }

    // normalise coordinate vector
    Coordinate<T> normalise() const{
       T mag = magnitude();
       return Coordinate<T>(x/mag, y/mag, z/mag);
    }

    /**
     * Getters and Setters
     */
    T getX() const {
        return x;
    }

    void setX(T x) {
        this->x = x;
    }

    T getY() const {
        return y;
    }

    void setY(T y) {
        this->y = y;
    }

    T getZ() const {
        return z;
    }

    void setZ(T z) {
        this->z = z;
    }
};

// Vertex Class
template <typename T=float> class Vertex{
    Coordinate<T> vertex, texture, averageNormal;
    vector<Coordinate <T> > normals;
public:
    // Constructor
    Vertex(){}
    Vertex(T x, T y, T z): vertex(x,y,z){}
    Vertex(const Vertex<T> &obj): vertex(obj.vertex), texture(obj.texture), averageNormal(obj.averageNormal), normals(obj.normals){}    // Copy constructor

    // Assignment Operator
    Vertex<T> &operator=(const Vertex<T> &obj){
       if (this == &obj)
           return *this;

       vertex = obj.vertex;
       texture = obj.texture;
       averageNormal = obj.averageNormal;
       normals = obj.normals;
       return *this;
    }


    void pushNormals(Coordinate <T> normal){
        normals.push_back(normal);
    }

    void calculateAverageNormal(){
        T x = 0, y = 0, z = 0;
        int n = normals.size();
        for (typename vector<Coordinate <T> >::iterator it = normals.begin(); it < normals.end(); it++){
            Coordinate<T> current = *it;
            x += current.getX()/n;
            y += current.getY()/n;
            z += current.getZ()/n;
        }
        averageNormal.setX(x);
        averageNormal.setY(y);
        averageNormal.setZ(z);
        averageNormal.normalise();
    }

    /**
     * Getters and Setters
     */
    const Coordinate<T>& getAverageNormal() const {
        return averageNormal;
    }

    const Coordinate<T>& getTexture() const {
        return texture;
    }

    void setTexture(const Coordinate<T>& texture) {
        this->texture = texture;
    }

    void setTexture(const T x, const T y){
        texture.setX(x);
        texture.setY(y);
        texture.setZ(0);
    }

    const Coordinate<T>& getVertex() const {
        return vertex;
    }

    void setVertex(const Coordinate<T>& vertex) {
        this->vertex = vertex;
    }

    void setVertex(const T x, const T y, const T z){
        vertex.setX(x);
        vertex.setY(y);
        vertex.setZ(z);
    }

    const vector<Coordinate<T> >& getNormals() const {
        return normals;
    }

    vector<Coordinate<T> >& getNormals(){
            return normals;
        }

    void setNormals(const vector<Coordinate<T> >& normals) {
        this->normals = normals;
    }
};

// Overload to allow for e.g. cout << Coordinate
template <typename T=float> std::ostream& operator<< (std::ostream &output, const Coordinate<T> &obj){
    output << "(" << obj.getX() << "," << obj.getY() << "," << obj.getZ() << ")";

    return output;
}

template <typename T=float> std::ostream& operator<< (std::ostream &output, const Vertex<T> &obj){
    return operator<<(output, obj.getVertex());
}

/*************** Function Prototypes *******************/
void init();        // Initialise polygons, lighting, and texture
void idle();        // Idle rotation animation
void display();     // Render
void reshape (int w, int h);    // Viewport change
void keyboard(unsigned char key, int x, int y); // respond to keyboard
void keyboardSpecial (int key, int x, int y);   // ditto
void loadData();    // load polygon data and texture
void screendump(short W, short H);  // dump a screenshot
void setMaterial();     // set the material setting on the face

/****************** Materials Related Declaration and variables ***************************/
// Material state
/*
 * 0 - none - based on defaults @ http://www.gamedev.net/topic/283067-reset-material-values-to-default/
 *
 * Human skin tone colours based on RGB at http://www.makehuman.org/forum/viewtopic.php?f=8&t=1529
 * 1 - white person
 * 2 - Asian (oriental) person (presumably)
 * 3 - black person
 */
int materialState = 0;
#define MAX_MATERIAL_STATE 3

GLfloat materialAmbient[MAX_MATERIAL_STATE+1][3] = {
        {0.2f, 0.2f, 0.2f},
        {1.0f, (206.f/255.f), (180.f/255.f)},
        {180.f/255.f, 138.f/255.f, 120.f/255.f },
        {60.f/255.f, 46.f/255.f, 40.f/255.f}
};

GLfloat materialDiffuse[MAX_MATERIAL_STATE+1][3] = {
        {0.8f, 0.8f, 0.8f},
        {1.0f, (195.f/255.f), (170.f/255.f)},
        {165.f/255.f, 126.f/255.f, 110.f/255.f},
        {45.f/255.f, 34.f/255.f, 30.f/255.f}
};

GLfloat materialSpecular[MAX_MATERIAL_STATE+1][3] = {
        {0.f, 0.f, 0.f},
        {1.0f, (218.0f/255.0f), (190.0f/255.0f) },
        {195.f/255.f, 149.f/255.f, 130.f/255.f},
        {75.f/255.f, 57.f/255.f, 50.f/255.f}

};

/******************* GLOBALS **********************************/
// Global variables
vector< Vertex<float> > vertices;   // vector of vertices
vector< vector< int > > polygons;   // vector of indices for the vertices for each polygon
vector< Coordinate<float> > polygonsNormal; // Normal for each polygon

GLuint texture, displayList;	// OpenGL indices for texture and display list
//GLfloat lightPosition[] = {0.0f, 0.0f, 0.0f, 1.0f}; // unused

float angle = 0.0f;
float zoom = 1.f;
float translationFactor = 0.f;

int viewportWidth, viewportHeight;

// Calculated values
// min, max, and centre (mean) of all the vertices. Also the mean of polygon normal
// camera position, direction vector from camera to centreVertex
// translationVector is given by cameraVector X (0,1,0)
Coordinate<float> minVertex, maxVertex, centreVertex, meanNormal, camera, cameraVector, translationVector;

// Toggles
bool rotate = false;
bool showTexture = true;
float rotationFactor = ROTATION_ANTICLOCKWISE;

// Texture Data
int textureWidth, textureHeight;
char *textureData;

/******************** FUNCTIONS ***********************/

int main(int argc, char** argv){
    // Load data to memory
    loadData();

    // Initialize graphics window
    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); // double buffering
    // c.f. http://en.wikipedia.org/wiki/Depth_buffer for info on depth buffering

    glutInitWindowSize (1024, 1024);
    glutInitWindowPosition (-1, -1);    // set to -1 to let window manager decide
    glutCreateWindow ("Graphics Coursework 1 (ywc110)");

    // Initialise polygons, lighting, and texture
    init();

    cout << "Initialised" << endl;

    // Initialize callback functions
    glutDisplayFunc(display);   // render display
    glutReshapeFunc(reshape);   // window resize
    glutIdleFunc(idle);     // window idle
    glutKeyboardFunc(keyboard); // keyboard press events
    glutSpecialFunc(keyboardSpecial);   // keyboard special keys

    // Start rendering
    glutMainLoop();
}

void init(){
    glClearColor (0.0, 0.0, 0.0, 0.0);
    cout << "Setting up lighting" << endl;

    glShadeModel (GL_SMOOTH); // http://www.opengl.org/sdk/docs/man2/xhtml/glShadeModel.xml

    // Enable lighting
    glEnable (GL_LIGHTING);
    glEnable (GL_LIGHT0); //Light 0: white light (1.0, 1.0, 1.0, 1.0) in RGBA diffuse and specular components
    //glEnable(GL_COLOR_MATERIAL);  // allow vertex colours to be set as material colour
    //glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    // Light position: cf http://www.opengl.org/archives/resources/faq/technical/lights.htm#ligh0050
    // Default lighting settings: http://opengl.org.ru/docs/pg/0504.html


    GLfloat lightAmbient[] = {0.f, 0.f, 0.f, 1.0f};
    glLightfv(GL_LIGHT0, GL_AMBIENT,  lightAmbient);

    GLfloat lightDiffuse[] = {0.7f, 0.7f, 0.7f, 1.0f};
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  lightDiffuse);

    GLfloat lightSpecular[] = {0.9f, 0.9f, 0.9f, 1.0f};
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);

    //glLightf(GL_LIGHT0, GL_SPOT_EXPONENT,1.0f);

    //glLightf( GL_LIGHT0, GL_SPOT_CUTOFF, 150.0 );

    // Global Ambient Light
    //GLfloat lightModelAmbient[] = {0.2, 0.2, 0.2, 1.0};
    //glLightModelfv( GL_LIGHT_MODEL_AMBIENT, lightModelAmbient );

    // Local viewpoint
    //glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

    // Two sided lighting
    //glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE );


    //glEnable(GL_NORMALIZE);   // Auto normlization of normals, but slow

    // Enable Z-buffering
    glEnable(GL_DEPTH_TEST);

    /*
     * Links:
     *  - http://www.opengl.org/wiki/Common_Mistakes#Automatic_mipmap_generation
     *  - http://www.nullterminator.net/gltexture.html
     */
    // Allocate a texture name
    glGenTextures(1, &texture);

    // select our current texture
    glBindTexture( GL_TEXTURE_2D, texture );

    // select modulate to mix texture with colour for shading
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

    // clamp texture at edges
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );

    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);

    // assign texture
    glTexImage2D(GL_TEXTURE_2D, 0, 3, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData);

    //gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB8, textureWidth, textureHeight, GL_RGB, GL_UNSIGNED_BYTE, textureData);

    //glBindTexture(GL_TEXTURE_2D, 0);

    cout << "Texture loaded" << endl;

    delete[] textureData;   // can now be safely deleted

    // Initialise polygons
    displayList = glGenLists(1);	// create display list
    glNewList(displayList, GL_COMPILE);	// compile

    for (vector< vector< int > >::iterator i = polygons.begin(); i < polygons.end(); i++){
        vector<int> polygon = *i;
        glBegin(GL_POLYGON);    // Begin drawing polygon

        for (vector<int>::iterator j = polygon.begin(); j < polygon.end(); j++){
            Vertex<float> vertex = vertices.at(*j);

            // Define texture coordinates of vertex
            glTexCoord2f(vertex.getTexture().getX(), vertex.getTexture().getY());

            // Define normal of vertex
            glNormal3f(vertex.getAverageNormal().getX(),vertex.getAverageNormal().getY(), vertex.getAverageNormal().getZ());

            // Define coordinates of vertex
            glVertex3f(vertex.getVertex().getX(), vertex.getVertex().getY(), vertex.getVertex().getZ());



        }
        glEnd();
    }
    glEndList();
}

void idle(){
    if (!rotate) return;
    angle += rotationFactor*ROTATION_STEP;
    glutPostRedisplay();
}

// Render the scene
void display(){
    // Clear Color and Depth Buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //cout << "display" << endl;

    if (showTexture){
        // Texture mapping
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture );
    }
    // Reset transformation
    glLoadIdentity();

    // Set the Camera
    // gluLookAt(eyex, eyey, eyez, centerx, centery, centerz, upx, upy, upz);

    Coordinate<float> eye;  // camera position
    if (zoom != 1.f){
        eye = centreVertex - cameraVector*(1.f/zoom);           // this is basically a 1/x curve
    }
    else{
        eye = camera;
    }

    Coordinate<float> lookAt;   // look at position
    if (translationFactor != 0){
        lookAt = centreVertex + translationVector * translationFactor;
    }
    else{
        lookAt = centreVertex;
    }

    gluLookAt(eye.getX(), eye.getY(), eye.getZ(),
            lookAt.getX(), lookAt.getY(),  lookAt.getZ(),
            0.0f, 1.0f,  0.0f);

    // "Clean up" rotation angle variable
    if (angle > 0) angle = fmod(angle,360.f);
    else if (angle < 0) angle = -fmod( fabs(angle), 360.f);

    // Rotation
    glRotatef(angle, 0.f, centreVertex.getY(), 0.f);

    // Draw polygons
    glCallList(displayList);

    if (showTexture)
        glDisable(GL_TEXTURE_2D);
    //glFlush ();
    glutSwapBuffers();
}

// cf http://www.lighthouse3d.com/tutorials/glut-tutorial/preparing-the-window-for-a-reshape/
void reshape (int w, int h)
{
    //cout << "reshape" << endl;
    viewportWidth = w;
    viewportHeight = h;

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
    // fov, aspect ratio, near g planes, far clipping planes
    gluPerspective(27.5,ratio, 0.0001f, 20.f);

    // set the camera view
    // gluLookAt(eyex, eyey, eyez, centerx, centery, centerz, upx, upy, upz);

    // Get back to the ModelView
    glMatrixMode (GL_MODELVIEW);

    glLoadIdentity(); // Identity matrix
}

// "Normal" key presses
void keyboard(unsigned char key, int x, int y){
    switch (key) {
        case 27: // ESC
            exit(0);
            break;
        case 'r':
            rotate = !rotate;
            break;
        case 'p':
            screendump(viewportWidth, viewportHeight);
            cout << "Dumped" << endl;
            break;
        case 'm':
            materialState++;
            if (materialState > MAX_MATERIAL_STATE)
                materialState = 0;
            setMaterial();
            cout << "Material toggled: " << materialState << endl;
            if (!rotate) glutPostRedisplay();
            break;
        case 't':
            showTexture = !showTexture;
            cout << "Texture toggled: " << showTexture << endl;
            if (!rotate) glutPostRedisplay();
            break;
        case 's':   // Output current setting to console
            cout << "Zoom factor: " << zoom << endl;
            cout << "Translation factor: " << translationFactor << endl;
            cout << "Rotation angle: " << angle << endl;
            cout << "Material state: " << materialState << endl;
            cout << "Texture state: " << showTexture << endl;
            break;

        case '1':   // Set the scene to take picture for gouraud-1
            /*
                Zoom factor: 2.7
                Translation factor: 0.083
                Rotation angle: 0
                Material State: 0
                Texture State: 0
             */
            zoom = 2.7f;
            translationFactor = 0.083f; // perhaps unnecessary
            angle = 0.f;
            materialState = 0;
            showTexture = false;
            rotate = false;
            setMaterial();
            if (!rotate) glutPostRedisplay();
            break;
        case '2': // Set the scene to take picture for gouraud-2
            /*
                Zoom factor: 2.7
                Translation factor: -0.042
                Rotation angle: 32
                Material state: 0
                Texture State: 0
             */
            zoom = 2.7f;
            angle = 32.f;
            materialState = 0;
            translationFactor = -0.042f;
            rotate = false;
            showTexture = false;
            setMaterial();
            if (!rotate) glutPostRedisplay();
            break;
        case '3': // Set the scene to take picture for gouraud-3
    /*      Zoom factor: 2.7
            Translation factor: -0.099
            Rotation angle: 68
            Texture State: 0
            Material state: 2*/

            zoom = 2.7f;
            angle = 68.f;
            materialState = 2;
            translationFactor = -0.099f;
            rotate = false;
            showTexture = false;
            setMaterial();
            if (!rotate) glutPostRedisplay();
            break;
        case '4': // Set the scene to take picture for texture
    /*      Zoom factor: 2.7
            Translation factor: -0.099
            Rotation angle: 68
            Texture State: 0
            Material state: 2*/

            zoom = 2.7f;
            angle = 68.f;
            materialState = 2;
            translationFactor = -0.099f;
            rotate = false;
            showTexture = true;
            setMaterial();
            if (!rotate) glutPostRedisplay();
            break;
    }


}

// Special key presses
void keyboardSpecial (int key, int x, int y){
    int modifier = glutGetModifiers();
    switch (key){
        case GLUT_KEY_LEFT:
            if (modifier == GLUT_ACTIVE_CTRL){
                angle += rotate ? 0.f : ROTATION_STEP;  // ignore if rotation mode is on
                rotationFactor = rotate ? ROTATION_ANTICLOCKWISE : rotationFactor;  // ignored if rotation factor is not on
            }
            else{
                translationFactor += TRANSLATE_STEP;
            }
            break;
        case GLUT_KEY_RIGHT:
            if (modifier == GLUT_ACTIVE_CTRL){
                angle -= rotate ? 0.f : ROTATION_STEP;  // ignore if rotation mode is on
                rotationFactor = rotate ? ROTATION_CLOCKWISE : rotationFactor;  // ignored if rotation factor is not on
            }
            else{
                translationFactor -= TRANSLATE_STEP;
            }
            break;
        case GLUT_KEY_UP:
            zoom += ZOOM_STEP;
            break;
        case GLUT_KEY_DOWN:
            zoom -= ZOOM_STEP;
            zoom = zoom <= 0.1f ? 0.1f : zoom;  // Max zoom out is at a factor of 0.1
            break;
    }
    if (!rotate)
        glutPostRedisplay();
}

// Loads the VTK file and texture into memory
// Also populates some variables
void loadData(){
    cout << "Loading VTK" << endl;
    ifstream vtk(VTK_PATH); // Open the VTK file
    if (vtk.fail()){    // File opening has failed
        cerr << "Unable to open VTK file \n";
        exit(1);
    }

    // Initialise variables
    minVertex = Coordinate<float>(INFINITY, INFINITY, INFINITY);
    maxVertex = Coordinate<float>(-INFINITY, -INFINITY, -INFINITY);

    string buffer;  // String buffer
    stringstream bufferStream;  // StringStream object

    // The first four lines are essentially useless. Let's get rid of them
    getline(vtk, buffer);       // # vtk DataFile Version 3.0
    getline(vtk, buffer);       // Somebody's face
    getline(vtk, buffer);       // ASCII
    getline(vtk, buffer);       // DATASET POLYDATA

    int n;  // Store the number of points

    // Now let's get the number of points
    // Loop to ensure that we read correctly.
    do{
        getline(vtk, buffer);       // The line with the number of points
        bufferStream.clear();
        bufferStream.str(buffer);   // Put line in to a stringstream

        // Extract the POINTS bit
        bufferStream >> buffer;


    } while(buffer != "POINTS" && !vtk.eof());

    if (vtk.eof()){
        cerr << "File ended unexpectedly (POINTS) \n";
        exit(1);
    }
    // Get the number of points
    bufferStream >> n;
    // Now let's load the vertices
    vertices.reserve(n);

    float centreX = 0, centreY = 0, centreZ = 0;

    for (int i = 0; i < n; i++){
        if (vtk.eof()){
            cerr << "File ended unexpectedly (VERTICES) \n";
            exit(1);
        }
        float x,y,z;
        vtk >> x >> y >> z;
        Vertex<float> current(x,y,z);

        // Determine min coordinates
        minVertex.setX(min(x,minVertex.getX()));
        minVertex.setY(min(y,minVertex.getY()));
        minVertex.setZ(min(z,minVertex.getZ()));

        // Determine max coordinates
        maxVertex.setX(max(x,maxVertex.getX()));
        maxVertex.setY(max(y,maxVertex.getY()));
        maxVertex.setZ(max(z,maxVertex.getZ()));

        // Centre coordinate
        centreX += x/n;
        centreY += y/n;
        centreZ += z/n;

        vertices.push_back(current);
    }
    centreVertex = Coordinate<float>(centreX, centreY, centreZ);

    cout << "Min: " << minVertex << endl;
    cout << "Max: " << maxVertex << endl;
    cout << "Centre: " << centreVertex << endl;
    cout << vertices.size() << " vertices loaded" << endl;

    //Initialise camera position
//  camera.x = ceil(maxVertex.x);
//  camera.y = ceil(maxVertex.y);
//  camera.z = ceil(maxVertex.z);

    camera = maxVertex*10;
    camera.setY(0.f);       // to "straighten" view

    cout << "Camera: " << camera << endl;

    // Camera vector
    cameraVector = centreVertex - camera;
    cout << "Camera vector: " << cameraVector << endl;

    // Translation vector
    translationVector = (cameraVector * Coordinate<float>(0.f,1.f,0.f)).normalise();
    cout << "Translation vector: " << translationVector << endl;


    // Now we might get some leftover garbage like new line delimiter.
    // Use a loop to rid fhe buffer of extraneous new line characters, for example
    do{
      getline(vtk, buffer);     // The line with the number of points
      bufferStream.clear();
      bufferStream.str(buffer); // Put line in to a stringstream

      // Extract the POLYGONS bit
      bufferStream >> buffer;


    } while(buffer != "POLYGONS" && !vtk.eof());

    if (vtk.eof()){
        cerr << "File ended unexpectedly (POLYGONS) \n";
        exit(1);
    }

    // Get the number of polygons
    bufferStream >> n;

    // Get the number of cells
    int cell;
    bufferStream >> cell;

    polygonsNormal.reserve(n);
    int cellCount = 0;  // track and count the number of cells

    centreX = 0;
    centreY = 0;
    centreZ = 0;
    // Load polygons
    for (int i = 0; i < n; i++){
        // Get the number of vertices for the current polygon
        int numVertices;
        vtk >> numVertices;
        cellCount++;
        // Load the vertices for the current polygon
        vector<int> current;
        current.reserve(numVertices);
        for (int j = 0; j < numVertices; j++){
            int index;  // Index of the vertex
            vtk >> index;
            current.push_back(index);
            cellCount++;
        }
        polygons.push_back(current);

        // Calculate the normal for the polygon
        Coordinate<float> normal, vector1, vector2;

        vector1 = vertices.at(current.at(1)).getVertex() - vertices.at(current.at(0)).getVertex();
        vector2 = vertices.at(current.at(2)).getVertex() - vertices.at(current.at(0)).getVertex();

        normal = (vector1*vector2).normalise();

        centreX += normal.getX()/n;
        centreY += normal.getY()/n;
        centreZ += normal.getZ()/n;

        polygonsNormal.push_back(normal);

        // Add normal to all the vertex
        for (vector<int>::iterator it = current.begin(); it < current.end(); it++){
            int i = *it;
            vertices.at(i).pushNormals(normal);
        }

    }
    meanNormal = Coordinate<float>(centreX, centreY, centreZ);
    meanNormal = meanNormal.normalise();

    // Calculate average normals for all the vertices
    for(vector< Vertex<float> >::iterator it = vertices.begin(); it < vertices.end(); it++){
        it -> calculateAverageNormal();
    }
    if (cellCount != cell){
        cerr << "Cell count mismatch " << cell << " vs " << cellCount << endl;
        exit(1);
    }

    cout << polygons.size() << " polygons loaded \n";

    // Get texture data
    do{
      getline(vtk, buffer);     // The line with the number of points
      bufferStream.clear();
      bufferStream.str(buffer); // Put line in to a stringstream

      // Extract the POINT_DATA bit
      bufferStream >> buffer;


    } while(buffer != "POINT_DATA" && !vtk.eof());

    if (vtk.eof()){
        cerr << "File ended unexpectedly (POINT_DATA) \n";
        exit(1);
    }

    // Get number of data points
    bufferStream >> n;
    // Next line can be discarded
    getline(vtk, buffer);

    for (int i = 0; i < n; i++){
        float x, y;
        vtk >> x >> y;
        vertices.at(i).setTexture(x,y);
    }

    cout << n << " texture data points loaded.\n";

    cout << "VTK Load complete" << endl;

    // Load texture - cf http://www.nullterminator.net/gltexture.html
    cout << "Loading ppm texture" << endl;

    ifstream ppm(TEXTURE_PATH);
    if (ppm.fail()){    // File opening has failed
        cerr << "Unable to open PPM file \n";
        exit(1);
    }

    // Check "magic number"
    ppm >> buffer;
    if (buffer != "P6"){
        cerr << "Invalid magic number" << endl;
        exit(1);
    }

    // Height and width
    ppm >> textureWidth >> textureHeight;

    cout << textureWidth << " x " << textureHeight << " texture found" << endl;

    int maxVal;
    ppm >> maxVal;

    cout << "MaxVal: " << maxVal << endl;

    if (maxVal > 255){
        cerr << "Only maxval of up to 255 is supported \n";
        exit(1);
    }

    // Read away the next whitespace (one byte or a char)
    ppm.get();

    if (ppm.fail() || ppm.eof()){
        cerr << "Texture loading failed" << endl;
        exit(1);
    }

    int size = textureHeight*textureWidth*3;
    textureData = new char[size];
    ppm.read(textureData, size);

    if (ppm.fail()){
            cerr << "Texture loading failed" << endl;
            exit(1);
    }

    cout << "Texture read: " << size << " bytes" << endl;


    //GLenum error = glGetError();
    //cerr << error << endl;

    //delete[] textureData;
}

// Save file as TGA
// from http://www.opengl.org/discussion_boards/showthread.php/161499-Output-Image-to-file
// doesn't seem to work well with textures.
void screendump(short W, short H){
    FILE   *out = fopen("screenshot.tga","wb");
    char   *pixel_data = new char[3*W*H];
    short  TGAhead[] = { 0, 2, 0, 0, 0, 0, W, H, 24 };

    glReadBuffer(GL_FRONT);
    glReadPixels(0, 0, W, H, GL_BGR, GL_UNSIGNED_BYTE, pixel_data);

    fwrite(TGAhead,sizeof(TGAhead),1,out);
    fwrite(pixel_data, 3*W*H, 1, out);
    fclose(out);

    delete[] pixel_data;
}

void setMaterial(){
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,  materialAmbient[materialState]);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,  materialDiffuse[materialState]);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, materialSpecular[materialState]);

    GLfloat materialShininess[] = { 5.0f };
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, materialShininess);
}
