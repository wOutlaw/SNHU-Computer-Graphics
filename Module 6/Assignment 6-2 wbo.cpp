//============================================================================
// Name        : Assignment 6-2 wbo.cpp
// Author      : Warren Outlaw
// Course      : CS-330-J6135
// Description : Assignment 6-2, Practice Activity 8
//============================================================================

/* Header Inclusions */
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>

/* GLM Math Header Inclusions */
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

/* SOIL image loader Inclusion */
#include "SOIL2/SOIL2.h"

using namespace std; // Standard namespace

#define WINDOW_TITLE "Warren Outlaw: Assignment 6-2" // Window title Macro

/* Shader program Macro */
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version "\n" #Source
#endif

/* Variable declarations for shader, window size initialization, buffer and array objects */
GLint pyramidShaderProgram, lampShaderProgram, WindowWidth = 800, WindowHeight = 600;
GLuint VBO, PyramidVAO, LightVAO, keyLightVAO, texture;

// Subject position and scale
glm::vec3 pyramidPosition(0.0f, 0.0f, 0.0f);
glm::vec3 pyramidScale(2.0f);

// Pyramid and light color
glm::vec3 objectColor(1.0f, 1.0f, 1.0f);
glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
glm::vec3 keyLightColor(0.0f, 1.0f, 0.0f);

// Light position and scale
glm::vec3 keyLightPosition(3.0f, 0.5f, -3.0f);
glm::vec3 lightPosition(-3.5f, 2.5f, -3.0f);
glm::vec3 lightScale(0.3f);

// Camera position
glm::vec3 cameraPosition(0.0f, 0.0f, -10.0f);

// Camera rotation
float cameraRotation = glm::radians(-25.0f);

/* Function prototypes */
void UResizeWindow(int, int);
void URenderGraphics(void);
void UCreateShader(void);
void UCreateBuffers(void);
void UGenerateTexture(void);

/* Pyramid Vertex Shader Source Code */
const GLchar * pyramidVertexShaderSource = GLSL(330,
        layout (location = 0) in vec3 position; // Vertex data from Vertex Attrib Pointer 0
        layout (location = 1) in vec3 normal; // VAP position 1 for normals
        layout (location = 2) in vec2 textureCoordinate;

        out vec3 FragmentPos; // For outgoing color / pixels to fragment shader
        out vec3 Normal; // For outgoing normals to fragment shader
        out vec2 mobileTextureCoordinate;

        // Global variables for the transform matrices
        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;

void main(){
        gl_Position = projection * view * model * vec4(position, 1.0f); // Transforms vertices to clip coordinates

        FragmentPos = vec3(model * vec4(position, 1.0f)); // Gets fragment / pixel position in world space only (exclude view and projection)

        Normal = mat3(transpose(inverse(model))) *  normal; // Get normal vectors in world space only and exclude normal translation properties

        mobileTextureCoordinate = vec2(textureCoordinate.x, 1 - textureCoordinate.y); // Flips the texture horizontal
    }
);

/* Pyramid Fragment Shader Source Code */
const GLchar * pyramidFragmentShaderSource = GLSL(330,

        in vec3 FragmentPos; // For incoming fragment position
        in vec3 Normal; // For incoming normals
        in vec2 mobileTextureCoordinate;

        out vec4 pyramidColor; // For outgoing pyramid color to the GPU

        // Uniform / Global variables for light color, light position, and camera/view position
        uniform vec3 lightColor;
        uniform vec3 keyLightColor;
        uniform vec3 lightPos;
        uniform vec3 keyLightPos;
        uniform vec3 viewPosition;

        uniform sampler2D uTexture; // Useful when working with multiple textures

        void main(){

            /* Phong lighting model calculations to generate ambient, diffuse, and specular components */

            // Calculate Ambient Lighting
            float ambientStrength = 0.2f; // Set ambient or global lighting strength
            vec3 ambient = ambientStrength * lightColor; // Generate ambient light color

            // Calculate Diffuse Lighting
            vec3 norm = normalize(Normal); // Normalize vectors to 1 unit
            vec3 lightDirection = normalize(keyLightPos - FragmentPos); // Calculate distance (light direction) between light source and fragments/pixels on
            float impact = max(dot(norm, lightDirection), 0.0); // Calculate diffuse impact by generating dot product of normal and light
            vec3 diffuse = impact * keyLightColor; // Generate diffuse light color

            // Calculate Specular lighting
            float specularIntensity = 0.1f; // Set specular light strength
            float highlightSize = 16.0f; // Set specular highlight size
            vec3 viewDir = normalize(viewPosition - FragmentPos); // Calculate view direction
            vec3 reflectDir = reflect(-lightDirection, norm); // Calculate reflection vector

            // Calculate specular component
            float specularComponent = pow(max(dot(viewDir, reflectDir), 0.0), highlightSize);
            vec3 specular = specularIntensity * specularComponent * lightColor;

            // Calculate phong result
            vec3 objectColor = texture(uTexture, mobileTextureCoordinate).xyz;
            vec3 phong = (ambient + diffuse) * objectColor + specular;
            pyramidColor = vec4(phong, 1.0f); // Send lighting results to GPU
        }
);

/* Lamp Shader Source Code */
const GLchar * lampVertexShaderSource = GLSL(330,

        layout (location = 0) in vec3 position; // VAP position 0 for vertex position data

        // Uniform / Global variables for the transform matrices
        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;

        void main()
        {
            gl_Position = projection * view * model * vec4(position, 1.0f); // Transforms vertices into clip coordinates
        }
);

/* Fragment Shader Source Code */
const GLchar * lampFragmentShaderSource = GLSL(330,

        out vec4 color; // For outgoing lamp color (smaller pyramid) to the GPU

        void main()
        {
            color = vec4(1.0f); // Set color to white (1.0f, 1.0f, 1.0f) with alpha 1.0

        }
);

/* Main Program */
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
                std::cout<< "Failed to initialize GLEW" << std::endl;
                return -1;
            }

    UCreateShader();

    UCreateBuffers();

    UGenerateTexture();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color

    glutDisplayFunc(URenderGraphics);

    glutMainLoop();

    // Destroys Buffer objects once used
    glDeleteVertexArrays(1, &PyramidVAO);
    glDeleteVertexArrays(1, &LightVAO);
    glDeleteVertexArrays(1, &keyLightVAO);
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

    GLint modelLoc, viewLoc, projLoc, uTextureLoc, lightColorLoc, lightPositionLoc, viewPositionLoc;
    GLint keyLightColorLoc, keyLightPositionLoc;

    glm::mat4 model;
    glm::mat4 keyModel;
    glm::mat4 view;
    glm::mat4 projection;

    /* Use the pyramid Shader to activate the pyramid Vertex Array Object for rendering and transforming */
    glUseProgram(pyramidShaderProgram);
    glBindVertexArray(PyramidVAO);

    // Transform the pyramid
    model = glm::translate(model, pyramidPosition);
    model = glm::scale(model, pyramidScale);

    // Transform the camera
    view = glm::translate(view, cameraPosition);
    view = glm::rotate(view, cameraRotation, glm::vec3(0.0f, 1.0f, 0.0f));

    // Set the camera projection to perspective
    projection = glm::perspective(45.0f,(GLfloat)WindowWidth / (GLfloat)WindowHeight, 0.1f, 100.0f);

    // Reference matrix uniforms from the pyramid Shader program
    modelLoc = glGetUniformLocation(pyramidShaderProgram, "model");
    viewLoc = glGetUniformLocation(pyramidShaderProgram, "view");
    projLoc = glGetUniformLocation(pyramidShaderProgram, "projection");

    // Pass matrix data to the pyramid Shader program's matrix uniforms
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Reference matrix uniforms from the pyramid Shader program for the pyramid color, light color, light position, and camera position
    uTextureLoc = glGetUniformLocation(pyramidShaderProgram, "uTexture");
    lightColorLoc = glGetUniformLocation(pyramidShaderProgram, "lightColor");
    keyLightColorLoc = glGetUniformLocation(pyramidShaderProgram, "keyLightColor");
    lightPositionLoc = glGetUniformLocation(pyramidShaderProgram, "lightPos");
    keyLightPositionLoc = glGetUniformLocation(pyramidShaderProgram, "keyLightPos");
    viewPositionLoc = glGetUniformLocation(pyramidShaderProgram, "viewPosition");

    // Pass texture, light, and camera data to the pyramid Shader programs corresponding uniforms
    glUniform1i(uTextureLoc, 0); // texture unit 0
    glUniform3f(lightColorLoc, lightColor.r, lightColor.g, lightColor.b);
    glUniform3f(keyLightColorLoc, keyLightColor.r, keyLightColor.g, keyLightColor.b);
    glUniform3f(lightPositionLoc, lightPosition.x, lightPosition.y, lightPosition.z);
    glUniform3f(keyLightPositionLoc, keyLightPosition.x, keyLightPosition.y, keyLightPosition.z);
    glUniform3f(viewPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glDrawArrays(GL_TRIANGLES, 0, 18); // Draw the primitives / pyramid

    glBindVertexArray(0); // Deactivate the Pyramid Vertex Array Object

    /* Use the Lamp Shader and activate the Lamp Vertex Array Object for rendering and transforming */
    glUseProgram(lampShaderProgram);
    glBindVertexArray(LightVAO);

    // Transform the smaller pyramid used as a visual cue for the light source
    model = glm::translate(model, lightPosition);
    model = glm::scale(model, lightScale);

    // Reference matrix uniforms from the Lamp Shader program
    modelLoc = glGetUniformLocation(lampShaderProgram, "model");
    viewLoc = glGetUniformLocation(lampShaderProgram, "view");
    projLoc = glGetUniformLocation(lampShaderProgram, "projection");

    // Pass matrix uniforms from the Lamp Shader Program
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Draws the triangles
    glDrawArrays(GL_TRIANGLES, 0, 18);

    glBindVertexArray(0); // Deactivate the Pyramid Vertex Array Object

	/* Use the Lamp Shader and activate the Lamp Vertex Array Object for rendering and transforming */
	glUseProgram(lampShaderProgram);
	glBindVertexArray(keyLightVAO);

	// Transform the smaller pyramid used as a visual cue for the light source
	keyModel = glm::translate(keyModel, keyLightPosition);
	keyModel = glm::scale(keyModel, lightScale);

	// Pass matrix uniforms from the Lamp Shader Program
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(keyModel));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	// Draws the triangles
	glDrawArrays(GL_TRIANGLES, 0, 18);

	glBindVertexArray(0); // Deactivate the Lamp Vertex Array Object

    glutPostRedisplay();
    glutSwapBuffers(); // Flips the back buffer with the front buffer every frame. Similar to GL Flush

}

/* Create the Shader program */
void UCreateShader()
{
    // Pyramid Vertex shader
    GLint pyramidVertexShader = glCreateShader(GL_VERTEX_SHADER); // Creates the Vertex shader
    glShaderSource(pyramidVertexShader, 1, &pyramidVertexShaderSource, NULL); // Attaches the Vertex shader to the source code
    glCompileShader(pyramidVertexShader); // Compiles the Vertex shader

    // Pyramid Fragment Shader
    GLint pyramidFragmentShader = glCreateShader(GL_FRAGMENT_SHADER); // Creates the Fragment Shader
    glShaderSource(pyramidFragmentShader, 1, &pyramidFragmentShaderSource, NULL); // Attaches the Fragment shader to the source code
    glCompileShader(pyramidFragmentShader); // Compiles the Fragment Shader

    // Pyramid Shader program
    pyramidShaderProgram = glCreateProgram(); // Creates the Shader program and returns an id
    glAttachShader(pyramidShaderProgram, pyramidVertexShader); // Attaches Vertex shader to the Shader program
    glAttachShader(pyramidShaderProgram, pyramidFragmentShader); // Attaches Fragment shader to the Shader program
    glLinkProgram(pyramidShaderProgram); // Link Vertex and Fragment shaders to the Shader program

    // Delete the Vertex and Fragment shaders once linked
    glDeleteShader(pyramidVertexShader);
    glDeleteShader(pyramidFragmentShader);

    // Lamp Vertex shader
    GLint lampVertexShader = glCreateShader(GL_VERTEX_SHADER); // Creates the Vertex shader
    glShaderSource(lampVertexShader, 1, &lampVertexShaderSource, NULL); // Attaches the Vertex shader to the source code
    glCompileShader(lampVertexShader); // Compiles the Vertex shader

    // Lamp Fragment shader
    GLint lampFragmentShader = glCreateShader(GL_FRAGMENT_SHADER); // Creates the Fragment shader
    glShaderSource(lampFragmentShader, 1, &lampFragmentShaderSource, NULL); // Attaches the Fragment shader to the source code
    glCompileShader(lampFragmentShader); // Compiles the Fragment shader

    // Lamp Shader Program
    lampShaderProgram = glCreateProgram(); // Creates the Shader program and returns an id
    glAttachShader(lampShaderProgram, lampVertexShader); // Attach Vertex shader to the Shader program
    glAttachShader(lampShaderProgram, lampFragmentShader); // Attach Fragment shader to the Shader program
    glLinkProgram(lampShaderProgram); // Link Vertex and Fragment shaders to the Shader program

    // Delete the lamp shaders once linked
    glDeleteShader(lampVertexShader);
    glDeleteShader(lampFragmentShader);

}

/* Creates the Buffer and Array Objects */
void UCreateBuffers()
{
    // Position and Texture coordinate data for 18 triangles
    GLfloat vertices[] = {

                        // Positions             // Normals             // Texture Coordinates

                        // Back Face             // Negative Z Normals
                         0.0f,  0.5f,  0.0f,     0.0f,  0.0f, -1.0f,    0.5f, 1.0f,
                         0.5f, -0.5f, -0.5f,     0.0f,  0.0f, -1.0f,    0.0f, 0.0f,
                        -0.5f, -0.5f, -0.5f,     0.0f,  0.0f, -1.0f,    1.0f, 0.0f,

                        // Front Face            // Positive Z Normals
                         0.0f,  0.5f,  0.0f,     0.0f,  0.0f,  1.0f,    0.5f, 1.0f,
                        -0.5f, -0.5f,  0.5f,     0.0f,  0.0f,  1.0f,    0.0f, 0.0f,
                         0.5f, -0.5f,  0.5f,     0.0f,  0.0f,  1.0f,    1.0f, 0.0f,

                         // Left Face            // Negative X Normals
                         0.0f,  0.5f,  0.0f,    -1.0f,  0.0f,  0.0f,    0.5f, 1.0f,
                        -0.5f, -0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    0.0f, 0.0f,
                        -0.5f, -0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    1.0f, 0.0f,

                         // Right Face           // Positive X Normals
                         0.0f,  0.5f,  0.0f,     1.0f,  0.0f,  0.0f,    0.5f, 1.0f,
                         0.5f, -0.5f,  0.5f,     1.0f,  0.0f,  0.0f,    0.0f, 0.0f,
                         0.5f, -0.5f, -0.5f,     1.0f,  0.0f,  0.0f,    1.0f, 0.0f,

                         // Bottom Face          // Negative Y Normals
                        -0.5f, -0.5f, -0.5f,     0.0f, -1.0f,  0.0f,    0.0f, 1.0f,
                         0.5f, -0.5f, -0.5f,     0.0f, -1.0f,  0.0f,    0.0f, 0.0f,
                        -0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,    1.0f, 1.0f,
                        -0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,    1.0f, 1.0f,
                         0.5f, -0.5f, -0.5f,     0.0f, -1.0f,  0.0f,    0.0f, 0.0f,
                         0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,    1.0f, 0.0f,

    };


    // Generate buffer ids
    glGenVertexArrays(1, &PyramidVAO);
    glGenBuffers(1, &VBO);

    // Activate the PyramidVAO before binding and setting VBOs and VAPs
    glBindVertexArray(PyramidVAO);

    // Activate the VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // Copy vertices to VBO

    // Set attribute pointer 0 to hold position data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0); // Enables vertex attribute

    //Set attribute pointer 1 to hold Normal data
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    // Set attribute pointer 2 to hold Texture coordinate data
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0); // Unbind the pyramid VAO

    // Generate buffer ids for lamp (smaller pyramid)
    glGenVertexArrays(1, &LightVAO); // Vertex Array for pyramid vertex copies to serve as light source

    // Activate the Vertex Array Object before binding and setting any VBOs and Vertex Attribute Pointers
    glBindVertexArray(LightVAO);

    // Referencing the same VBO for its vertices
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Set attribute pointer to 0 to hold Position data (used for the lamp)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    // Generate buffer ids for lamp (smaller pyramid)
	glGenVertexArrays(1, &keyLightVAO); // Vertex Array for pyramid vertex copies to serve as light source

	// Activate the Vertex Array Object before binding and setting any VBOs and Vertex Attribute Pointers
	glBindVertexArray(keyLightVAO);

	// Referencing the same VBO for its vertices
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Set attribute pointer to 0 to hold Position data (used for the lamp)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

}

/* Generate and load the texture */
void UGenerateTexture(){

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    int width, height;

    unsigned char* image = SOIL_load_image("wall.png", &width, &height, 0, SOIL_LOAD_RGB); // Loads texture file

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(image);
    glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture
}
