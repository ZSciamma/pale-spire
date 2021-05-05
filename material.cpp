#include "material.h"

Material::Material() {}

Plastic::Plastic(float cPhong)
	: Material(), cPhong(cPhong) {}

// Calculates the Phong shading for a single light source
VEC3 Plastic::calculateShading(const Shape *shape, VEC3 point, VEC3 normal, const Light &light, VEC3 eyeDir) const {
	// Calculate light direction
	VEC3 lightDir = (light.pos - point).normalized();

	// Calculate diffuse term
	float lightValue = normal.dot(lightDir);
	VEC3 diffuseColour = light.colour * max((float) 0.0, lightValue);

	// Calculate specular term
	VEC3 reflection = (-lightDir + 2.0 * lightValue * normal);
	float specularValue = reflection.dot(eyeDir);
	VEC3 specularColour = light.colour * pow(max((float) 0.0, specularValue), cPhong);

	// Sum all lights
	VEC3 lightEffects = diffuseColour + specularColour;
	return Shape::hadamard(shape->colour, lightEffects);
}

Metal::Metal(float cGaussian, float cReflection)
	: Material(), cGaussian(cGaussian), cReflection(cReflection) {}

// Calculates the Cook-Torrance Shading for a single source
// 	Code taken directly from Professor Kim's BRDFs demo, from file
//	BRDFs/chapter6/illumination_models/illumination_models/shaders.js, 
//	line 113-176, provided on 19th April 2021. We edited this 
//	code to convert it from GLSL to C++ and make it more legible 
//	and consistent within the context of our program.
VEC3 Metal::calculateShading(const Shape *shape, VEC3 point, VEC3 normal, const Light &light, VEC3 eyeDir) const {
	// material properties
	VEC3 mat_diffuse = shape->colour;	// Main colour of the material, I think?
	//VEC3(0.0, 0.0, 1.0);VEC3(1.0, 1.0, 1.0);
	VEC3 mat_specular = VEC3(1.0, 1.0, 1.0);	// Brightness of the specular highlights, I think?

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
	float D = cGaussian * exp(- alpha * alpha * cGaussian); 

	// Geometric factor (G)
	float G1 = 2.0 * NdotH * NdotV / VdotH;
	float G2 = 2.0 * NdotH * NdotL / VdotH;
	float smaller = min(G1, G2);
	float G = min((float) 1.0, smaller);

	// Fresnel Refraction (F) - Schlick's approximation
	float k = pow(1.0 - NdotV, 5.0);
	float F = cReflection + (1.0 - cReflection) * k;

	VEC3 specular = mat_specular * (F * D * G) / NdotV;

	// final color
	VEC4 finalColor = 0.4*VEC4(diffuse[0], diffuse[1], diffuse[2], 1.0) + 0.8*VEC4(specular[0], specular[1], specular[2], 1.0);
	return VEC3(finalColor[0], finalColor[1], finalColor[2]);
}
