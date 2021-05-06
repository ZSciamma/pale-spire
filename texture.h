// A texture is any image, either imported or generated,
//	which will mapped onto a shape's surface

#ifndef _TEXTURE__H
#define _TEXTURE__H

using namespace std;

class Texture {
public:
	// Defines a texture from a ppm file
	Texture(const string& filename);

	// Gets the texture colour at tex_coords (u, v)
	//	u, v are between 0 and 1
	VEC3 texture_coords(float u, float v);
};

#endif