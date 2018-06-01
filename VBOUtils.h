#ifndef VBOUTILS_H
#define VBOUTILS_H

#include "shaderprogram.h"

class VBOUtils {
 public:
  static GLuint makeBuffer(void *data, int vertexCount, int vertexSize);
  static void assignVBOtoAttribute(ShaderProgram *shaderProgram,const char* attributeName, GLuint bufVBO, int vertexSize);
};

#endif // VBOUTILS_H
