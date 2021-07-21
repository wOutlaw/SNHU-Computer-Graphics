//============================================================================
// Name        : Assignment 3-1 wbo.cpp
// Author      : Warren Outlaw
// Course	   : CS-330-J6135
// Description : Assignment 3-1, Practice Activity 4
//============================================================================

#include <iostream> // Includes C++ i/o stream
#include <GL/glew.h> // Includes glew header
#include <GL/freeglut.h> // Includes freeglut header

using namespace std; // Uses the standard namespace

#define WINDOW_TITLE "Warren Outlaw: Assignment 3-1" // Macro for window title

// Vertex and Fragment Shader Source Macro
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version "\n" #Source
#endif

// Variables for window width and height
int WindowWidth = 800, WindowHeight = 600;

/* User-defined Function prototypes to:
 * initialize the program, set the window size,
 * redraw graphics on the window when resized,
 * and render graphics on the screen
 */
void UInitialize(int, char*[]);
void UInitWindow(int, char*[]);
void UResizeWindow(int, int);
void URenderGraphics(void);
void UCreateVBO(void);
void UCreateShaders(void);

/* Vertex Shader program Source Code */
const GLchar * VertexShader = GLSL(440,
	in layout(location = 0) vec4 vertex_Position; // Receive vertex coordinates from attribute 0. i.e. 2 floats per vertex

	/* Get the vertex colors from the Vertex Buffer Object */
	in layout(location = 1) vec4 colorFromVBO; // for attribute 1 expect vec(4) floats passed into the Vertex shader
	out vec4 colorFromVShader; // declare a vec 4 variable that will reference the vertex colors passed into the Vertex shader from the buffer
		void main(){
			gl_Position = vertex_Position; // Sends vertex positions to gl_position vec 4
			colorFromVShader = colorFromVBO; // references vertex colors sent from the buffer
		}
);


/* Fragment Shader Program Source Code */
const GLchar * FragmentShader = GLSL(440,
	in vec4 colorFromVShader; // Vertex colors from the vertex shader
	out vec4 vertex_Color; // vec 4 variable that will reference the vertex colors passed intothe Vertex shader from the buffer
	void main(){
		//gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0); // Sets the pixels / fragments of the triangle to green
		vertex_Color = colorFromVShader; // Send the vertex colors to the GPU
	}
);

// Main function. Entry point to the OpenGL program
int main(int argc, char* argv[])
{
	UInitialize(argc, argv); // Initialize the OpenGL program
	glutMainLoop(); // Starts the OpenGL loop in the background
	exit(EXIT_SUCCESS); // Terminates the program successfully
}


// Implements the UInitialize function
void UInitialize(int argc, char* argv[])
{
	// glew status variable
	GLenum GlewInitResult;

	UInitWindow(argc, argv); // Creates the window

	// Checks glew status
	GlewInitResult = glewInit();

	if (GLEW_OK != GlewInitResult)
	{
		fprintf(stderr, "ERROR: %s\n", glewGetErrorString(GlewInitResult));
		exit(EXIT_FAILURE);
	}

	// Displays GPU OpenGL version
	fprintf(stdout, "INFO: OpenGL Version: %s\n", glGetString(GL_VERSION));

	UCreateVBO(); // Calls the function to create the Vertex Buffer Object

	UCreateShaders(); // Calls the function to create the Shader Program

	// Sets the background color of the window to black. Optional
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

}


// Implements the UInitWindow function
void UInitWindow(int argc, char* argv[])
{
	// Initializes freeglut
	glutInit(&argc, argv);

	// Sets the window size
	glutInitWindowSize(WindowWidth, WindowHeight);

	// Memory buffer setup for display
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);

	// Creates a window with the macro placeholder title
	glutCreateWindow(WINDOW_TITLE);

	glutReshapeFunc(UResizeWindow); // Called when the window is resized
	glutDisplayFunc(URenderGraphics); // Renders graphics on the screen

}


// Implements the UResizeWindow function
void UResizeWindow(int Width, int Height)
{
	glViewport(0, 0, Width, Height);
}


// Implements the URenderGraphics function
void URenderGraphics(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clears the screen

	/* Creates the triangle */
	GLuint totalVertices = 6; // Specifies the number of vertices for the triangle i.e. 3
	//glDrawArrays(GL_TRIANGLES, 0, totalVertices); // Draws the Triangle

	// Draw the triangles using the indices
	glDrawElements(GL_TRIANGLES, totalVertices, GL_UNSIGNED_SHORT, NULL);

	glutSwapBuffers(); // Flips the back buffer with the front buffer every frame. Similar to GL Flush
}


// Implements the CreateVBO function
void UCreateVBO(void)
{
	// Specifies coordinates for triangle vertices on x and y
	GLfloat verts[]=
	{
		/* indices will be 0, 1, 2, 3, 4, 1
		 * index 1 will be shared by both triangles
		 */

		/* index 0 */
		-1.0f, 1.0f, // top-left of the screen
		1.0f, 0.0f, 0.0f, 1.0f, // Red vertex

		/* index 1 */
		-0.5f, 0.0f, // middle of left half of the screen
		0.0f, 1.0f, 0.0f, 1.0f, // Green vertex

		/* index 2 */
		-1.0f, 0.0f, // middle-left of the screen
		0.0f, 0.0f, 1.0f, 1.0f, // Blue vertex

		/* index 3 */
		0.0f, 0.0f, // middle of the screen
		1.0f, 0.0f, 0.0f, 1.0f, // Red vertex

		/* index 4 */
		0.0f, -1.0f, // bottom-middle of the screen
		0.0f, 1.0f, 0.0f, 1.0f, // Green vertex

	};

	// Stores the size of the verts array / number of coordinates needed for the triangle i.e. 6
	float numVerticies = sizeof(verts);

	GLuint myBufferID; //Variable for vertex buffer object id
	glGenBuffers(1, &myBufferID); // Creates 1 buffer
	glBindBuffer(GL_ARRAY_BUFFER, myBufferID); // Activates the buffer
	glBufferData(GL_ARRAY_BUFFER, numVerticies, verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

	/* Creates the Vertex Attribute Pointer */
	GLuint floatsPerVertex = 2; // Number of coordinates per vertex
	glEnableVertexAttribArray(0); // Specifies the initial position of the coordinates in the buffer

	/* Strides between vertex coordinates in 6 (x, y, r, g, b, a). A tightly packed stride is 0. */
	GLint vertexStride = sizeof(float) * 6; // The number of floats before each vertex position is 6 i.e. xyrgba

	/* Instructs the GPU on how to handle the vertex buffer object data.
	 * Parameters: attribPointerPosition | coordinates per vertex | data type | deactivate normalization | 0 strides | 0 offset
	 */
	glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, vertexStride, 0); // Change the stride from 0 to 6

	/* Sets an attribute pointer position for the vertex colors i.e. Attribute 1 for rgba floats. Attribute 0 was for position x, y */
	glEnableVertexAttribArray(1); // Specifies position 1 for the color values in the buffer

	GLint colorStride = sizeof(float) * 6; // The number of floats before each color is 6 i.e. rgbaxy

	// Parameters: attribPointerPosition 1 | floats per color is 4 i.e. rgba | data type | deactivate normalization | 6 strides until the next color i
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, colorStride, (char*)(sizeof(float) * 2));

	/* Creates a buffer object for the indices */
	GLushort indices[] = {0,1,2, 3,4,1}; // Using index 1 twice
	float numIndices = sizeof(indices);
	GLuint indexBufferID;
	glGenBuffers(1, &indexBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices, indices, GL_STATIC_DRAW);
}


// Implements the UCreateShaders function
void UCreateShaders(void)
{
	// Create a Shader program object
	GLuint ProgramId = glCreateProgram();

	GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER); // Create a Vertex Shader object
	GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER); // Create a Vertex Shader object

	glShaderSource(vertexShaderId, 1, &VertexShader, NULL); // retrieves the vertex shader source code
	glShaderSource(fragmentShaderId, 1, &FragmentShader, NULL); // retrieves the fragment shader source code

	glCompileShader(vertexShaderId); // Compile the vertex shader
	glCompileShader(fragmentShaderId); // Compile the fragment shader

	// Attaches the vertex and fragment shaders to the shader program
	glAttachShader(ProgramId, vertexShaderId);
	glAttachShader(ProgramId, fragmentShaderId);

	glLinkProgram(ProgramId); // Links the shader program
	glUseProgram(ProgramId); // Uses the shader program
}
