//============================================================================
// Name        : Assignment 5-1 wbo.cpp
// Author      : Warren Outlaw
// Course      : CS-330-J6135
// Description : Assignment 5-1, Practice Activity 6
//
// This project implements an orbiting camera function to mouse movement while
// holding alt + left mouse and a zooming function to mouse movement while
// holding alt + right mouse.
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

#define WINDOW_TITLE "Warren Outlaw: Assignment 5-1" // Window title macro

/* Shader program macro */
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version "\n" #Source
#endif

/* Variable declarations for shader, window size initialization, buffer and array objects */
GLint shaderProgram, WindowWidth = 800, WindowHeight = 600;
GLuint VBO, VAO;

bool orbit = false; // Orbit enable variable for Alt + LeftMouse
bool zoom = false; // Zoom enable variable for Alt + RightMouse
bool mouseDetected = true; // Initially true when mouse movement is detected

GLfloat lastMouseX = 400, lastMouseY = 300; // Locks mouse cursor at the center of screen
GLfloat mouseXOffset, mouseYOffset, yaw = 0.0f, pitch = 0.0f; // Mouse offset, yaw, and pitch vars
GLfloat sensitivity = 0.8f; // Used for mouse sensitivity
GLfloat fov = 45.0f; // FoV variable for zooming functionality

// Timing to generate zoom speed
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f;

/* Global vector declarations */
glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 10.0f); // Initial camera position
glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f); // Temp z unit vector
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); // Temp y unit vector
glm::vec3 front = glm::vec3(0.0f, 0.0f, 10.0f); // Temp z unit vector for mouse

/* Function prototypes */
void UResizeWindow(int, int);
void URenderGraphics(void);
void UCreateShader(void);
void UCreateBuffers(void);
void UMouseMove(int x, int y);
void UMouseClick(int button, int state, int x, int y);

/* Vertex Shader Source Code */
const GLchar * vertexShaderSource = GLSL(330,
	layout (location = 0) in vec3 position; // Vertex data from Vertex Attrib Pointer 0
	layout (location = 1) in vec3 color; // Color data from Vertex Attrib Pointer 1
	
	out vec3 mobileColor; // Variable to transfer color data to the fragment shader
	
	// Global variables for the transform matrices
	uniform mat4 model;
	uniform mat4 view;
	uniform mat4 projection;
	
	void main(){
		gl_Position = projection * view * model * vec4(position, 1.0f); // Transforms vertices to clip coordinates
		mobileColor = color; // References incoming color data
	}
);

/* Fragment Shader Source Code */
const GLchar * fragmentShaderSource = GLSL(330,
	in vec3 mobileColor; // Variable to hold incoming color data from vertex shader
	
	out vec4 gpuColor; // Variable to pass color data to the GPU
	
	void main(){
		gpuColor = vec4(mobileColor, 1.0); // Sends color data to the GPU for rendering
	}
);

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

	UCreateShader();

	UCreateBuffers();
	
	// Use the shader program
	glUseProgram(shaderProgram);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color
	
	glutDisplayFunc(URenderGraphics);
	
	glutMotionFunc(UMouseMove); // Detects mouse movement
	glutMouseFunc(UMouseClick); // Detects mouse movement
	
	glutMainLoop();
	
	// Destroy buffer objects once used
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	
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
	// Generate time between currentFrame and lastFrame for consistent zoomSpeed between machines
	int currentFrame = glutGet(GLUT_ELAPSED_TIME);
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	glEnable(GL_DEPTH_TEST); // Enable z-depth
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clears the screen
	
	cameraPosition = front; // Replaces camera position vector with radians normalized as a unit vector

	glBindVertexArray(VAO); // Activate the vertex array object before rendering and transforming
	
	// Transforms the object
	glm::mat4 model;
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // Place object in center of the viewport
	model = glm::rotate(model, 45.0f, glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate object 45 degrees on the x
	model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f)); // Increase the object size by a scale of 2
	
	// Transforms the camera
	glm::mat4 view;
	view = glm::lookAt(cameraPosition, cameraTarget, cameraUp);
	
	// Creates a perspective projection
	glm::mat4 projection;
	projection = glm::perspective(glm::radians(fov), (GLfloat)WindowWidth / (GLfloat)WindowHeight, 0.1f, 100.0f);
	
	// Retrieves and passes transform matrices to the shader program
	GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
	GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
	GLint projLoc = glGetUniformLocation(shaderProgram, "projection");
	
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
	
	glutPostRedisplay();
	
	// Draws the triangles
	glDrawArrays(GL_TRIANGLES, 0, 36);
	
	glBindVertexArray(0); // Deactivate the vertex array object
	
	glutSwapBuffers(); // Swaps front and back buffer every frame
}

/* Creates the shader program */
void UCreateShader()
{
	// Vertex shader
	GLint vertexShader = glCreateShader(GL_VERTEX_SHADER); // Creates the vertex shader
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL); // Attaches the vertex shader to the source code
	glCompileShader(vertexShader); // Compiles the vertex shader
	
	// Fragment shader
	GLint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER); // Creates the fragment shader
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL); // Attaches the fragment shader to the source code
	glCompileShader(fragmentShader); // Compiles the fragment shader

	// Shader program
	shaderProgram = glCreateProgram(); // Creates shader program and returns an id
	glAttachShader(shaderProgram, vertexShader); // Attach vertex shader to shader program
	glAttachShader(shaderProgram, fragmentShader); // Attach fragment shader to shader program
	glLinkProgram(shaderProgram); // Link vertex and fragment shaders to the shader program

	// Delete the vertex and fragment shaders once linked
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void UCreateBuffers()
{
	GLfloat vertices[] = {
							// Positions		 // Color
							-0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
							 0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
							 0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
							 0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
							-0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
							-0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
							
							-0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 0.0f,
							 0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 0.0f,
							 0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f,
							 0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f,
							-0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f,
							-0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 0.0f,
							
							-0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
							-0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
							-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
							-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
							-0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
							-0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
							
							 0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 0.0f,
							 0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 0.0f,
							 0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f,
							 0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f,
							 0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 0.0f,
							 0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 0.0f,
							 
							-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f,
							 0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f,
							 0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
							 0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
							-0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
							-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f,
							
							-0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 1.0f,
							 0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 1.0f,
							 0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 1.0f,
							 0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 1.0f,
							-0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 1.0f,
							-0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 1.0f
	};
	
	// Generate buffer ids
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	
	// Activate the VAO before binding and setting any VBOs and vertex attrib pointers
	glBindVertexArray(VAO);
	
	// Activate the VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // Copies vertices to the VBO
	
	// Set attrib pointer 0 to hold position data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0); // Enable vertex attrib
	
	// Set attrib pointer 1 to hold color data
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1); // Enable vertex attrib
	
	glBindVertexArray(0); // Deactivates the VAO
}

/* Implements the UMouseMove function */
void UMouseMove(int x, int y)
{	
	GLfloat zoomSpeed = 0.5 * deltaTime; // Camera speed for zooming

	// Immediately replaces center locked coords with new mouse coords
	if(mouseDetected) {
		lastMouseX = x;
		lastMouseY = y;
		mouseDetected = false;
	}

	// Gets the direction the mouse was moved in x and y
	mouseXOffset = x - lastMouseX;
	mouseYOffset = lastMouseY - y;

	// Updates with new mouse coords
	lastMouseX = x;
	lastMouseY = y;

	// Applies sensitivity to mouse direction
	mouseXOffset *= sensitivity;
	mouseYOffset *= sensitivity;

	// Implements orbiting
	if(orbit) {
		// Accumulates the yaw and pitch vars
		yaw += mouseXOffset;
		pitch += mouseYOffset;

		// Converts mouse coords / degrees to radians then to vectors
		front.x = 10.0f * cos(glm::radians(yaw));
		front.y = 10.0f * sin(glm::radians(pitch));
		front.z = 10.0f * sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	}

	// Implements zooming
	else if (zoom) {
		if (fov >= 1.0f && fov <= 100.0f)
			fov -= mouseYOffset * zoomSpeed;
		if (fov <= 1.0f)
			fov = 1.0f;
		if (fov >= 100.0f)
			fov = 100.0f;
	}
}

/* Implements the UMouseClick function */
void UMouseClick(int button, int state, int x, int y)
{	
	// Checks for alt + left mouse combination
	if((glutGetModifiers() == GLUT_ACTIVE_ALT) && (button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN)) {
		orbit = true;
	}
	
	// Checks for alt + right mouse combination
	else if((glutGetModifiers() == GLUT_ACTIVE_ALT) && (button == GLUT_RIGHT_BUTTON) && (state == GLUT_DOWN)) {
		zoom = true;
	}

	else {
		orbit = false;
		zoom = false;
	}
}
