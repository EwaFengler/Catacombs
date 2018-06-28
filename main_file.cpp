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

glm::vec3 position = glm::vec3( 0, 1.5, 0 );
float positionY = 1.5;

short currentTriangle = 1769;
bool cheatMode = false;
//---------------------------------


//Shader program object
ShaderProgram *shaderProgram;

GLuint vao;

//Error handling procedure
void error_callback(int error, const char* description) {
  fputs(description, stderr);
}

//Key event processing procedure
void key_callback(GLFWwindow* window, int key,
                  int scancode, int action, int mods) {
  if (action == GLFW_PRESS) {
    if (key == GLFW_KEY_LEFT_SHIFT) {
      shift_speed = 3;
      speed_x = speed_x *shift_speed;
      speed_y = speed_y * shift_speed;
    }
    if (key == GLFW_KEY_A)
      speed_y = -3.14 * shift_speed;
    if (key == GLFW_KEY_D)
      speed_y = 3.14 * shift_speed;
    if (key == GLFW_KEY_W)
      speed_x = -3.14 * shift_speed;
    if (key == GLFW_KEY_S)
      speed_x = 3.14 * shift_speed;
    if (key == GLFW_KEY_ESCAPE)
      glfwSetWindowShouldClose(window, GLFW_TRUE);
    if (key == GLFW_KEY_LEFT_CONTROL)
      cheatMode = true;
  }


  if (action == GLFW_RELEASE) {
    if (key == GLFW_KEY_A)
      speed_y = 0;
    if (key == GLFW_KEY_D)
      speed_y = 0;
    if (key == GLFW_KEY_W)
      speed_x = 0;
    if (key == GLFW_KEY_S)
      speed_x = 0;
    if (key == GLFW_KEY_LEFT_SHIFT) {
      speed_x = speed_x / shift_speed;
      speed_y = speed_y / shift_speed;
      shift_speed = 1;

    }
    if (key == GLFW_KEY_LEFT_CONTROL)
      cheatMode = false;
  }
}

float isOnTriangle(short triangleNum, glm::vec3 position){

  float Ax = Models::FloorsInternal::vertices[triangleNum*3*4];
  float Az = Models::FloorsInternal::vertices[triangleNum*3*4+2];

  float Bx = Models::FloorsInternal::vertices[triangleNum*3*4+4];
  float Bz = Models::FloorsInternal::vertices[triangleNum*3*4+6];

  float Cx = Models::FloorsInternal::vertices[triangleNum*3*4+8];
  float Cz = Models::FloorsInternal::vertices[triangleNum*3*4+10];

  glm::vec3 AB = glm::vec3(Bx-Ax, Bz-Az, 0);
  glm::vec3 AC = glm::vec3(Cx-Ax, Cz-Az, 0);
  glm::vec3 BC = glm::vec3(Cx-Bx, Cz-Bz ,0);
  glm::vec3 AP = glm::vec3(position.x-Ax, position.z-Az,0);
  glm::vec3 BP = glm::vec3(position.x-Bx, position.z-Bz,0);
  glm::vec3 CP = glm::vec3(position.x-Cx, position.z-Cz,0);

  float areaABC = length(cross(AB, AC));
  float areaABP = length(cross(AB, AP));
  float areaACP = length(cross(AC, CP));
  float areaBCP = length(cross(BC, BP));

  return areaABC - max(areaABP + areaACP, max(areaABP + areaBCP, areaBCP + areaACP));
}

//Initialization procedure
void initOpenGLProgram(GLFWwindow* window) {
  //************Insert initialization code here************
  glClearColor(0, 0, 0, 1); //Clear the screen to black
  glEnable(GL_DEPTH_TEST); //Turn on Z-Buffer
  glfwSetKeyCallback(window, key_callback); //Register key event processing procedure

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);//mouse init

  shaderProgram=new ShaderProgram("vshader.vert",NULL,"fshader.frag"); //Read, compile and link the shader program

  Models::walls.readTexture();
  Models::floors.readTexture();
  Models::tomb.readTexture();

  Models::walls.makeBuffers(shaderProgram, vao);
  Models::floors.makeBuffers(shaderProgram, vao);
  Models::tomb.makeBuffers(shaderProgram, vao);
}

//Freeing of resources
void freeOpenGLProgram() {
  delete shaderProgram; //Delete shader program

  glDeleteVertexArrays(1,&vao); //Delete VAO

  Models::walls.deleteVBOs();
  Models::floors.deleteVBOs();
  Models::tomb.deleteVBOs();

  Models::walls.deleteTexture();
  Models::floors.deleteTexture();
  Models::tomb.deleteTexture();
}

//Procedure which draws the scene
void drawScene(GLFWwindow* window, float angle_x, float angle_y, float deltaTime) {

  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT); //Clear color and depth buffers

  double xpos, ypos;//mouse position on the screen
  glfwGetCursorPos(window,&xpos, &ypos);
  horizontalAngle += mouseSpeed * deltaTime * float(WindowLenght/2 - xpos )*1.0e4;
  verticalAngle   += mouseSpeed * deltaTime * float(WindowHeight/2 - ypos )*1.0e4;
  glfwSetCursorPos(window,WindowLenght/2, WindowHeight/2);//setting Cursor Position in the middle of the screen

  glm::vec3 direction (
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

  glm::vec3 newPosition = position - direction * angle_x;
  newPosition += right * angle_y;

  float multiplier_x = 0.15f * ((angle_x > 0) - (angle_x < 0));
  float multiplier_y = 0.15f * ((angle_y > 0) - (angle_y < 0));

  glm::vec3 positionToCheck = newPosition - direction * multiplier_x;
  positionToCheck += right * multiplier_y;

  float res = isOnTriangle(currentTriangle, positionToCheck);

  if(res < 0){
    for(int j = 0; j < 1858; j++){
      float newRes = isOnTriangle(j, positionToCheck);

        if(newRes > res){
          currentTriangle = j;
          res = newRes;
        }
    }
  }

  newPosition.y = Models::FloorsInternal::vertices[currentTriangle*3*4+1] + 1.2;

  if(cheatMode || (res >= 0 && abs(newPosition.y - position.y) < 0.4)){
    position = newPosition;
  }

  glm::mat4 P = glm::perspective(40 * PI/180, aspect, 0.05f, 10.0f); //Compute projection matrix

  glm::mat4 V = glm::lookAt(
                  position,           // Camera is here
                  position+direction, // and looks here : at the same position, plus "direction"
                  up);                  // Head is up (set to 0,-1,0 to look upside-down)

  //Compute model matrix
  glm::mat4 M = glm::mat4(1.0f);
  //passing light position to shaders
  glUniform4f(shaderProgram->getUniformLocation("lightPosition"), position.x - direction.x, position.y - direction.y, position.z - direction.z, 1.0);

  Models::walls.loadToShader(shaderProgram,vao);
  Models::walls.drawModel(vao,shaderProgram,P,V,M);
  Models::floors.loadToShader(shaderProgram,vao);
  Models::floors.drawModel(vao,shaderProgram,P,V,M);

  Models::tomb.loadToShader(shaderProgram,vao);
  //drawing all tombs
  for(int i = 0; i < 372; i+=4){
    glm::mat4 Mt = M;
    Mt = translate(Mt, glm::vec3(Models::TombInternal::positions[i],
                                 Models::TombInternal::positions[i+1],
                                 Models::TombInternal::positions[i+2]));
    Mt = rotate(Mt, -Models::TombInternal::positions[i+3], glm::vec3(0.0f, 1.0f, 0.0f));
    Models::tomb.drawModel(vao,shaderProgram,P,V,Mt);
  }

  //Swap front and back buffers
  glfwSwapBuffers(window);
}



int main(void) {
  GLFWwindow* window; //Pointer to window object

  glfwSetErrorCallback(error_callback);//Register error callback procedure

  if (!glfwInit()) { //Initialize GLFW procedure
    fprintf(stderr, "Can't initialize GLFW GLFW.\n");
    exit(EXIT_FAILURE);
  }

  window = glfwCreateWindow(WindowLenght, WindowHeight, "Catacombs", NULL, NULL);

  if (!window) { //If window could not be created, then end the program
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
  while (!glfwWindowShouldClose(window)) { //As long as window shouldnt be closed...
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
