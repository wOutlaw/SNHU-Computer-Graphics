//============================================================================
// Name        : Assignment 4-1 wbo.cpp
// Author      : Warren Outlaw
// Course      : CS-330-J6135
// Description : Assignment 4-1, Practice Activity 5
//============================================================================

/* Header inclusions */
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>

// GLM Math Header inclusions
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std; // Standard namespace

#define WINDOW_TITLE "Warren Outlaw: Assignment 4-1" // Window title macro

/* Shader program macro */
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version "\n" #Source
#endif

/* Variable declarations for shader, window size initialization, buffer and array objects */
GLint shaderProgram, WindowWidth = 800, WindowHeight = 600;
GLuint VBO, VAO, EBO, texture;

/* Forward declarations */
void UResizeWindow(int, int);
void URenderGraphics(void);
void UCreateShader(void);
void UCreateBuffers(void);

/* Vertex Shader Source Code */
const GLchar * vertexShaderSource = GLSL(330,
	layout (location = 0) in vec3 position; // Vertex data from Vertex Attrib Pointer 0
	layout (location = 1) in vec3 color; // Color data from Vertex Attrib Pointer 1
	
	out vec3 mobileColor; // variable to transfer color data to the fragment shader
	
	// Global variables for the transform matrices
	uniform mat4 model;
	uniform mat4 view;
	uniform mat4 projection;
	
	void main(){
		gl_Position = projection * view * model * vec4(position, 1.0f); // transforms vertices to clip coordinates
		mobileColor = color; // references incoming color data
	}
);

/* Fragment Shader Source Code */
const GLchar * fragmentShaderSource = GLSL(330,
	in vec3 mobileColor; // variable to hold incoming color data from vertex shader
	
	out vec4 gpuColor; // variable to pass color data to the GPU
	
	void main(){
		gpuColor = vec4(mobileColor, 1.0); // sends color data to the GPU for rendering
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
	
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	
	glutDisplayFunc(URenderGraphics);
	
	glutMainLoop();
	
	// Destroys buffer objects once used
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	
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
	glEnable(GL_DEPTH_TEST);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clears the screen
	
	glBindVertexArray(VAO); // activate the Verex Array Object before rendering and transforming them
	
	// Transforms the object
	glm::mat4 model;
	model = glm::translate(model, glm::vec3(0.0, 0.0f, 0.0f)); // place the object at the center of the viewport
	model = glm::rotate(model, 0.0f, glm::vec3(1.0, 1.0f, 1.0f)); // rotate the object 45 degrees on the x-axis
	model = glm::scale(model, glm::vec3(2.0, 2.0f, 2.0f)); // increase the object size by a scale of 2
	
	// Transforms the camera
	glm::mat4 view;
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -4.0f)); // moves the world 0.5 units on x and -5 units on z
	
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

	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

	// draws the triangles
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	
	glBindVertexArray(0); // deactivate the Vertex Array Object
	
	glutSwapBuffers(); // swaps the front and back buffer every frame
}

/* Creates the Shader program */
void UCreateShader()
{
	// Vertex shader
	GLint vertexShader = glCreateShader(GL_VERTEX_SHADER); // creates the Vertex shader
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL); // attaches the Vertex shader to the source code
	glCompileShader(vertexShader); // compiles the Vertex shader
	
	// Fragment shader
	GLint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER); // creates the Fragment shader
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL); // attaches the Fragment shader to the source code
	glCompileShader(fragmentShader); // compiles the Fragment shader
	
	// Shader program
	shaderProgram = glCreateProgram(); // creates the Shader program and returns an id
	glAttachShader(shaderProgram, vertexShader); // attach the Vertex shader to the Shader program
	glAttachShader(shaderProgram, fragmentShader); // attach the Fragment shader to the Shader program
	glLinkProgram(shaderProgram); // link Vertex and Fragment shaders to the Shader program
	
	// Delete the Vertex and Fragment shaders once linked
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

/* Creates the Buffer and Array Objects */
void UCreateBuffers()
{
	// Position and Color data
	GLfloat vertices[] = {
		// Vertex positions			// Colors
		 0.5f,  0.5f,  0.0f,		1.0f, 0.0f, 0.0f,	// top right vertex 0
		 0.5f, -0.5f,  0.0f,		0.0f, 1.0f, 0.0f,	// bottom right vertex 1
		-0.5f, -0.5f,  0.0f,		0.0f, 0.0f, 1.0f,	// bottom left vertex 2
		-0.5f,  0.5f,  0.0f,		1.0f, 0.0f, 1.0f,	// top left vertex 3
		
		 0.5f, -0.5f, -1.0f,		0.5f, 0.5f, 1.0f,	// 4 br right
		 0.5f,  0.5f, -1.0f,		1.0f, 1.0f, 0.5f,	// 5 tl right
		-0.5f,  0.5f, -1.0f,		0.2f, 0.2f, 0.5f,	// 6 tl top
		-0.5f, -0.5f, -1.0f,		1.0f, 0.0f, 1.0f	// 7 bl back
	};
	
	// Index data to share position data
	GLuint indices[] = {
		0, 1, 3,	// triangle 1
		1, 2, 3,	// triangle 2
		0, 1, 4,	// triangle 3
		0, 4, 5,	// triangle 4
		0, 5, 6,	// triangle 5
		0, 3, 6,	// triangle 6
		4, 5, 6,	// triangle 7
		4, 6, 7,	// triangle 8
		2, 3, 6,	// triangle 9
		2, 6, 7,	// triangle 10
		1, 4, 7,	// triangle 11
		1, 2, 7		// triangle 12
		};
	
	// Generate buffer ids
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	
	// Activate the Vertex Array Object before binding and setting any VBOs and Vertex Attribute Pointers
	glBindVertexArray(VAO);
	
	// Activate the VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // copies vertices to VBO
	
	// Activate the Element Buffer Object / Indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); // copies indices to EBO
	
	// Set attribute pointer 0 to hold Position data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0); // enables vertex attribute
	
	// Set attribute pointer 1 to hold Color data
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1); // enables vertex attribute
	
	glBindVertexArray(0); // deactivates the VAO, which is good practice
}
