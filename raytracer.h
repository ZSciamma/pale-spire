// This file is responsible for defining the raytracing engine
// The raytracing engine creates rays, shoots them at the scene,
//	and determines their colour	

#ifndef _RAYTRACER_H
#define _RAYTRACER_H

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <iostream>
#include <float.h>

#include "SETTINGS.h"
#include "ray.h"
#include "shapes.h"
#include "shader.h"
#include "physicsWorld.h"

using namespace std;

class Camera {
public:
	float xRes, yRes;
	VEC3 eye, lookingAt, up;
	float nearPlane, fovy;

	float screenLeft, screenRight, screenBot, screenTop;	// (0, 0) is the center of the screen

	Camera(float xRes, float yRes, VEC3 eye, VEC3 lookingAt, VEC3 up, float nearPlane, float fovy);
};

/*
class Shader {
public:
	// Calculate the colour at the point given on the shape
	//	Inputs the ray that lands on that point
	VEC3 calculateShading(VEC3 point, const Shape *shape, const Ray &ray) const;
};
*/

// Generates rays through each pixel of the screen
//	Interacts with the Camera to generate rays relative to the world
//	Passes rays into the Shader to colour them with certain effects
class RayTracer {
	float top, bot, right, left;  // Real-world viewing plane boundaries (i.e. the place the camera is looking at)
	VEC3 u, v, w;                                                      // SHOULD THESE BE REFS?
	Camera &camera;  // Camera object viewing the world
	Shader &shader;
	PhysicsWorld &world;	// Object that computes intersections of rays and shapes

	// Computes and sets the dimensions of the real world viewing plane
	void initialise_viewing_plane_dimensions();

	// Creates the basis vectors that define the camera frame
	void initialise_camera_frame();

public:
	RayTracer(Camera &camera, Shader &shader, PhysicsWorld &world);

	// Generate the ray that goes through this pixel, using perspective projection
	//  Coord (0, 0) is in the center
	Ray generateAtCoord(float x, float y) const;

	// Calculates the colour of the ray
	// Determines where the ray interesects the scene and computes
	// 	the colour at that point
	VEC3 calculateColour(const Ray &ray) const;
};

#endif
