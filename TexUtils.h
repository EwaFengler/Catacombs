#ifndef TEXUTILS_H
#define TEXUTILS_H

#include <GL/glew.h>
#include <vector>
#include "lodepng.h"

class TexUtils
{
  public:
    static GLuint readTexture(char* filename);
};

#endif // TEXUTILS_H
