//============================================================================
// Name        : MyFirstOpenGLProject.cpp
// Author      : Warren Outlaw
// Course	   : CS-330-J6135
// Description : Display a 640x480 window with black background and title
//				 "Warren Outlaw" using double buffering.
//============================================================================

#include <GL/freeglut.h> // Include the freeGLUT header file
#define WINDOW_TITLE "Warren Outlaw" // Define the window title

/* Define the window's initial width and height */
int
	Width = 640,
	Height = 480;

/* Forward declarations */
void InitWindow(int, char*[]);
void DisplayGraphics(void);

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
	glutInitWindowPosition(50, 50); // Specifies the position of the window's top-left corner
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
	glutSwapBuffers(); // When double buffering, instructs the computer to swap the front and back buffers.
}
