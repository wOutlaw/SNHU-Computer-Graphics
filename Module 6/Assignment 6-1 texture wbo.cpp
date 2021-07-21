//============================================================================
// Name        : Assignment 6-1 wbo.cpp
// Author      : Warren Outlaw
// Course      : CS-330-J6135
// Description : Assignment 6-1, Practice Activity 7
//============================================================================

/* Header inclusions */
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>

/* GLM header inclusions */
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "SOIL2/SOIL2.h"

using namespace std; // Standard namespace

#define WINDOW_TITLE "Warren Outlaw: Assignment 6-1" // Window title macro

/* Shader program macro */
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version "\n" #Source
#endif

/* Variable declarations for shader, window size initialization, buffer and array objects */
GLint shaderProgram, WindowWidth = 800, WindowHeight = 600;
GLuint VBO, VAO, EBO, texture;

/* Function prototypes */
void UResizeWindow(int, int);
void URenderGraphics(void);
void UCreateShader(void);
void UCreateBuffers(void);
void UGenerateTexture(void);

/* Vertex Shader Source Code */
const GLchar * vertexShaderSource = GLSL(330,
	layout (location = 0) in vec3 position; // Vertex data from Vertex Attrib Pointer 0
	layout (location = 2) in vec2 textureCoordinate;
	
	out vec2 mobileTextureCoordinate;
	
	// Global variables for the transform matrices
	uniform mat4 model;
	uniform mat4 view;
	uniform mat4 projection;

	void main(){
		gl_Position = vec4(position.x, position.y, position.z, 1.0f);
		gl_Position = projection * view * model * vec4(position, 1.0f); // Transforms vertices to clip coordinates
		mobileTextureCoordinate = vec2(textureCoordinate.x, 1.0f - textureCoordinate.y); // Flips the texture horizontally
	}
);

/* Fragment Shader Source Code */
const GLchar * fragmentShaderSource = GLSL(330,

	in vec2 mobileTextureCoordinate;

	out vec4 gpuTexture; // Variable to pass texture to the GPU
	
	uniform sampler2D uTexture;
	
	void main(){
		gpuTexture = texture(uTexture, mobileTextureCoordinate); // Sends texture to the GPU for rendering
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
	UGenerateTexture();
	
	// Use the shader program
	glUseProgram(shaderProgram);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color
	
	glutDisplayFunc(URenderGraphics);

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
	glEnable(GL_DEPTH_TEST); // Enable z-depth

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clears the screen
	
	glBindVertexArray(VAO); // Activate the vertex array object before rendering and transforming
	
	// Transforms the object
	glm::mat4 model;
	model = glm::translate(model, glm::vec3(0.0, 0.0f, 0.0f)); // place the object at the center of the viewport
	model = glm::rotate(model, 35.0f, glm::vec3(0.0, 1.0f, 0.0f)); // rotate the object 35 degrees on the y-axis
	model = glm::scale(model, glm::vec3(2.0, 2.0f, 2.0f)); // increase the object size by a scale of 2

	// Transforms the camera
	glm::mat4 view;
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -4.0f)); // moves the world -4 units on z

	// Creates a perspective projection
	glm::mat4 projection;
	projection = glm::perspective(45.0f, (GLfloat)WindowWidth / (GLfloat)WindowHeight, 0.1f, 100.0f);

	// Retrieves and passes transform matrices to the Shader program
	GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
	GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
	GLint projLoc = glGetUniformLocation(shaderProgram, "projection");

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	glutPostRedisplay();

	glBindTexture(GL_TEXTURE_2D, texture);
	
	// Draws the triangles
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	
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
		// Vertex positions			// Texture Coordinates
		 0.0f,  0.5f,  0.0f,		0.0f, 0.0f,	// 0 top of pyramid
		 0.5f, -0.5f,  0.5f,		1.0f, 1.0f,	// 1 front right
		-0.5f, -0.5f,  0.5f,		1.0f, 0.0f,	// 2 front left
		 0.5f, -0.5f, -0.5f,		0.0f, 1.0f,	// 3 back right
		-0.5f, -0.5f, -0.5f,		1.0f, 1.0f	// 4 back left
	};

	// Index data to share position data
	GLuint indices[] = {
		0, 1, 2,	// front
		0, 1, 3,	// right
		0, 3, 4,	// back
		0, 2, 4,	// left
		2, 4, 3,	// base 1
		1, 3, 2		// base 2
		};

	// Generate buffer ids
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	
	// Activate the VAO before binding and setting any VBOs and vertex attrib pointers
	glBindVertexArray(VAO);
	
	// Activate the VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // Copies vertices to the VBO

	// Activate the Element Buffer Object / Indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); // copies indices to EBO

	// Set attrib pointer 0 to hold position data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0); // Enable vertex attrib
	
	// Set attrib pointer 2 to hold texture coordinate data
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2); // Enable vertex attrib

	glBindVertexArray(0); // Deactivates the VAO
}

/* Generate and load the texture */
void UGenerateTexture()
{	
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	int width , height;

	unsigned char* image = SOIL_load_image("wall.png", &width, &height, 0, SOIL_LOAD_RGB);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture
}
