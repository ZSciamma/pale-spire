#include "shader.h"

Shader::Shader(const vector<const Light> &lights, const PhysicsWorld &world, VEC3 eye)
	: lights(lights), world(world), eye(eye)
{}

// Returns true if a point is blocked from the light
bool Shader::isOccludedFromLight(VEC3 point, const Light &light) const {
	VEC3 dir = (light.pos - point);
	const float shadowAcneFix = 0.01;  // Prevents light from intersecting with point itself
	Ray ray(point + dir * shadowAcneFix, dir);                                      // IS THIS A CORRECT FIX??
	
	// Check if reverse light ray hits a shape
	const Shape* intersectShape = NULL;
	VEC3 intersectPoint;
	bool intersects = world.existsClosestIntersection(ray, intersectShape, intersectPoint);

	// Check if the hit shape was in front of the light
	if (intersects) {
		for (int i = 0; i < 3; i++) {
			if (ray.d[i] == 0) {
				// Ray direction ith component is 0, so useless
				continue;
			} else {
				// Check if, along this component, shape is closer than light
				float timeToLight = (light.pos[i] - ray.o[i]) / ray.d[i];
				float timeToShape = (intersectPoint[i] - ray.o[i]) / ray.d[i];
				intersects = timeToShape < timeToLight;
				break;
			}
		}
	}

	// Return true if the ray intersected with a sphere
	return intersects;
}

// Calculates full 3-term lighting with shadows
//  Computes diffuse lighting and specular highligts for all lights
VEC3 Shader::calculateShading(VEC3 point, const Shape *shape, const Ray &ray) const {
	// Return black if no intersection
	if (shape == NULL) {
		return VEC3(1, 1, 1);
	}

	VEC3 colour = VEC3(0, 0, 0);
	VEC3 normal = shape->getNormalAt(point, ray);
	VEC3 eyeDir = (eye - point).normalized(); 
	
	// Sum shading for all lights
	for (const Light &light : lights) { 
		// If occluder exists, ignore shading
		bool isOccluded = isOccludedFromLight(point, light);
		if (isOccluded) {
			continue;
		}

		//colour += calculateSourcePhongShading(point, light, shape, normal, eyeDir);
		colour += shape->material.calculateShading(shape, point, normal, light, eyeDir);

	}

	return colour;
}


/*
VEC3 Shader::calculateShading(VEC3 point, const Shape *shape, const Ray &ray) const {
	if (shape == NULL) {
		return VEC3(1, 1, 1);
	}
	return shape->colour;
}
*/

/*
// Calculates the component-wise product of two vectors
VEC3 hadamard(VEC3 a, VEC3 b) {
	return VEC3(a[0]*b[0], a[1]*b[1], a[2]*b[2]);
}
*/

/*
// Calculates the Phong shading for a single source
VEC3 Shader::calculateSourcePhongShading(VEC3 point, const Light &light, const Shape *shape, VEC3 normal, VEC3 eyeDir) const {
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
	return Shape::hadamard(shape->colour, lightEffects);
}

// Calculates the Cook-Torrance Shading for a single source
// 	Code taken directly from Professor Kim's BRDFs demo, from file
//	BRDFs/chapter6/illumination_models/illumination_models/shaders.js, 
//	line 83-176, provided on 19th April 2021. We edited this 
//	code to convert it from GLSL to C++ and make it more legible 
//	and consistent within the context of our program.
VEC3 Shader::calculateSourceCookTorranceShading(const Shape *shape, VEC3 point, VEC3 normal, const Light &light, VEC3 eyeDir) const {
	// material properties
	VEC3 mat_diffuse = VEC3(0.0, 0.0, 1.0);
	//VEC3 mat_diffuse = VEC3(1.0, 1.0, 1.0);
	VEC3 mat_specular = VEC3(1.0, 1.0, 1.0);

	// normalize interpolated normal
	VEC3 N = (normal).normalized();

	// light vector (positional light)
	VEC3 L = (light.pos - point).normalized();

	// vertex-to-eye space (view vector)
	VEC3 V = (eyeDir -point).normalized();

	// half-vector
	VEC3 H = (L + V).normalized();
	
	// scalar products
	float NdotH = max(0.0, N.dot(H));
	float VdotH = V.dot(H);
	float NdotV = N.dot(V);
	float NdotL = N.dot(L);

	// ambient component is neglected

	// diffuse component
	VEC3 diffuse = mat_diffuse * NdotL;

	// specular component (Cook-Torrance reflection model)

	// D term (gaussian)
	float alpha = acos(NdotH);
	float D = uC * exp(- alpha * alpha * uC); 

	// Geometric factor (G)
	float G1 = 2.0 * NdotH * NdotV / VdotH;
	float G2 = 2.0 * NdotH * NdotL / VdotH;
	float smaller = min(G1, G2);
	float G = min((float) 1.0, smaller);

	// Fresnel Refraction (F) - Schlick's approximation
	float k = pow(1.0 - NdotV, 5.0);
	float F = uR0 + (1.0 - uR0) * k;

	VEC3 specular = mat_specular * (F * D * G) / NdotV;

	// final color
	VEC4 finalColor = 0.4*VEC4(diffuse[0], diffuse[1], diffuse[2], 1.0) + 0.8*VEC4(specular[0], specular[1], specular[2], 1.0);
	return VEC3(finalColor[0], finalColor[1], finalColor[2]);
}
*/
