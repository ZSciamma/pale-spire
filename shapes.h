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

// Abstract class representing any shape in the world
class Shape {
public:
	VEC3 colour;

	Shape(VEC3 colour);

	// Returns the normal to the shape at that point
	virtual VEC3 getNormalAt(VEC3 point) const = 0;

	// Returns if the ray intersects with the shape
	//  Sets to be how far along the ray the shape intersects
	virtual bool intersects(const Ray &ray, float& t) const = 0;
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

	Sphere(VEC3 center, float radius, VEC3 colour);
	VEC3 getNormalAt(VEC3 point) const;
	bool intersects(const Ray &ray, float& t) const;
};

class Triangle : public Shape {
	bool intersectsWithRay(const Ray &ray, float& t) const;

public:
	VEC3 a,b,c;
	VEC3 colour;

	Triangle(VEC3 a, VEC3 b, VEC3 c, VEC3 colour);
	VEC3 getNormalAt(VEC3 point) const;
	bool intersects(const Ray &ray, float& t) const;
};

#endif
