// This class is responsible for handling intersections between rays
//	and the world.

#ifndef _PHYSICSWORLD_H
#define _PHYSICSWORLD_H

#include "shapes.h"

class PhysicsWorld {
	const vector<const Shape *> &shapes;	// The shapes defining the entire world

public:
	PhysicsWorld(const vector<const Shape*> &shapes);

	// Returns true if the ray intersects with a shape in the world
	// 	Sets intersectShape to the closest shape with which the ray intersects
	//	Sets point to the point at which the ray hits the shape
	bool existsClosestIntersection(const Ray &ray, const Shape *&intersectShape, VEC3 &point) const;
};

#endif