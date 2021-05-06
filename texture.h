// A texture is any image, either imported or generated,
//	which will mapped onto a shape's surface

#ifndef _TEXTURE__H
#define _TEXTURE__H

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <float.h>
#include "SETTINGS.h"

using namespace std;

class Texture {
public:
	int xRes, yRes;
	float *pixelValues;	// The pixels in the image, one component at a time (r, g, b)

	// Defines a texture from a ppm file
	//	Takes the name of the file and its resolution
	Texture(const string& filename, int xRes, int yRes);

	// Gets the texture colour at tex_coords (u, v)
	//	u, v are between 0 and 1
	VEC3 texture_lookup(float u, float v) const;
};

#endif