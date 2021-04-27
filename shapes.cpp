#include "shapes.h"

Shape::Shape(VEC3 colour, float phong) : colour(colour), phong(phong)
{}

Sphere::Sphere(VEC3 center, float radius, VEC3 colour, float phong)
	: Shape(colour, phong), center(center), radius(radius)
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

VEC3 Sphere::getNormalAt(VEC3 point, const Ray &ray) const {
	return (point - center).normalized();
}

bool Sphere::intersects(const Ray &ray, float &t) const {
	vector<float> roots = computeAllIntersectionRoots(ray);
	return hasSmallestPositiveRoot(roots, t);
}

Triangle::Triangle(VEC3 a, VEC3 b, VEC3 c, VEC3 colour, float phong)
	: Shape(colour, phong), a(a), b(b), c(c)
{}

VEC3 Triangle::getNormalAt(VEC3 point, const Ray &ray) const {									// FIX THIS!!
	// Get vector perpendicular to plane
	VEC3 normal = ((b-a).cross(c-a)).normalized();

	// Reverse normal if it's pointing in the wrong direction (away from ray origin)
	bool wrongDirection = (-ray.d).dot(normal) < 0;
	if (wrongDirection) {
		normal = -normal;
	}

	return normal;
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
	if (beta < 0 or beta > 1 - gamma) {
		return false;
	}

	t = -(_f*ak_jb + _e*jc_al + _d*bl_kc) / M;
	return t > 0;
}

bool Triangle::intersects(const Ray &ray, float& t) const {
	return intersectsWithRay(ray, t);
}

// Creates the new basis vectors for the ray tracing
//  Follows computation given in M&S, pg. 145; t is "view-up vector"
void Cylinder::create_basis_vectors(VEC3 up) {
	w = up.normalized();
	// Get any vector perpendicular to the up vector. This is a radius
	v = VEC3(1, 1, (-w[0] -w[1]) / w[2]);
	v.normalize();
	// Get the radius perpendicular to the other radius.
	u = v.cross(w);
	u.normalize();								// IS THIS NECESSARY?????
}

Cylinder::Cylinder(VEC3 center, float radius, float height, VEC3 up, VEC3 colour, float phong)
	: Shape(colour, phong), center(center), radius(radius), height(height)
{
	create_basis_vectors(up);
}

VEC3 Cylinder::getNormalAt(VEC3 point, const Ray &ray) const {									// FIX THIS!!
	return point - center;				
}

// Calculates the component-wise product of two vectors
VEC3 Shape::hadamard(VEC3 a, VEC3 b) {
	return VEC3(a[0]*b[0], a[1]*b[1], a[2]*b[2]);
}

// Returns sum of square of each element
float sum_of_square_elements(VEC3 a) {
	return pow(a[0], 2) + pow(a[1], 2) + pow(a[2], 2);
}

// Returns true if an intersection exists
//	Takes the t points at which the ray hits the infinite cylinder
//	Keeps only the closest t point which intersects with the finite cylinder
bool exists_closest_valid_intersection(vector<float> roots, float &t) {
	return true;							// COMPLETE THIS FUNCTION!!
}

// Extrude the cylinder to infinity and find intersection t range
//	Then limit the height and check there's an intersection point within it
bool Cylinder::intersects(const Ray &ray, float& t) const {
	// Get vector from chosen to point to center
	//	chosen = o + t*d - center;

	// Find distance from center along radiusx direction and radiusy direction
	//	xDist = chosen.dot(radiusX)
	//	yDist = chosen.dot(radiusY)

	// Check distance from center is radius
	//	xDist^2 + yDist^2 = radius^2
	//	(t*d + o-center).dot(radiusX) etc.
	VEC3 diff = ray.o - center;
	float A = sum_of_square_elements(hadamard(ray.d, u)) + sum_of_square_elements(hadamard(ray.d, v));
	float B = 0;
	float C = -radius * radius;
	for (int i = 0; i < 3; i++) {						// MAKE THIS MORE EFFICIENT? (store intermediates)
		for (int j = 0; j < 2; j++) {
			// t^1 term
			B += ray.d[i]*u[i] + diff[j]*u[j];
			B += ray.d[i]*v[i] + diff[j]*v[j];
			// t^0 term
			C += diff[i]*u[i] * diff[j]*u[j];
			C += diff[i]*v[i] * diff[j]*v[j];
		}
	}

	// Solve infinite intersection and keep closest intersection with finite cylinder
	vector<float> roots = get_quadratic_positive_roots(A, B, C);
	return exists_closest_valid_intersection(roots, t);
}

/*	// Check in what t range the ray is within the circle radius
	// Extend the ray vector
	// ((t*d).dot(u))^2 + ((t*d).dot(v))^2 <= radius
	//float A = sum_of_square_elements(hadamard(ray.d, u)) + sum_of_square_elements(hadamard(ray.d, v));
	//float B = 
	//float C = radius;

	// Get radius of cylinder in direction of origin


	// Get distance along x radius
	// td.dot(u)

	// Get distance along y radius
	// td.dot(v)


	// ((t*d).dot(u))^2 + ((t*d).dot(v))^2 +- radius = 0
	// (td[0]u[0] + td[1]u[1] + td[2]u[2])^2 + (td[0]v[0] + td[1]v[1] + td[2]v[2])^2 +- radius = 0
	// t^2(d[0]u[0]^2 + d[1]u[1]^2 + d[2]u[2]^2 + ...) + t()

	// dir = ray.o
	// v1.u1 + v2.u2 + v3.u3 = 0

	// Calculate cylinder radius vector in that plane
	// Calculate distance between origin and center, along the cylinder radius in that plane
	// Keep points where distance is equal to radius of cylinder
	// (o + td)	

*/
