// The Shader class is responsible for calculating the colour
//	of any point on a sphere, using the lighting effects chosen

#ifndef _SHADER_H
#define _SHADER_H

#include "shapes.h"
#include "physicsWorld.h"

struct Light {
	VEC3 pos, colour;
};

class Shader {
	const vector<const Light> &lights;	// List of all the lights in the scene
	const PhysicsWorld &world;	// Ohysics engine handling collisions between rays and shapes
	VEC3 eye;

	// Calculates the Phong shading for a single source
	VEC3 calculateSourcePhongShading(VEC3 point, const Light &light, const Shape *shape, VEC3 normal, VEC3 eyeDir) const;
	// Returns true if a point is blocked from the light
	bool isOccludedFromLight(VEC3 point, const Light &light) const;

public:
	Shader(const vector<const Light> &lights, const PhysicsWorld &world, VEC3 eye);

	// Calculate the colour at the point given on the shape
	//	Inputs the ray that lands on that point
	VEC3 calculateShading(VEC3 point, const Shape *shape, const Ray &ray) const;
};
#endif
