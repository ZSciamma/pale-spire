// A Material is the material which defines the coefficients for
//	interaction with light.

#ifndef _MATERIAL__H
#define _MATERIAL__H

#include "shapes.h"
#include "light.h"

class Material {
public:

	Material();

	// Calculates the colour at this point using the material's specific lighting model
	//	point, normal: the point on the surface of the shape, and normal at that point
	virtual VEC3 calculateShading(const Shape *shape, VEC3 point, VEC3 normal, const Light &light, VEC3 eyeDir) const = 0;
};

// Uses Phong to look like a plastic
class Plastic: public Material {
public:
	// coefficient affecting the size of the specular highlight
	float cPhong;

	Plastic(float cPhong);

	// Uses Cook-Torrance, from Professor Kim's BDRFs code (see material.cpp)
	VEC3 calculateShading(const Shape *shape, VEC3 point, VEC3 normal, const Light &light, VEC3 eyeDir) const;
};

// Uses Cook-Torrance to look like a metal
class Metal: public Material {
public:
	// gaussian coefficient: size of the specular highlight
	float cGaussian;
	// reflection coefficient
	float cReflection;

	Metal(float cGaussian, float cReflection);

	// Uses Cook-Torrance, from Professor Kim's BDRFs code (see material.cpp)
	VEC3 calculateShading(const Shape *shape, VEC3 point, VEC3 normal, const Light &light, VEC3 eyeDir) const;
};

#endif