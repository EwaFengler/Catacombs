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

#ifndef MODEL_H
#define MODEL_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include "constants.h"

#include "shaderprogram.h"

#include "VBOUtils.h"
#include "TexUtils.h"

namespace Models {

class Model {
 public:
  GLuint bufVertices;
  GLuint bufNormals;
  GLuint bufTexCoords;

  GLuint tex0;
  GLuint tex1;

  int vertexCount;
  float *vertices;
  float *normals;
  float *texCoords;

  virtual void readTexture()=0;

  virtual void makeBuffers(ShaderProgram* shaderProgram, GLuint &vao);
  virtual void loadToShader(ShaderProgram* shaderProgram, GLuint &vao);
  virtual void drawModel(GLuint vao, ShaderProgram *shaderProgram, glm::mat4 mP, glm::mat4 mV, glm::mat4 mM);

  virtual void deleteVBOs();
  virtual void deleteTexture();

};
}

#endif
