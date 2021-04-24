

#include "shader.h"

/*
VEC3 Shader::calculateShading(VEC3 point, const Shape *shape, const Ray &ray) const {
	if (shape == NULL) {
		return VEC3(1, 1, 1);
	}
	return shape->colour;
}
*/

Shader::Shader(const vector<Light> lights, const vector<const Shape *> &shapes, VEC3 eye)
	: lights(lights), shapes(shapes), eye(eye)
{}


// Calculates the component-wise product of two vectors
VEC3 hadamard(VEC3 a, VEC3 b) {
	return VEC3(a[0]*b[0], a[1]*b[1], a[2]*b[2]);
}

// Calculates the Phong shading for a single source
VEC3 Shader::calculateSourcePhongShading(VEC3 point, const Light &light, const Shape *shape, VEC3 normal, VEC3 eyeDir) {
	// Calculate light direction
	VEC3 lightDir = (light.pos - point).normalized();

	// Calculate diffuse term
	float lightValue = normal.dot(lightDir);
	VEC3 diffuseColour = light.colour * max((float) 0.0, lightValue);

	// Calculate specular term
	VEC3 reflection = (-lightDir + 2.0 * lightValue * normal);
	float specularValue = reflection.dot(eyeDir);
	VEC3 specularColour = light.colour * pow(max((float) 0.0, specularValue), shape->phong);

	// Sum all lights
	VEC3 lightEffects = diffuseColour + specularColour;
	return hadamard(shape->colour, lightEffects);
}

// Returns true if a point is blocked from the light
bool Shader::isOccludedFromLight(VEC3 point, const Light &light, const vector<const Shape *> &shapes) {
	VEC3 dir = (light.pos - point).normalized();
	const float shadowAcneFix = 0.01;  // Prevents light from intersecting with point itself
	Ray ray(point + dir * shadowAcneFix, dir);                                      // IS THIS A CORRECT FIX??
	pair<VEC3, const Shape*> intersection= ray.getClosestIntersection(shapes);      // BUG: SPHERE MIGHT BE FURTHER THAN LIGHT!!!

	// Return true if the ray intersected with a sphere
	return (intersection.second != NULL);
}

// Calculates full 3-term lighting with shadows
//  Computes diffuse lighting and specular highligts for all lights
VEC3 calculateShading(VEC3 point, const Shape *shape, const Ray &ray) {
	// Return black if no intersection
	if (shape == NULL) {
		return VEC3(0, 0, 0);
	}

	VEC3 colour = VEC3(0, 0, 0);
	VEC3 normal = shape->getNormalAt(point);
	VEC3 eyeDir = (eye - point).normalized(); 
	
	// Sum shading for all lights
	for (const Light &light : lights) { 
		// If occluder exists, ignore shading
		bool isOccluded = isOccludedFromLight(point, light, shapes);
		if (isOccluded) {
			continue;
		}

		colour += calculateSourcePhongShading(point, light, shape, normal, eyeDir);
	}

	return colour;
}
