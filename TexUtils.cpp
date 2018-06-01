#include "TexUtils.h"

GLuint TexUtils::readTexture(char* filename) {
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
