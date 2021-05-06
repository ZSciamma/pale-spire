#include "shapes.h"

Shape::Shape(const Material &mat, VEC3 colour) 
	: baseColour(colour), material(mat)
{}

Shape::Shape(const Material &mat, Texture *tex) 
	: Shape(mat, VEC3(0, 0, 0))
{
	texture = tex;
}

VEC3 Shape::getColourAt(VEC3 point) const {
	return baseColour;
}

Sphere::Sphere(VEC3 center, float radius, const Material &mat, VEC3 colour)
	: Shape(mat, colour), center(center), radius(radius), material(mat)
{}

Sphere::Sphere(VEC3 center, float radius, const Material &mat, Texture *tex)
	: Sphere(center, radius, mat, VEC3(0, 0, 0))
{
	texture = tex;
}

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

Triangle::Triangle(VEC3 a, VEC3 b, VEC3 c, const Material &mat, VEC3 colour)
	: Shape(mat, colour), a(a), b(b), c(c), material(mat)
{}

Triangle::Triangle(VEC3 a, VEC3 b, VEC3 c, const Material &mat, Texture *tex)
	: Triangle(a, b, c, mat, VEC3(0, 0, 0))
{
	texture = tex;
}

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

// Uses the method from
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
void Cylinder::create_basis_vectors(VEC3 up) {						// CHECK THIS IS CORRECT
	w = up.normalized();
	// Get any vector perpendicular to the up vector. This is a radius
	if (w[0] == 0 and w[1] == 0) {
		v = VEC3(1, 0, 0);
	} else if (w[1] == 0 and w[2] == 0) {
		v = VEC3(0, 1, 0);
	} else {
		v = VEC3(1, (-w[0] -w[2]) / w[1], 1);
	}
	v.normalize();
	// Get the radius perpendicular to the other radius.
	u = v.cross(w);
	u.normalize();								// IS THIS NECESSARY?????
}

void Cylinder::initialise_rotation_matrix() {
	localToGlobal.setZero();
	localToGlobal(0, 0) = u[0];
	localToGlobal(1, 0) = u[1];
	localToGlobal(2, 0) = u[2];
	localToGlobal(0, 1) = v[0];
	localToGlobal(1, 1) = v[1];
	localToGlobal(2, 1) = v[2];
	localToGlobal(0, 2) = w[0];
	localToGlobal(1, 2) = w[1];
	localToGlobal(2, 2) = w[2];

	// Invert rotation matrix to reverse transformation
	globalToLocal = localToGlobal.inverse().eval();
}

Cylinder::Cylinder(VEC3 center, float radius, float height, VEC3 up, const Material &mat, VEC3 colour)
	: Shape(mat, colour), center(center), radius(radius), height(height), material(mat)
{
	create_basis_vectors(up);
	initialise_rotation_matrix();
}

Cylinder::Cylinder(VEC3 center, float radius, float height, VEC3 up, const Material &mat, Texture *tex)
	: Cylinder(center, radius, height, up, mat, VEC3(0, 0, 0))
{
	texture = tex;
}

VEC3 Cylinder::transformToLocal(VEC3 point) const {
	return globalToLocal * point;
}

VEC3 Cylinder::transformToGlobal(VEC3 point) const {
	return localToGlobal * point;
}

VEC3 Cylinder::getNormalAt(VEC3 point, const Ray &ray) const {									// FIX THIS!!
	// Get the point in local space (cylinder centered at origin pointing up z axis)
	VEC3 localPoint = transformToLocal(point);

	// Check if point is on circular edges ("top" and "bottom")
	//bool isOnCircularEdges = pow(point[0], 2) + pow(point[2], 2) < pow(radius, 2);
	bool isOnCircularEdges = pow(localPoint[0], 2) + pow(localPoint[1], 2) < pow(radius, 2);				// IS THIS CORRECT? IS THERE A PROBLEM ON THE EDGES??
	//bool isOnCircularEdges = pow(localPoint[0], 2) + pow(localPoint[2], 2) < pow(radius, 2);				// IS THIS CORRECT? IS THERE A PROBLEM ON THE EDGES??
	// Get normal
	VEC3 normal;
	if (isOnCircularEdges) {
		// Normal points up or down depending on whether point is above or below origin
		//normal = VEC3(0, point[1], 0);
		normal = VEC3(0, 0, localPoint[2]);
		//normal = VEC3(0, localPoint[1], 0);
	} else {
		// On rounded edges; normal points outwards
		//normal = VEC3(point[0], 0, point[2]);
		//normal = VEC3(localPoint[0], 0, localPoint[2]);
		normal = VEC3(localPoint[0], localPoint[1], 0);
	}

	// Transform back to global coordinates
	normal.normalize();
	normal = transformToGlobal(normal);
	normal.normalize();

	return normal;			
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
	// Transform ray origin and direction to local cylinder space
	VEC3 localD = transformToLocal(ray.d);
	//VEC3 localD = ray.d;
	//VEC3 localO = ray.o;
	VEC3 localO = transformToLocal(ray.o - center); //transformToLocal(ray.o);

	// Choose point on cylinder
	//	chosen = o + t*d

	// Check chosen point is within cylinder height
	//	chosen[1] < height/2 AND chosen[1] > -height/2

	// Check chosen point is within radius distance of cross section center
	//	chosen[0]^2 + chosen[1]^2 < radius^2
	// Get intersection of ray with edges
	//	chosen[0]^2 + chosen[1]^2 = radius^2
	//	(o[0] + t*d[0])^2 + (o[1] + t*d[1])^2 = radius^2
	float A = pow(localD[0], 2) + pow(localD[1], 2);
	float B = 2*localO[0]*localD[0] + 2*localO[1]*localD[1];
	float C = pow(localO[0], 2) + pow(localO[1], 2) - pow(radius, 2);

	// Solve intersection equation
	vector<float> roots = get_quadratic_positive_roots(A, B, C);

	// Return false if no roots
	if (roots.size() == 0) {
		return false;
	}

	// Get closest and furthest time along ray to intersection
	float closest, furthest;
	if (roots.size() == 1) {									// CHECK THIS IS CORRECT!
		closest = roots[0];
		furthest = roots[0];
	} else {
		closest = min(roots[0], roots[1]);						// CHECK GREATER THAN 0!!!!!
		furthest = max(roots[0], roots[1]);
	}
	//cout << closest << endl;
	//cout << furthest << endl << endl;

	// Check if 'intersection' actually misses cylinder height
	float startHeight = localO[2] + localD[2] * closest;
	float endHeight = localO[2] + localD[2] * furthest;

	//cout << startHeight << endl;
	//cout << endHeight << endl << endl;

	float intersectionHeight = startHeight;	// Height at which ray hits cylinder
	if (startHeight > height/2) {
		if (endHeight > height/2) {
			// 'intersection' actually misses cylinder height
			//cout << "hi" << endl;
			return false;
		}
		intersectionHeight = height/2;
	} else if (startHeight < -height/2) {
		if (endHeight < -height/2) {
			//cout << "hello" << endl;
			return false;
		}
		intersectionHeight = -height/2;
	}

	t = (intersectionHeight - localO[2]) / localD[2];
	//t = 3;
	return true;



	/*
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
	*/
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
