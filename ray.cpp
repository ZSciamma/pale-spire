#include "ray.h"

Ray::Ray(VEC3 o, VEC3 d, int recurse_depth) 
	: o(o), d(d), recurse_depth(recurse_depth)
{
	d.normalize();						// IS THIS NEEDED? REMOVE OTHER NORMALIZATIONS?
}
