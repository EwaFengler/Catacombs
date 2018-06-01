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

#include "model.h"

namespace Models {
  void Model::drawWire() {
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    drawSolid();

    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
  }

  void Model::prepareModel(ShaderProgram* shaderProgram, GLuint &vao) {

    //Build VBO buffers with object data
    bufVertices=VBOUtils::makeBuffer(vertices, vertexCount, sizeof(float)*4);
    bufNormals=VBOUtils::makeBuffer(normals, vertexCount, sizeof(float)*4);
    bufTexCoords=VBOUtils::makeBuffer(texCoords, vertexCount, sizeof(float)*2);
  }

  void Model::loadToShader(ShaderProgram* shaderProgram, GLuint &vao){
    //Create VAO which associates VBO with attributes in shading program
    glGenVertexArrays(1,&vao); //Generate VAO handle and store it in the global variable

    glBindVertexArray(vao); //Activate newly created VAO

    VBOUtils::assignVBOtoAttribute(shaderProgram,"vertex",bufVertices,4); //"vertex" refers to the declaration "in vec4 vertex;" in vertex shader
    VBOUtils::assignVBOtoAttribute(shaderProgram,"normal",bufNormals,4);
    VBOUtils::assignVBOtoAttribute(shaderProgram,"texCoord0",bufTexCoords,2);

    glBindVertexArray(0); //Deactivate VAO
  }

  void Model::drawModel(GLuint vao, ShaderProgram *shaderProgram, glm::mat4 mP, glm::mat4 mV, glm::mat4 mM){
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

  void Model::deleteVBOs(){
    glDeleteBuffers(1,&bufVertices);
    glDeleteBuffers(1,&bufNormals);
    glDeleteBuffers(1,&bufTexCoords);
  }

  void Model::deleteTexture(){
    glDeleteTextures(1,&tex0);
    glDeleteTextures(1,&tex1);
  }
}
