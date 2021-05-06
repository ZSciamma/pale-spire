// This file defines all the shapes that can be used in the world
//	Defines spheres and triangles
//	For each shape, defines how to compute whether a ray intersects
//		with it, and how to get the normal at a certain point.

#ifndef _SHAPES_H
#define _SHAPES_H

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <float.h>
#include "SETTINGS.h"

#include "ray.h"
#include "texture.h"

using namespace std;

class Material;

// Abstract class representing any shape in the world
class Shape {
public:
	const Material &material;	// The material in which to render the shape
	VEC3 baseColour;
	Texture *texture = NULL;

	Shape(const Material &mat, VEC3 colour);
	Shape(const Material &mat, Texture *texture);

	// Gets the component-wise product of two vectors
	static VEC3 hadamard(VEC3 a, VEC3 b);

	// Returns the normal to the shape at that point
	//	Takes the ray which lands on the shape 
	virtual VEC3 getNormalAt(VEC3 point, const Ray &ray) const = 0;

	// Returns if the ray intersects with the shape
	//  Sets to be how far along the ray the shape intersects
	virtual bool intersects(const Ray &ray, float& t) const = 0;

	// Get the colour at that point on the shape
	//	Gets the appropriate colour from the texture,
	//	or the base colour of the shape if no texture
	VEC3 getColourAt(VEC3 point) const;
};

class Sphere : public Shape {
	// Calculates relevant roots for the intersection of this ray with the sphere
	vector<float> computeAllIntersectionRoots(const Ray &ray) const;

	// Keeps only the smallest root
	bool hasSmallestPositiveRoot(vector<float> roots, float& smallest) const;

public:
	VEC3 center;
	float radius;
	const Material &material;
	VEC3 baseColour;
	Texture *texture = NULL;

	Sphere(VEC3 center, float radius, const Material &mat, VEC3 colour);
	Sphere(VEC3 center, float radius, const Material &mat, Texture *texture);
	
	VEC3 getNormalAt(VEC3 point, const Ray &ray) const;
	bool intersects(const Ray &ray, float &t) const;
};

class Triangle : public Shape {
	bool intersectsWithRay(const Ray &ray, float &t) const;

public:
	VEC3 a,b,c;
	const Material &material;
	VEC3 baseColour;
	Texture *texture = NULL;

	Triangle(VEC3 a, VEC3 b, VEC3 c, const Material &mat, VEC3 colour);
	Triangle(VEC3 a, VEC3 b, VEC3 c, const Material &mat, Texture *texture);

	VEC3 getNormalAt(VEC3 point, const Ray &ray) const;
	bool intersects(const Ray &ray, float &t) const;
};

class Cylinder : public Shape {
private:
	VEC3 u, v, w;	// Basis vectors for calculating cylinder interesections
	MATRIX3 globalToLocal, localToGlobal;	// Matrices for rotating a point between global and local space
	void create_basis_vectors(VEC3 up);
	void initialise_rotation_matrix();

	// Transforms point to local coordinates, relative to the cylinder
	//	centered at the origin and pointing up the y axis
	VEC3 transformToLocal(VEC3 point) const;
	// Transforms point back to global coordinates
	VEC3 transformToGlobal(VEC3 point) const;

public:
	VEC3 center;	// The center of the base circle, halfway up the cylinder
	float radius, height;
	const Material &material;
	VEC3 baseColour;
	Texture *texture = NULL;

	Cylinder(VEC3 center, float radius, float height, VEC3 up, const Material &material, VEC3 colour);
	Cylinder(VEC3 center, float radius, float height, VEC3 up, const Material &material, Texture *texture);

	VEC3 getNormalAt(VEC3 point, const Ray &ray) const;
	bool intersects(const Ray &ray, float &t) const;
};

#endif
