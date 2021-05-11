#include "shapes.h"

#include <cmath>

Shape::Shape(const Material &mat, VEC3 colour) 
	: baseColour(colour), material(mat)
{}

Shape::Shape(const Material &mat, const Texture *tex) 
	: Shape(mat, VEC3(0, 0, 0))
{
	texture = tex;
}

VEC3 Shape::getColourAt(VEC3 point) const {
	return baseColour;
}

// Calculates the component-wise product of two vectors
VEC3 Shape::hadamard(VEC3 a, VEC3 b) {
	return VEC3(a[0]*b[0], a[1]*b[1], a[2]*b[2]);
}


//////////////////////////////////// SPHERE //////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

Sphere::Sphere(VEC3 center, float radius, const Material &mat, VEC3 colour)
	: Shape(mat, colour), center(center), radius(radius), material(mat)
{}

Sphere::Sphere(VEC3 center, float radius, const Material &mat, const Texture *tex)
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


//////////////////////////////////// TRIANGLE //////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


void Triangle::initialise_intersection_values() {
	_a = a[0] - b[0];
	cout << "_a: " << _a << endl;
	_d = a[0] - c[0];
	_b = a[1] - b[1];
	_e = a[1] - c[1];
	_c = a[2] - b[2];
	_f = a[2] - c[2];
}

// Initialises the change of basis matrix defining the triangle's plane
void Triangle::initialise_rotation_matrix() {
	VEC3 u = (b - a).normalized();
	VEC3 w = u.cross(c - a).normalized();
	VEC3 v = u.cross(w).normalized();

	MATRIX3 localToGlobal;
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

	// Initialise local vertices
	la = transformToLocal(a);
	lb = transformToLocal(b);
	lc = transformToLocal(c);
}

VEC3 Triangle::transformToLocal(VEC3 point) const {
	return globalToLocal * point;
}

Triangle::Triangle(VEC3 a, VEC3 b, VEC3 c, const Material &mat, VEC3 colour)
	: Shape(mat, colour), a(a), b(b), c(c), material(mat)
{
	// Initialise reused values for intersection checking
	_a = a[0] - b[0];
	_d = a[0] - c[0];
	_b = a[1] - b[1];
	_e = a[1] - c[1];
	_c = a[2] - b[2];
	_f = a[2] - c[2];
	initialise_rotation_matrix();
}

Triangle::Triangle(VEC3 a, VEC3 b, VEC3 c, const Material &mat, const Texture *tex)
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

// Sets mapping of triangle to texture
//	So vertex a will map to texA, etc, and any point inside
//	the triangle will find its location on the texture using 
//	linear interpolation.
//	It is the caller's responsibility to provide a sensible mapping.
void Triangle::setTextureCoords(VEC2 _texA, VEC2 _texB, VEC2 _texC) {
	texA = _texA;
	texB = _texB;
	texC = _texC;
}

// Calculates the f function needed for barycentric coordinates
//  Part of the algorithm described on M&S pg. 165
float Triangle::bary_compute_f(VEC3 fa, VEC3 fb, float x, float y) const {
  return (fa[1]-fb[1])*x + (fb[0]-fa[0])*y + fa[0]*fb[1] - fa[1]*fb[0];
}

// Returns true if a point is inside a triangle
//  (x, y) is the point to be checked; a,b,c are the vertices of the triangle
//  Part of the algorithm described on M&S pg. 165
VEC3 Triangle::get_bary_parameters(float x, float y) const {
	//cout << x << " " << y << endl;
	//cout << "a: " << _a << endl;
	//cout << "b: " << _b << endl;
	//cout << "c: " << _c << endl << endl;

	float alpha = bary_compute_f(lb, lc, x, y) / bary_compute_f(lb, lc, la[0], la[1]);
	float beta = bary_compute_f(lc, la, x, y) / bary_compute_f(lc, la, lb[0], lb[1]);
	float gamma = bary_compute_f(la, lb, x, y) / bary_compute_f(la, lb, lc[0], lc[1]);		// OPTIMISE THIS; CALCULATE IN TERMS OF THE OTHERS
	//bool isInTriangle = (alpha >= 0 and beta >= 0 and gamma >= 0);					// ARE THE EQUALITIES CORRECT? (NOT IN THE TEXT BOOK)

	//cout << alpha << endl;
	return VEC3(alpha, beta, gamma);
	//return isInTriangle;
}

VEC3 Triangle::getColourAt(VEC3 point) const {
	//cout << "Point: " << point[0] << " " << point[1] << " " << point[2] << endl;
	//cout << "getting colour" << endl;
	// Return simple colour if no texture was set
	if (texture == NULL) {
		return baseColour;
	}
	//cout << "got to texture lookup!" << endl;

	// Interpolate between vertices to get relative location of point on triangle
	VEC3 localPoint = transformToLocal(point);
	//cout << "Point: " << _point << endl;
	VEC3 params = get_bary_parameters(localPoint[0], localPoint[1]);
	//VEC3 params = get_bary_parameters(point[0], point[2]);
	//cout << "Bary paremeters:" << endl;
	//cout << params << endl;

	// If point not in triangle for some reason, return red
	if (isnan(params[0]) or isnan(params[1]) or isnan(params[2]) or params[0] > 1 or params[1] > 1 or params[2] > 1) 
		return VEC3(1, 0, 0);

	// Place that point at the same relative position on the texture triangle
	//cout << "params: " << params[0] << " " << params[1] << " " << params[2] << endl;
	VEC2 uv = params[0] * texA + params[1] * texB + params[2] * texC;
	//cout << "uv point is: " << uv[0] << " " << uv[1] << endl;

	// Get the pixel
	return texture->texture_lookup(uv[0], uv[1]);
}


//////////////////////////////////// CYLINDER //////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

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

Cylinder::Cylinder(VEC3 center, float radius, float height, VEC3 up, const Material &mat, const Texture *tex)
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
		normal = VEC3(0, 0, localPoint[2]);
	} else {
		// On rounded edges; normal points outwards
		normal = VEC3(localPoint[0], localPoint[1], 0);
	}

	// Transform back to global coordinates
	normal.normalize();
	normal = transformToGlobal(normal);
	normal.normalize();

	return normal;			
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

	// Check if 'intersection' actually misses cylinder height
	float startHeight = localO[2] + localD[2] * closest;
	float endHeight = localO[2] + localD[2] * furthest;


	float intersectionHeight = startHeight;	// Height at which ray hits cylinder
	if (startHeight > height/2) {
		if (endHeight > height/2) {
			return false;
		}
		intersectionHeight = height/2;
	} else if (startHeight < -height/2) {
		if (endHeight < -height/2) {
			return false;
		}
		intersectionHeight = -height/2;
	}

	t = (intersectionHeight - localO[2]) / localD[2];

	return true;
}

