//============================================================================
// Name        : Simple3DPrimitive.cpp
// Author      : Warren Outlaw
// Course	   : CS-330-J6135
// Description : Practice Activity 3
//============================================================================

#include <GL/freeglut.h> // Include the freeGLUT header file
#define WINDOW_TITLE "OpenGL 3D Primitive" // Define the window title

/* Define the window's initial width and height */
int
	Width = 420,
	Height = 420;

/* Forward declarations */
void InitWindow(int, char*[]);
void Display();
void Reshape(int, int);
void ApplyDepthSettings();

/* Main function required for Immediate mode */
int main(int argc, char* argv[]) {
	InitWindow(argc, argv); // Window creation function
	glutMainLoop(); // Enters the GLUT event processing loop
	return 0; // Exits main function
}

/* Window initialization including size, position, display modes, color */
void InitWindow(int argc, char* argv[]) {
	glutInit(&argc, argv); // Initializes the freeGLUT library
	glutInitWindowSize(Width, Height); // Specifies the window's width and height
	glutInitWindowPosition(100, 100); // Specifies the position of the window's top-left corner
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA); // Enable depth, double buffering, and RGBA modes
	glutCreateWindow(WINDOW_TITLE); // Create a window and title

	glutDisplayFunc(Display); // Sets the display callback for the screen re-paint event
	glutReshapeFunc(Reshape); // Sets the display callback for the window re-size event
	ApplyDepthSettings(); // Apply depth settings
}

/* Implements Display Callback Handler*/
void Display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clears color and depth buffers
	glMatrixMode(GL_MODELVIEW); // Activates the Model-View matrix

	glLoadIdentity(); // Reset the Model-View matrix
	glTranslatef(-0.0f, 0.0f, -8.0f); // Move the model to the center of the world with a z depth of -7
	glRotatef(45.0f, 0.4f, -1.0f, -0.1f); // Front, right, top
	//glRotatef(150.0f, 0.0f, 1.0f, 0.0f); // Left and back
	//glRotatef(150.0f, 0.0f, 1.0f, -0.4f); // Bottom


	/* Creates the 3D Primitive */
	glBegin(GL_QUADS);

		// Set Front face color (Red) and vertex coordinates
		glColor3f(1.0f,  0.0f, 0.0f);
		glVertex3f( 0.5f,  1.0f, 0.5f);
		glVertex3f(-0.5f,  1.0f, 0.5f);
		glVertex3f(-1.0f, -1.0f, 1.0f);
		glVertex3f( 1.0f, -1.0f, 1.0f);

		// Set Right face color (Green) and vertex coordinates
		glColor3f(0.0f,  1.0f, 0.0f);
		glVertex3f(0.5f,  1.0f, -0.5f);
		glVertex3f(0.5f,  1.0f,  0.5f);
		glVertex3f(1.0f, -1.0f,  1.0f);
		glVertex3f(1.0f, -1.0f, -1.0f);

		// Set Top face color (Blue) and vertex coordinates
		glColor3f(0.0f,  0.0f, 1.0f);
		glVertex3f( 0.5f, 1.0f,  0.5f);
		glVertex3f( 0.5f, 1.0f, -0.5f);
		glVertex3f(-0.5f, 1.0f, -0.5f);
		glVertex3f(-0.5f, 1.0f,  0.5f);

		// Set Left face color (Yellow) and vertex coordinates
		glColor3f(1.0f,  1.0f, 0.0f);
		glVertex3f(-1.0f, -1.0f, -1.0f);
		glVertex3f(-1.0f, -1.0f,  1.0f);
		glVertex3f(-0.5f,  1.0f,  0.5f);
		glVertex3f(-0.5f,  1.0f, -0.5f);

		// Set Back face color (Dull Green) and vertex coordinates
		glColor3f(0.0f,  0.5f, 0.5f);
		glVertex3f( 0.5f,  1.0f, -0.5f);
		glVertex3f(-0.5f,  1.0f, -0.5f);
		glVertex3f(-1.0f, -1.0f, -1.0f);
		glVertex3f( 1.0f, -1.0f, -1.0f);

		// Set Bottom face color (Grey) and vertex coordinates
		glColor3f(0.5f,  0.5f, 0.5f);
		glVertex3f( 1.0f, -1.0f,  1.0f);
		glVertex3f( 1.0f, -1.0f, -1.0f);
		glVertex3f(-1.0f, -1.0f, -1.0f);
		glVertex3f(-1.0f, -1.0f,  1.0f);

	glEnd(); //Ending delimiter for Triangle primitive

	glutSwapBuffers(); // When double buffering, instructs the computer to swap the front and back buffers.
}

/* Reshape settings */
void Reshape(GLsizei width, GLsizei height) {

	if (height == 0) height = 1;
		GLfloat aspect = (GLfloat)width / (GLfloat)height;

	glViewport(0, 0, width, height);

	// Set the aspect ratio of the clipping volume to match the viewport
	glMatrixMode(GL_PROJECTION); // Create a Projection matrix
	glLoadIdentity(); //Reset Projection matrix

	//Set perspective projection, fov, aspect, zNear, and zFar clipping
	gluPerspective(45.0f, aspect, 0.1f, 100.0f);
}

/* Depth and tessellation settings */
void ApplyDepthSettings() {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color to black and opaque
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}
