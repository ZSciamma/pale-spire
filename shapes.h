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

using namespace std;

class Material;

// Abstract class representing any shape in the world
class Shape {
public:
	VEC3 colour;
	const Material &material;	// The material in which to render the shape

	Shape(VEC3 colour, const Material &mat);

	// Returns the normal to the shape at that point
	//	Takes the ray which lands on the shape 
	virtual VEC3 getNormalAt(VEC3 point, const Ray &ray) const = 0;

	// Returns if the ray intersects with the shape
	//  Sets to be how far along the ray the shape intersects
	virtual bool intersects(const Ray &ray, float& t) const = 0;

	static VEC3 hadamard(VEC3 a, VEC3 b);
};

class Sphere : public Shape {
	// Calculates relevant roots for the intersection of this ray with the sphere
	vector<float> computeAllIntersectionRoots(const Ray &ray) const;

	// Keeps only the smallest root
	bool hasSmallestPositiveRoot(vector<float> roots, float& smallest) const;

public:
	VEC3 center;
	float radius;
	VEC3 colour;
	const Material &material;

	Sphere(VEC3 center, float radius, VEC3 colour, const Material &mat);
	VEC3 getNormalAt(VEC3 point, const Ray &ray) const;
	bool intersects(const Ray &ray, float &t) const;
};

class Triangle : public Shape {
	bool intersectsWithRay(const Ray &ray, float &t) const;

public:
	VEC3 a,b,c;
	VEC3 colour;
	const Material &material;

	Triangle(VEC3 a, VEC3 b, VEC3 c, VEC3 colour, const Material &mat);
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
	VEC3 colour;
	const Material &material;

	Cylinder(VEC3 center, float radius, float height, VEC3 up, VEC3 colour, const Material &material);
	VEC3 getNormalAt(VEC3 point, const Ray &ray) const;
	bool intersects(const Ray &ray, float &t) const;
};

#endif
