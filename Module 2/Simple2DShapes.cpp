//============================================================================
// Name        : Simple2DShapes.cpp
// Author      : Warren Outlaw
// Course	   : CS-330-J6135
// Description : Practice Activity 2
//============================================================================

#include <GL/freeglut.h> // Include the freeGLUT header file
#define WINDOW_TITLE "OpenGL 2D Shapes" // Define the window title

/* Define the window's initial width and height */
int
	Width = 420,
	Height = 420;

/* Forward declarations */
void InitWindow(int, char*[]);
void DisplayGraphics(void);
void DrawStar();
void DrawPentagon();
void DrawHeptagon();

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
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // Enable double buffering and RGBA
	glutCreateWindow(WINDOW_TITLE); // Create a window and title

	glutDisplayFunc(DisplayGraphics); // Sets the display callback for the current window

	/*Specify the red, green, blue, and alpha values used when the color buffers are cleared. Color is set to
	Black with full Opacity.*/
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

/* Implements Display Callback Handler*/
void DisplayGraphics(void) {
	glClear(GL_COLOR_BUFFER_BIT); // Uses the color buffer and sets the background color of the window

	DrawStar(); // Add DrawStar to the display callback
	DrawPentagon(); // Add DrawPentagon to the display callback
	DrawHeptagon(); // Add DrawHeptagon to the display callback

	glutSwapBuffers(); // When double buffering, instructs the computer to swap the front and back buffers.
}

/* Draw the Star shape */
void DrawStar() {
	glMatrixMode(GL_MODELVIEW); // Required for transforming (translating, rotating, and scaling) graphic objects
	glLoadIdentity(); //Replaces the current matrix with the identity matrix, i.e. resets of refreshes the matrix for the next frame when the window is resized
	glTranslatef(-0.5f, 0.0f, 0.0f); // Translates all graphic objects -0.5 in the x-axis
	glScalef(0.5f, 0.5f, 0.0f); // Scale the Star down in the x and y-axis

	glBegin(GL_POLYGON); // Starting delimiter for first Polygon
		glColor3f(1.0f, 0.0f, 0.0f); // Sets the vertex color to Red using floating point values

		/* Sets 4 vertices to form the first Polygon */
		glVertex2f(0.12f, 0.63f);
		glVertex2f(0.0f, 1.0f);
		glVertex2f(-0.33f, 0.0f);
		glVertex2f(0.53f, 0.63f);
	glEnd(); //Ending delimiter for first Polygon

	glBegin(GL_POLYGON); // Starting delimiter for second Polygon
		glColor3f(1.0f, 0.0f, 0.0f); // Sets the vertex color to Red using floating point values

		/* Sets 4 vertices to form the second Polygon */
		glVertex2f(-0.12f, 0.63f);
		glVertex2f(0.0f, 1.0f);
		glVertex2f(0.33f, 0.0f);
		glVertex2f(-0.53f, 0.63f);
	glEnd(); //Ending delimiter for second Polygon
}

/* Draw the Pentagon shape */
void DrawPentagon() {
	glMatrixMode(GL_MODELVIEW); // Required for transforming (translating, rotating, and scaling) graphic objects
	glLoadIdentity(); //Replaces the current matrix with the identity matrix, i.e. resets of refreshes the matrix for the next frame when the window is resized
	glTranslatef(0.0f, -0.5f, 0.0f); // Translates all graphic objects -0.5 in the y-axis
	glScalef(0.25f, 0.25f, 0.0f); // Scale the Pentagon down in the x and y-axis

	glBegin(GL_POLYGON); // Starting delimiter for the Pentagon
		glColor3f(1.0f, 1.0f, 1.0f); // Sets the vertex color to White using floating point values

		/* Sets 5 vertices to form the Pentagon */
		glVertex2f(0.0f, 1.0f);
		glVertex2f(-0.95f, 0.31f);
		glVertex2f(-0.59f, -0.81f);
		glVertex2f(0.59f, -0.81f);
		glVertex2f(0.95f, 0.31f);
	glEnd(); //Ending delimiter
}

/* Draw the Heptagon shape */
void DrawHeptagon() {
	glMatrixMode(GL_MODELVIEW); // Required for transforming (translating, rotating, and scaling) graphic objects
	glLoadIdentity(); //Replaces the current matrix with the identity matrix, i.e. resets of refreshes the matrix for the next frame when the window is resized
	glTranslatef(0.5f, 0.25f, 0.0f); // Translates all graphic objects 0.5 in the x-axis and 0.25 in the y-axis
	glScalef(0.25f, 0.25f, 0.0f); // Scale the Heptagon down in the x and y-axis

	glBegin(GL_POLYGON); // Starting delimiter for the Heptagon
	glColor3f(0.0f, 0.0f, 1.0f); // Sets the vertex color to Blue using floating point values

		/* Sets 7 vertices to form the Heptagon */
		glVertex2f(0.0f, 1.0f);
		glVertex2f(0.8f, 0.6f);
		glVertex2f(1.0f, -0.2f);
		glVertex2f(0.4f, -0.9f);
		glVertex2f(-0.4f, -0.9f);
		glVertex2f(-1.0f, -0.2f);
		glVertex2f(-0.8f, 0.6f);
	glEnd(); // Ending delimiter
}
