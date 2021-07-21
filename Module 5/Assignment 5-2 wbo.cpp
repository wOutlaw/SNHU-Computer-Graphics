//============================================================================
// Name        : Assignment 5-2 wbo.cpp
// Author      : Warren Outlaw
// Course      : CS-330-J6135
// Description : Assignment 5-2
//
// This program renders the rough draft of the cup for my final project.
//
// Move the mouse while holding the left button to rotate the cup.
// Move the mouse while holding the right mouse button to zoom in or out.
//
// Press the 'd' key to switch between 2D and 3D display modes.
//============================================================================

/* Header inclusions */
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>

/* GLM header inclusions */
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std; // Standard namespace

#define WINDOW_TITLE "Warren Outlaw: Assignment 5-2" // Window title macro

/* Shader program macro */
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version "\n" #Source
#endif

/* Variable declarations for shader, window size initialization, buffer and array objects */
GLint WindowWidth = 800, WindowHeight = 600;

bool orbit = false; // Orbit enable variable for LeftMouse
bool zoom = false; // Zoom enable variable for RightMouse

// Mouse variables for zoom and rotation
GLfloat objectRotX, objectRotY, objectZoom = 1.0;
int curx, cury;

// Keyboard variables for display mode
GLchar currentKey;
bool twoDimensional = false;

/* Function prototypes */
void UResizeWindow(int, int);
void URenderGraphics(void);
void drawCylinder(int numMajor, int numMinor, float height, float radius);
void UMouseMove(int x, int y);
void UMouseClick(int button, int state, int x, int y);
void UKeyboard(unsigned char key, int x, int y);
void UKeyReleased(unsigned char key, int x, int y);
void initialize(void);

/* Main program */
int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(WindowWidth, WindowHeight);
	glutCreateWindow(WINDOW_TITLE);

	glutReshapeFunc(UResizeWindow);

	glewExperimental = GL_TRUE;
		if (glewInit() != GLEW_OK)
		{
			std::cout << "Failed to initialize GLEW" << std::endl;
			return -1;
		}

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color

	glutDisplayFunc(URenderGraphics);

	glutMotionFunc(UMouseMove); // Detects mouse movement
	glutMouseFunc(UMouseClick); // Detects mouse movement
	glutKeyboardFunc(UKeyboard); // Detects key press
	glutKeyboardUpFunc(UKeyReleased); // Detects key release

	initialize(); // Load initial display settings
	glutMainLoop();

	return 0;
}

/* Resizes the window */
void UResizeWindow(int w, int h)
{
	WindowWidth = w;
	WindowHeight = h;
	glViewport(0, 0, WindowWidth, WindowHeight);
}

/* Renders graphics */
void URenderGraphics(void)
{

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Background color to black
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clears the screen

	// 3D display parameters
	glMatrixMode(GL_PROJECTION); // Create a Projection matrix
	glLoadIdentity();
	gluPerspective(60.0, 1.0, 0.1, 100.0); //Set perspective projection, fov, aspect, zNear, and zFar clipping
	glMatrixMode(GL_MODELVIEW); // Activates the Model-View matrix

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// 2D display parameters
	if (twoDimensional) {
		glMatrixMode(GL_PROJECTION); // Create a Projection matrix
		glLoadIdentity();
		glOrtho(-1.0, 1.0, -1.0, 1.0, 0.1, 100.0); //Set perspective projection, fov, aspect, zNear, and zFar clipping
		glMatrixMode(GL_MODELVIEW); // Activates the Model-View matrix
	}

	glPushMatrix();

	glRotatef(objectRotY, 0, 1, 0); // Rotates the object on the Y-axis
	glRotatef(objectRotX, 1, 0, 0); // Rotates the object on the X-axis
	glScalef(objectZoom, objectZoom, objectZoom); // Scale the object to zoom in or out

	// Draw the cylinder
	drawCylinder(1, 360, 0.8, 0.4);

	glPopMatrix();

	glutSwapBuffers(); // Swaps front and back buffer every frame
}

void drawCylinder(int numMajor, int numMinor, float height, float radius)
{
	double majorStep = height / numMajor;
	double minorStep = 2.0 * M_PI / numMinor;
	int i, j;

	// Draw the sides
	for (i = 0; i < numMajor; ++i) {
		GLfloat z0 = 0.5 * height - i * majorStep;
		GLfloat z1 = z0 - majorStep;

		glBegin(GL_TRIANGLE_STRIP);
			for (j = 0; j <= numMinor; ++j) {
				double a = j * minorStep;
				GLfloat x = radius * cos(a);
				GLfloat y = radius * sin(a);

				glColor3f(1.0, 0.0, 0.0); // Top color
				glVertex3f(x, y, z0);

				glColor3f(0.0, 1.0, 0.0); // Bottom color
				glVertex3f(x, y, z1);
		}
		glEnd();
	}

	// Draw the bottom
	glBegin(GL_TRIANGLE_FAN);
		glColor3f(1.0, 1.0, 1.0); // Center color
		glVertex3f(0.0, 0.0, 0.5 * height);
		for (i = 0; i <= numMinor; ++i) {
			double a = i * minorStep;
			GLfloat x = radius * cos(a);
			GLfloat y = radius * sin(a);
			glColor3f(0.0, 0.0, 1.0); // Outer color
			glVertex3f(x, y, 0.5 * height);
		}
	glEnd();
}

/* Implements the UMouseMove function */
void UMouseMove(int x, int y)
{
	if (orbit) {
		// Get the X and Y mouse deltas during movement
		GLfloat dx = (y - cury) * 360.0 / WindowHeight;
		GLfloat dy = (x - curx) * 360.0 / WindowWidth;

		// Rotate the object on the X-axis
		objectRotX += dx;

		// Clamp X rotation to 0 - 360 to prevent odd angles
		if (objectRotX > 360)
			objectRotX -= 360;
		if (objectRotX < 0)
			objectRotX += 360;

		// Rotate the object on the Y-axis
		objectRotY += dy;

		// Clamp Y rotation to 0 - 360 to prevent odd angles
		if (objectRotY > 360)
			objectRotY -= 360;
		if (objectRotY < 0)
			objectRotY += 360;

		// Update the current mouse coordinates
		curx = x;
		cury = y;
	}

	if (zoom) {
		GLfloat dz = (cury - y); // Get the Y mouse delta
		GLfloat zoomSpeed = 0.05; // Zoom speed multiplier

		// Zoom in or out, change the numbers to change zoom limit
		if (objectZoom >= 1.0f && objectZoom <= 10.0f)
			objectZoom += dz * zoomSpeed;
		if (objectZoom <= 1.0f)
			objectZoom = 1.0f;
		if (objectZoom >= 10.0f)
			objectZoom = 10.0f;

		// Update the current mouse coordinates
		cury = y;
	}

	glutPostRedisplay();
}

/* Implements the UMouseClick function */
void UMouseClick(int button, int state, int x, int y)
{
	// Enable orbit while holding left mouse
	if ((state == GLUT_DOWN) && (button == GLUT_LEFT_BUTTON)) {
		orbit = true;
		UMouseMove(curx = x, cury = y);

	// Enable zoom while holding right mouse
	} else if ((state == GLUT_DOWN) && (button == GLUT_RIGHT_BUTTON)){
		zoom = true;
	}
	else {
		orbit = false;
		zoom = false;
	}
}

void UKeyboard(unsigned char key, GLint x, GLint y)
{
	currentKey = key; // Store the press key

	// Swaps between orthographic and perspective displays
	if ((currentKey == 'd') && !twoDimensional) {
		cout << "2D on" << endl;
		twoDimensional = true;
	} else if (currentKey == 'd') {
		cout << "3D on" << endl;
		twoDimensional = false;
	}
}

void UKeyReleased(unsigned char key, GLint x, GLint y)
{
	// Resets the currentKey on key release
	currentKey = '0';
}

void initialize(void)
{
	glMatrixMode(GL_PROJECTION); // Create a Projection matrix
	gluPerspective(60.0, 1.0, 0.1, 100.0); //Set perspective projection, fov, aspect, zNear, and zFar clipping
	glMatrixMode(GL_MODELVIEW); // Activates the Model-View matrix
	glTranslatef(0.0, 0.0, -2.0); // Move the model to the center of the world with a z depth of -2
}
