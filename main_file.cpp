/*
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#define GLM_FORCE_RADIANS

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include "constants.h"
#include "allmodels.h"
#include "lodepng.h"
#include "shaderprogram.h"

using namespace glm;

float speed_x = 0; // [radians/s]
float speed_y = 0; // [radians/s]

float shift_speed = 1;

float aspect=1; //Ratio of width to height
//-----------------------------
int WindowLenght = 1024;//szerokosc okna
int WindowHeight = 768;//wysokosc okna

float horizontalAngle = 3.14f;// horizontal angle : toward -Z
float verticalAngle = 0.0f;// vertical angle : 0, look at the horizon
float mouseSpeed = 0.5f;

glm::vec3 position = glm::vec3( 0, 0, 5 );
//---------------------------------


//Shader program object
ShaderProgram *shaderProgram;

//VAO and VBO handles
GLuint vao;
GLuint bufVertices; //handle for VBO buffer which stores vertex coordinates
GLuint bufColors;  //handle for VBO buffer which stores vertex colors
GLuint bufNormals; //handle for VBO buffer which stores vertex normals
GLuint bufTexCoords; //handle for VBO buffer which stores texturing coordinates

GLuint tex0;
GLuint tex1;

//Cube
float* vertices=Models::WallsInternal::vertices;
float* colors=Models::WallsInternal::colors;
float* normals=Models::WallsInternal::normals;
float* texCoords=Models::WallsInternal::texCoords;
int vertexCount=Models::WallsInternal::vertexCount;

//Error handling procedure
void error_callback(int error, const char* description) {
	fputs(description, stderr);
}

//Key event processing procedure
void key_callback(GLFWwindow* window, int key,
	int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_LEFT_SHIFT){
            shift_speed = 3;
            speed_x = speed_x *shift_speed;
            speed_y = speed_y * shift_speed;
        }
		if (key == GLFW_KEY_A)  speed_y = -3.14 * shift_speed;
		if (key == GLFW_KEY_D)  speed_y = 3.14 * shift_speed;
		if (key == GLFW_KEY_W)   speed_x = -3.14 * shift_speed;
		if (key == GLFW_KEY_S)  speed_x = 3.14 * shift_speed;
        if (key == GLFW_KEY_ESCAPE)  glfwSetWindowShouldClose(window, GLFW_TRUE);
	}


	if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_A) speed_y = 0;
		if (key == GLFW_KEY_D) speed_y = 0;
		if (key == GLFW_KEY_W) speed_x = 0;
		if (key == GLFW_KEY_S) speed_x = 0;
		if (key == GLFW_KEY_LEFT_SHIFT){
            speed_x = speed_x / shift_speed;
            speed_y = speed_y / shift_speed;
            shift_speed = 1;

		}
	}
}

//Creates a VBO buffer from an array
GLuint makeBuffer(void *data, int vertexCount, int vertexSize) {
	GLuint handle;

	glGenBuffers(1,&handle);//Generate handle for VBO buffer
	glBindBuffer(GL_ARRAY_BUFFER,handle);  //Active the handle
	glBufferData(GL_ARRAY_BUFFER, vertexCount*vertexSize, data, GL_STATIC_DRAW);//Copy the array to VBO

	return handle;
}

//Assigns VBO buffer handle to an attribute of a given name
void assignVBOtoAttribute(ShaderProgram *shaderProgram,const char* attributeName, GLuint bufVBO, int vertexSize) {
	GLuint location=shaderProgram->getAttribLocation(attributeName); //Get slot number for the attribute
	glBindBuffer(GL_ARRAY_BUFFER,bufVBO);  //Activate VBO handle
	glEnableVertexAttribArray(location); //Turn on using of an attribute of a number passed as an argument
	glVertexAttribPointer(location,vertexSize,GL_FLOAT, GL_FALSE, 0, NULL); //Data for the slot should be taken from the current VBO buffer
}


//Preparation for drawing of a single object
void prepareObject(ShaderProgram* shaderProgram) {
	//Build VBO buffers with object data
	bufVertices=makeBuffer(vertices, vertexCount, sizeof(float)*4); //VBO with vertex coordinates
	bufColors=makeBuffer(colors, vertexCount, sizeof(float)*4);//VBO with vertes colors
	bufNormals=makeBuffer(normals, vertexCount, sizeof(float)*4);//VBO with vertex normals
    bufTexCoords=makeBuffer(texCoords, vertexCount, sizeof(float)*2); //VBO with texturing coordinates

	//Create VAO which associates VBO with attributes in shading program
	glGenVertexArrays(1,&vao); //Generate VAO handle and store it in the global variable

	glBindVertexArray(vao); //Activate newly created VAO

	assignVBOtoAttribute(shaderProgram,"vertex",bufVertices,4); //"vertex" refers to the declaration "in vec4 vertex;" in vertex shader
	assignVBOtoAttribute(shaderProgram,"color",bufColors,4); //"color" refers to the declaration "in vec4 color;" in vertex shader
	assignVBOtoAttribute(shaderProgram,"normal",bufNormals,4); //"normal" refers to the declaration "in vec4 normal;" w vertex shader
    assignVBOtoAttribute(shaderProgram,"texCoord0",bufTexCoords,2); //"texCoord0" refers to the declaration "in vec2 texCoord0;" w vertex shader

	glBindVertexArray(0); //Deactivate VAO
}

GLuint readTexture(char* filename) {
 GLuint tex;
 glActiveTexture(GL_TEXTURE0);

 //Load into computer's memory
 std::vector<unsigned char> image;   //Allocate a vector for image storage
 unsigned width, height;   //Variables for image size
 //Read image
 unsigned error = lodepng::decode(image, width, height, filename);

 //Import into graphics card's memory
 glGenTextures(1,&tex); //Initialize one handle
 glBindTexture(GL_TEXTURE_2D, tex); //Activate the handle
 //Import image into graphics card's memory associated with the handle
 glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0,
   GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*) image.data());

 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);

 return tex;
}

//Initialization procedure
void initOpenGLProgram(GLFWwindow* window) {
	//************Insert initialization code here************
	glClearColor(0, 0, 0, 1); //Clear the screen to black
	glEnable(GL_DEPTH_TEST); //Turn on Z-Buffer
	glfwSetKeyCallback(window, key_callback); //Register key event processing procedure

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);//mouse init

	shaderProgram=new ShaderProgram("vshader.vert",NULL,"fshader.frag"); //Read, compile and link the shader program

	tex0=readTexture("metal.png");
	tex1=readTexture("metal_spec.png");

	prepareObject(shaderProgram);

}

//Freeing of resources
void freeOpenGLProgram() {
	delete shaderProgram; //Delete shader program

	glDeleteVertexArrays(1,&vao); //Delete VAO
	//Delete VBOs
	glDeleteBuffers(1,&bufVertices);
	glDeleteBuffers(1,&bufColors);
	glDeleteBuffers(1,&bufNormals);
    glDeleteBuffers(1,&bufTexCoords);
    //Delete textures
	glDeleteTextures(1,&tex0);
	glDeleteTextures(1,&tex1);

}

void drawObject(GLuint vao, ShaderProgram *shaderProgram, mat4 mP, mat4 mV, mat4 mM) {
	//Turn on the shading program that will be used for drawing.
	//While in this program it would be perfectly correct to execute this once in the initOpenGLProgram,
	//in most cases more than one shading program is used and hence, it should be switched between drawing of objects
	//while we render a single scene.
	shaderProgram->use();

	//Set uniform variables P,V and M in the vertex shader by assigning the appropriate matrices
	//In the lines below, expression:
	//  shaderProgram->getUniformLocation("P")
	//Retrieves the slot number corresponding to a uniform variable of a given name.
	//WARNING! "P" in the instruction above corresponds to the declaration "uniform mat4 P;" in the vertex shader,
	//while mP in glm::value_ptr(mP) corresponds to the argument "mat4 mP;" in THIS file.
	//The whole line below copies data from variable mP to the uniform variable P in the vertex shader. The rest of the instructions work similarly.
	glUniformMatrix4fv(shaderProgram->getUniformLocation("P"),1, false, glm::value_ptr(mP));
	glUniformMatrix4fv(shaderProgram->getUniformLocation("V"),1, false, glm::value_ptr(mV));
	glUniformMatrix4fv(shaderProgram->getUniformLocation("M"),1, false, glm::value_ptr(mM));
    glUniform1i(shaderProgram->getUniformLocation("textureMap0"),0); //Bind textureMap0 in fragment shader with texturing unit 0
	glUniform1i(shaderProgram->getUniformLocation("textureMap1"),1); //Bind textureMap1 in fragment shader with texturing unit 1

	//Bind texture from tex0 with 0th texturing unit
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,tex0);
	//Bind texture from tex1 with 1st texturing unit
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D,tex1);

	//Activation of VAO and therefore making all associations of VBOs and attributes current
	glBindVertexArray(vao);

	//Drawing of an object
	glDrawArrays(GL_TRIANGLES,0,vertexCount);

	//Tidying up after ourselves (not needed if we use VAO for every object)
	glBindVertexArray(0);
}

//Procedure which draws the scene
void drawScene(GLFWwindow* window, float angle_x, float angle_y, float deltaTime) {
	//************Place the drawing code here******************l

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT); //Clear color and depth buffers



    double xpos, ypos;//mouse position on the screen
    glfwGetCursorPos(window,&xpos, &ypos);
    horizontalAngle += mouseSpeed * deltaTime * float(WindowLenght/2 - xpos )*1.0e4;
    verticalAngle   += mouseSpeed * deltaTime * float(WindowHeight/2 - ypos )*1.0e4;
    glfwSetCursorPos(window,WindowLenght/2, WindowHeight/2);//setting Cursor Position in the middle of the screen

    glm::vec3 direction(
        cos(verticalAngle) * sin(horizontalAngle),
        sin(verticalAngle),
        cos(verticalAngle) * cos(horizontalAngle)
    );

    glm::vec3 right = glm::vec3(
        sin(horizontalAngle - 3.14f/2.0f),
        0,
        cos(horizontalAngle - 3.14f/2.0f)
    );
    glm::vec3 up = glm::cross( right, direction );
    //glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    position -= direction * angle_x;
    position += right * angle_y;

	glm::mat4 P = glm::perspective(40 * PI/180, aspect, 1.0f, 50.0f); //Compute projection matrix

	glm::mat4 V = glm::lookAt(
        position,           // Camera is here
        position+direction, // and looks here : at the same position, plus "direction"
        up);                  // Head is up (set to 0,-1,0 to look upside-down)


	//Compute model matrix
	glm::mat4 M = glm::mat4(1.0f);

	drawObject(vao,shaderProgram,P,V,M);

	//Swap front and back buffers
	glfwSwapBuffers(window);

}



int main(void)
{
	GLFWwindow* window; //Pointer to window object

	glfwSetErrorCallback(error_callback);//Register error callback procedure

	if (!glfwInit()) { //Initialize GLFW procedure
		fprintf(stderr, "Can't initialize GLFW GLFW.\n");
		exit(EXIT_FAILURE);
	}

	window = glfwCreateWindow(WindowLenght, WindowHeight, "OpenGL", NULL, NULL);  //Create 500x500 window with "OpenGL" as well as OpenGL context.

	if (!window) //If window could not be created, then end the program
	{
		fprintf(stderr, "Can't create window.\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window); //Since this moment, the window context is active and OpenGL commands will work with it.
	glfwSwapInterval(1); //Synchronize with the first VBLANK signal

	if (glewInit() != GLEW_OK) { //Initialize GLEW library
		fprintf(stderr, "Can't initialize GLEW.\n");
		exit(EXIT_FAILURE);
	}

	initOpenGLProgram(window); //Initialization procedure

	float angle_x = 0; //Object rotation angle
	float angle_y = 0; //Object rotation angle

	glfwSetTime(0); //Zero time counter

	//Main loop
	while (!glfwWindowShouldClose(window)) //As long as window shouldnt be closed...
	{
		angle_x = speed_x*glfwGetTime(); //Increase angle by the angle speed times the time passed since the previous frame
		angle_y = speed_y*glfwGetTime(); //Increase angle by the angle speed times the time passed since the previous frame
		glfwSetTime(0); //Zero time counter
		float deltaTime = glfwGetTime();
        drawScene(window,angle_x,angle_y, deltaTime); //Execute drawing procedure
		glfwPollEvents(); //Execute callback procedures which process events
	}

	freeOpenGLProgram(); //Free resources

	glfwDestroyWindow(window); //Delete OpenGL context and window
	glfwTerminate(); //Free GLFW resources
	exit(EXIT_SUCCESS);
}
