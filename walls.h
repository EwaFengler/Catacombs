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

#ifndef WALLS_H
#define WALLS_H

//Cube model made out of triangles
//Contains arrays:
//vertices - vertex positions in homogenous coordinates
//normals - vertex normals in homogenous coordinates
//texCoords - texturing coordinates
//colors - vertex colors (rgba)
//Culling GL_CW
//TBN friendly

#include "model.h"

namespace Models {
	namespace WallsInternal {
		extern float vertices[];
		extern float normals[];
		extern float texCoords[];
		extern unsigned int vertexCount;
	}

	class Walls: public Model {
		public:
			Walls();
			virtual ~Walls();
			virtual void readTexture();
	};

	extern Walls walls;
}




#endif
