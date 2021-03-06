#include "raytracer.h"

extern const int STRATIFIED_SAMPLING_ROOT;

Camera::Camera(float xRes, float yRes, VEC3 eye, VEC3 lookingAt, VEC3 up, float nearPlane, float fovy) 
	: xRes(xRes), yRes(yRes), eye(eye), lookingAt(lookingAt),
	up(up), nearPlane(nearPlane), fovy(fovy)
{
	screenLeft = -xRes / 2;                                                // IS IT ALRIGHT THAT THE SCREEN IS UNEVEN? AND WHAT IF IT'S NOT DIVISIBLE BY 2?
	screenRight = screenLeft + xRes - 1;
	screenBot = -yRes / 2;
	screenTop = screenBot + yRes - 1;
}

// Computes and sets the dimensions of the real world viewing plane
void RayTracer::initialise_viewing_plane_dimensions() {
	top = tan(camera.fovy * M_PI / 360) * camera.nearPlane;
	bot = -top;
	right = top * (camera.xRes / camera.yRes);
	left = -right;
}

// Calculates the absolute value of a vector
float vector_absolute_value(VEC3 a) {
  return sqrt(pow(a[0], 2) + pow(a[1], 2) + pow(a[2], 2));
}

// Creates the new basis vectors for the ray tracing
//  Follows computation given in M&S, pg. 145; t is "view-up vector"
std::tuple<VEC3, VEC3, VEC3> create_basis_vectors(VEC3 gaze, VEC3 t) {
	VEC3 w = - (gaze / vector_absolute_value(gaze));
	VEC3 crossTW = t.cross(w);
	VEC3 u = crossTW / vector_absolute_value(crossTW);
	VEC3 v = w.cross(u);
	v.normalize();                                                    // ARE THESE NORMALIZATIONS CORRECT?
	u.normalize();
	w.normalize();
	return std::make_tuple(-u, v, w);                                 // FIX THIS -U TO BE CORRECT!!!
}

// Creates the basis vectors that define the camera frame
void RayTracer::initialise_camera_frame() {
	VEC3 gaze = camera.lookingAt - camera.eye;
	gaze.normalize();
	std::tuple<VEC3, VEC3, VEC3> basis = create_basis_vectors(gaze, camera.up);
	std::tie(u, v, w) = basis;
}

RayTracer::RayTracer(Camera &camera, Shader &shader, PhysicsWorld &world) 
	: camera(camera), shader(shader), world(world)
{
	initialise_viewing_plane_dimensions();
	initialise_camera_frame();
	stratifiedBinNum = pow(STRATIFIED_SAMPLING_ROOT, 2);
	binWidth = 1 / (float) STRATIFIED_SAMPLING_ROOT;
	binHeight = 1 / (float) STRATIFIED_SAMPLING_ROOT;
}

// Generate the ray that goes through this pixel, using perspective projection
//  Coord (0, 0) is in the center
//	binNum is the number of the bin used for stratified sampling
Ray RayTracer::generateAtCoord(float x, float y, int binNum) const {
	// Distributed ray tracing: jitter the pixel inside its bin
	float randX = (float) rand() / (float) RAND_MAX;						// REMOVE RANDOMNESS WHEN THERE'S JUST 1 BIN
	float randY = (float) rand() / (float) RAND_MAX;
	x += -0.5 + binWidth * (randX + (float) (binNum % STRATIFIED_SAMPLING_ROOT));
	y += -0.5 + binHeight * (randY + (float) (binNum / STRATIFIED_SAMPLING_ROOT));

	// Translate camera corner to origin (e.g. range [0, 480])
	x -= camera.screenLeft;                                                // MAKE THE ITERATION MORE EFFICIENT
	y -= camera.screenBot;

	// Convert pixel numbers to world range e.g. [worldleft, worldright]
	float x2 = left + ((right - left) * (x + 0.5) / camera.xRes);            // IS THE +0.5 CORRECT
	float y2 = bot + ((top - bot) * (y + 0.5) / camera.yRes);        // CHANGE THIS TO FIX CAST!!!
	
	// Calculate lookAt point for this ray
	VEC3 s = (x2 * (-u)) + (y2 * v) - (camera.nearPlane * w);
	return Ray(camera.eye, (s - camera.eye).normalized());
};

// Calculates the colour of this ray based on the world
VEC3 RayTracer::calculateColour(const Ray &ray) const {
	// Get intersection of ray with world
	const Shape* intersectShape = NULL;
	VEC3 intersectPoint;
	world.existsClosestIntersection(ray, intersectShape, intersectPoint);

	// Calculate shading at intersection point
	//cout << "calculating shading" << endl;
	VEC3 shaded = shader.calculateShading(intersectPoint, intersectShape, ray);
	return shaded;
}

// Calculates the colour for this pixel by averaging it
//	over all stratified sampling bins
VEC3 RayTracer::calculateAveragedPixelcolour(int x, int y) const {
	VEC3 colour(0, 0, 0);

	// Sum colours for ray going through each sampling bin
	for (int i = 0; i < stratifiedBinNum; i++) {
		Ray ray = generateAtCoord(x, y, i);
		colour += calculateColour(ray);
	}

	return colour / (float) stratifiedBinNum;
}
