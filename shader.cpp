

#include "shader.h"

VEC3 Shader::calculateShading(VEC3 point, const Shape *shape, const Ray &ray) const {
	if (shape == NULL) {
		return VEC3(0, 0, 0);
	}
	return shape->colour;
}
