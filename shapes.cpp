#include "shapes.h"

Shape::Shape(VEC3 colour) : colour(colour)
{}

Sphere::Sphere(VEC3 center, float radius, VEC3 colour)
	: Shape(colour), center(center), radius(radius)
{}

// Returns all positive roots of a quadratic equation, given the coefficients
//  Assumes Ax^2 + Bx^2 + C = 0
vector<float> get_quadratic_positive_roots(float A, float B, float C) {     // CAN WE MAKE THIS MORE EFFICIENT??
	vector<float> roots;
	float root1 = -1;
	float root2 = -1;

	// Compute all roots
	float discriminant = pow(B, 2) - 4 * A * C;
	if (discriminant == 0) {
		root1 = -B / (2 * A);
	}
	if (discriminant > 0) {
		float root_discriminant = sqrt(discriminant);
		root1 = (-B + root_discriminant) / (2 * A);
		root2 = (-B - root_discriminant) / (2 * A);
	}

	// Return only positive roots
	if (root1 > 0) {                                                    // WHAT ABOUT ROOTS THAT ARE ZERO????
		roots.push_back(root1);        
	}
	if (root2 > 0) {
		roots.push_back(root2);    
	}
	return roots;                                                       // WHAT IF THERE'S ONE POSITIVE AND ONE NEGATIVE?? YOU'RE INSIDE THE SPHERE!!!!!
}

vector<float> Sphere::computeAllIntersectionRoots(const Ray &ray) const {
	// Create sphere intersection equation
	VEC3 eyeToSphere = ray.o - center;
	float A = ray.d.dot(ray.d);
	float B = 2.0 * ray.d.dot(eyeToSphere);
	float C = eyeToSphere.dot(eyeToSphere) - pow(radius, 2);

	// Solve sphere intersection equation
	vector<float> roots = get_quadratic_positive_roots(A, B, C);
	return roots;
}

bool Sphere::hasSmallestPositiveRoot(std::vector<float> roots, float& smallest) const {
	// Handle intersections
	if (roots.size() == 0) {
		// No intersections; return dummy point
		smallest = 0;
		return false;
	} else {
		// Keep only the closest intersection
		smallest = roots[0];
		if (roots.size() == 2) {
			smallest = min(smallest, roots[1]);
		}

		return true;
	}
}

VEC3 Sphere::getNormalAt(VEC3 point) const {
	return (point - center).normalized();
}

bool Sphere::intersects(const Ray &ray, float &t) const {
	vector<float> roots = computeAllIntersectionRoots(ray);
	return hasSmallestPositiveRoot(roots, t);
}

Triangle::Triangle(VEC3 a, VEC3 b, VEC3 c, VEC3 colour)
	: Shape(colour), a(a), b(b), c(c)
{}

VEC3 Triangle::getNormalAt(VEC3 point) const {
	return -((b-a).cross(c-a)).normalized();
}

bool Triangle::intersectsWithRay(const Ray &ray, float& t) const {
	// Create matrix
	float _a = a[0] - b[0];
	float _d = a[0] - c[0];
	float _g = ray.d[0];
	float _b = a[1] - b[1];
	float _e = a[1] - c[1];
	float _h = ray.d[1];
	float _c = a[2] - b[2];
	float _f = a[2] - c[2];
	float _i = ray.d[2];
	float _j = a[0] - ray.o[0];
	float _k = a[1] - ray.o[1];
	float _l = a[2] - ray.o[2];

	// Compute values for efficiency
	float ei_hf = _e*_i - _h*_f;
	float gf_di = _g*_f - _d*_i;
	float dh_eg = _d*_h - _e*_g;
	float ak_jb = _a*_k - _j*_b;
	float jc_al = _j*_c - _a*_l;
	float bl_kc = _b*_l - _k*_c;
	float M = _a * ei_hf + _b * gf_di + _c * dh_eg;

	// Check intersection
	t = 0;
	float gamma = (_i*ak_jb + _h*jc_al + _g*bl_kc) / M;
	if (gamma < 0 or gamma > 1) {
		return false;
	}

	float beta = (_j*ei_hf + _k*gf_di + _l*dh_eg) / M;
	if (beta < 0 or beta > 1) {
		return false;
	}

	t = (_f*ak_jb + _e*jc_al + _d*bl_kc) / M;
	return true;
}

bool Triangle::intersects(const Ray &ray, float& t) const {
	return intersectsWithRay(ray, t);
}
