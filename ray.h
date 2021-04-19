// This file defines a ray, which has a position and direction

#ifndef _RAY_H
#define _RAY_H

#include <vector>
#include "SETTINGS.h"

class Ray {                                                              // IS THIS INEFFICIENT?
public:
	VEC3 o, d;  // Store origin and direction
	int recurse_depth;

	Ray(VEC3 o, VEC3 d, int recurse_depth = 10);
};

#endif
