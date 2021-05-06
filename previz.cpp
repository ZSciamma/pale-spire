//////////////////////////////////////////////////////////////////////////////////
// This is a front end for a set of viewer clases for the Carnegie Mellon
// Motion Capture Database: 
//    
//    http://mocap.cs.cmu.edu/
//
// The original viewer code was downloaded from:
//
//   http://graphics.cs.cmu.edu/software/mocapPlayer.zip
//
// where it is credited to James McCann (Adobe), Jernej Barbic (USC),
// and Yili Zhao (USC). There are also comments in it that suggest
// and Alla Safonova (UPenn) and Kiran Bhat (ILM) also had a hand in writing it.
//
//////////////////////////////////////////////////////////////////////////////////
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <float.h>
#include <time.h>
#include "SETTINGS.h"

#include "ray.h"
#include "shapes.h"
#include "material.h"
#include "texture.h"
#include "raytracer.h"
#include "shader.h"

#include "skeleton.h"
#include "displaySkeleton.h"
#include "motion.h"

using namespace std;

// Stick-man classes
DisplaySkeleton displayer;    
Skeleton* skeleton;
Motion* motion;

extern const int WINDOW_WIDTH;
extern const int WINDOW_HEIGHT;

VEC3 eye(-2, 0.5, 1);
//VEC3 eye(-6, 0.5, 1);
VEC3 lookingAt(5, 0.5, 1);
VEC3 up(0,1,0);
float nearPlane = 40;
float fovy = 60;

vector<const Shape *> shapes;
vector<const Light> lights;

// Materials for rendering
RayTracer *tracer = NULL;
RayTracer *&rayTracer = tracer;
const Plastic plastic(10.0);
const Metal metal(0.2, 0.5);
const GlossyPlastic glossyPlastic(10.0, rayTracer);

const Texture brushedMetal("textures/demo_brushed_metal.ppm", 800, 533);
const Texture marbleCheckerboard("textures/marble_checkerboard.ppm", 1200, 802);

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
void writePPM(const string& filename, float& xRes, float& yRes, const float* values)
{
	int totalCells = (int) xRes * (int) yRes;
	unsigned char* pixels = new unsigned char[3 * totalCells];
	for (int i = 0; i < 3 * totalCells; i++)
		pixels[i] = values[i];

	FILE *fp;
	fp = fopen(filename.c_str(), "wb");
	if (fp == NULL)
	{
		cout << " Could not open file \"" << filename.c_str() << "\" for writing." << endl;
		cout << " Make sure you're not trying to write from a weird location or with a " << endl;
		cout << " strange filename. Bailing ... " << endl;
		exit(0);
	}

	fprintf(fp, "P6\n%d %d\n255\n", (int) xRes, (int) yRes);
	fwrite(pixels, 1, totalCells * 3, fp);
	fclose(fp);
	delete[] pixels;
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
float clamp(float value)
{
	if (value < 0.0)      return 0.0;
	else if (value > 1.0) return 1.0;
	return value;
}

void renderImage(const string& filename) 
{
	Camera camera(WINDOW_WIDTH, WINDOW_HEIGHT, eye, lookingAt, up, nearPlane, fovy);

	// allocate the final image
	const int totalCells = camera.xRes * camera.yRes;
	float* ppmOut = new float[3 * totalCells];

	// Create rendering objects
	PhysicsWorld world(shapes);	// Calculates intersections
	Shader shader(lights, world, eye);	// Calculates colours
	if (rayTracer != NULL) {
		delete rayTracer;
	}
	rayTracer = new RayTracer(camera, shader, world);
	//RayTracer rayTracer(camera, shader, world);	// Interface handling all raytracing

	for (int y = camera.screenBot; y <= camera.screenTop; y++) {                       // WHAT IF IT'S NOT DIVISIBLE BY 2?
		for (int x = camera.screenLeft; x <= camera.screenRight; x++) {
			//cout << "tracing a ray" << endl;
			// Get the colour
			VEC3 colour = rayTracer->calculateAveragedPixelcolour(x, y);
			//Ray ray = rayTracer->generateAtCoord(x, y);
			//VEC3 colour = rayTracer->calculateColour(ray);
			//cout << "getting the colour" << endl;

			// set, in final image
			int startPos = 3 * ((camera.xRes * (camera.screenTop - y)) + (x - camera.screenLeft));
			ppmOut[startPos] = clamp(colour[0]) * 255.0;
			ppmOut[startPos + 1] = clamp(colour[1]) * 255.0;
			ppmOut[startPos + 2] = clamp(colour[2]) * 255.0;
			//ppmOut[3 * (y * camera.xRes + x)] = clamp(color[0]) * 255.0f;
			//ppmOut[3 * (y * camera.xRes + x) + 1] = clamp(color[1]) * 255.0f;
			//ppmOut[3 * (y * camera.xRes + x) + 2] = clamp(color[2]) * 255.0f;
		}
	}
	writePPM(filename, camera.xRes, camera.yRes, ppmOut);

	delete[] ppmOut;
}


//////////////////////////////////////////////////////////////////////////////////
// Load up a new motion captured frame
//////////////////////////////////////////////////////////////////////////////////
void setSkeletonsToSpecifiedFrame(int frameIndex)
{
	if (frameIndex < 0)
	{
		printf("Error in SetSkeletonsToSpecifiedFrame: frameIndex %d is illegal.\n", frameIndex);
		exit(0);
	}
	if (displayer.GetSkeletonMotion(0) != NULL)
	{
		int postureID;
		if (frameIndex >= displayer.GetSkeletonMotion(0)->GetNumFrames())
		{
			cout << " We hit the last frame! You might want to pick a different sequence. " << endl;
			postureID = displayer.GetSkeletonMotion(0)->GetNumFrames() - 1;
		}
		else 
			postureID = frameIndex;
		displayer.GetSkeleton(0)->setPosture(* (displayer.GetSkeletonMotion(0)->GetPosture(postureID)));
	}
}

// Defines a placement of a cylinder (for stars)
struct CylinderConfig {
	VEC3 base;	// The base of the cylinder (center of the 'lowest' cross-section)
	VEC3 up;	// The up vector, pointing up the cylinder's length (normalized)
	VEC3 colour;
};

vector<CylinderConfig> starConfigs;

// Sets the positions and colours of all the stars outside the spaceship
void initialiseStars() {
	vector<VEC3> allowedColours { VEC3(1, 0, 0), VEC3(0, 1, 0), VEC3(1, 1, 1) };

	// Choose a random number of stars
	int cylinderNum = 100;
	//float cylinderNum = rand() % 30 + 50;			// IS THIS RANDOMNESS NECESSARY? WHY NOT JUST CHOOSE ONE

	// For each star, choose a random position and colour
	for (int i = 0; i < cylinderNum; i++) {
		// Choose a base for the cylinder within the allowed space
		VEC3 base = VEC3(10, 0.5, 1);

		// Compute the up vector so it makes a circular pattern around the ship
		VEC3 up = VEC3(0, 0, 0);

		// Choose a random colour 
		float colourIndex = rand() % allowedColours.size(); 
		VEC3 colour = allowedColours[colourIndex];

		// Add the star
		starConfigs.push_back(CylinderConfig{ base, up, colour });
	}
}

// Creates the triangles for the floor
void createFloor() {
	// Create floor
	float floorLevel = 0;
	for (int x = -6; x < 8; x+=2) {
		for (int z = -2; z < 6; z+=2) {
			//shapes.push_back(new Sphere(VEC3(x, floorLevel-1, z), 1, VEC3(0.5, 0.5, 0.5), 10));
			//shapes.push_back(new Sphere(VEC3(x+1, floorLevel-0.95, z+1), 1, VEC3(0, 0, 1), 10));
			Triangle *triangle1 = new Triangle(VEC3(x, floorLevel, z), VEC3(x, floorLevel, z+2), VEC3(x+2, floorLevel, z+2), metal, &marbleCheckerboard);//VEC3(0.5, 0.5, 0.5)));//VEC3(0.5, 0.5, 0.5), 10));
			Triangle *triangle2 = new Triangle(VEC3(x, floorLevel, z), VEC3(x+2, floorLevel, z), VEC3(x+2, floorLevel, z+2), metal, &marbleCheckerboard);//VEC3(0, 1, 0)));//VEC3(0, 1, 0), 10));

			triangle1->setTextureCoords(VEC2(0, 0), VEC2(1, 0), VEC2(1, 1));
			triangle2->setTextureCoords(VEC2(0, 0), VEC2(0, 1), VEC2(1, 1));

			shapes.push_back(triangle1);
			shapes.push_back(triangle2);
		}
	}

	//shapes.push_back(new Triangle(VEC3(3, -1, -2), VEC3(3, -1, 2), VEC3(5, -1, 0), VEC3(0, 1, 1), 10));
	//shapes.push_back(new Triangle(VEC3(3, -1, -2), VEC3(5, -1, 0), VEC3(5, -1, -4), VEC3(0, 1, 1), 10));
	//shapes.push_back(new Triangle(VEC3(3, -1, 2), VEC3(5, -1, 0), VEC3(5, -1, 4), VEC3(0, 1, 1), 10));
}

// Computes the length of the stars outside of the spaceship for this frame
//	For the hyperspace effect, the star length increases gradually
//	to pretend we're shooting through space.
float computeStarLength(int frameNumber) {
	if (frameNumber < 100) {
		return 0.1;
	}
	return 0.1 + (frameNumber - 100) * 3;
}

// Creates the stars for the hyperspace animation
void createStars(int frameNumber) {
	// Calculate the length of stars for this frame
	float starLength = computeStarLength(frameNumber);
	float radius = 0.1;

	// Create all cylinders
	for (CylinderConfig config : starConfigs) {
		VEC3 center = config.base + config.up * starLength/2;
		shapes.push_back(new Cylinder(center, radius, starLength, config.up, metal, config.colour));
	}

}

// Calculates the camera position and direction for this frame
void incrementCamera(int frame) {
	if (frame < 20) {
		eye += VEC3(0.03, 0, 0);
	} else if (frame < 80) {
		eye += VEC3(0, 0, -0.05);
	} else {
		eye += VEC3(-0.03, 0, -0.03);
	}
}

//////////////////////////////////////////////////////////////////////////////////
// Build a list of spheres in the scene
//////////////////////////////////////////////////////////////////////////////////
void buildScene(int frameNumber)
{
	//cout << "building scene" << endl;
	shapes.clear();															// DO WE NEED TO DELETE THE SPHERES?
	//shapes.push_back(new Sphere(VEC3(5, 0.5, 2), 1, VEC3(0,1,0), 10));
	shapes.push_back(new Sphere(VEC3(0.8, 0, 0.8), 0.6, metal, VEC3(0,1,0)));
	//shapes.push_back(new Triangle(VEC3(-3, 0.5, -1), VEC3(-3, 0.5, 3), VEC3(-1, 1.5, 1), VEC3(0, 0, 1), 10));
	createFloor();
	//cout << "finished creating floor" << endl;
	//createStars(frameNumber);

	//shapes.push_back(new Cylinder(VEC3(3, 0.5, 1), 0.5, 1, VEC3(0, 1, 0), VEC3(0.5, 0.5, 0.5), 10));

	//shapes.push_back(new Sphere(VEC3(3, 0.5, 1), 0.5, VEC3(0,1,1)));
	//shapes.push_back(new Triangle(VEC3(3, 0.5, 0), VEC3(3, 0.5, 2), VEC3(3, 1.5, 1), VEC3(0,1,1), 10));
	//shapes.push_back(new Sphere(VEC3(1, -1, -10), 0.5, VEC3(0,1,0)));
	//shapes.push_back(new Sphere(VEC3(0, 1, -10), 0.5, VEC3(0,1,0)));
	//shapes.push_back(new Sphere(VEC3(0, 1, 10), 0.5, VEC3(0,1,0)));
	//shapes.push_back(new Sphere(VEC3(0, 1, 10), 0.5, VEC3(0,1,0)));
	//shapes.push_back(new Sphere(VEC3(0, 1, 10), 0.5, VEC3(0,1,0)));
	//shapes.push_back(new Sphere(VEC3(1, 1, 10), 0.5, VEC3(0,1,0)));
	//shapes.push_back(new Sphere(VEC3(1, 1, 1), 0.5, VEC3(0,1,0)));
	//shapes.push_back(new Sphere(VEC3(1, 1, 1), 0.5, VEC3(0,1,0)));
	//shapes.push_back(new Sphere(VEC3(5, 5, 5), 0.5, VEC3(0,1,0)));
	//shapes.push_back(new Sphere(VEC3(0, 0, 5), 0.5, VEC3(0,1,0)));
	//shapes.push_back(new Sphere(VEC3(1, 1, 5), 0.5, VEC3(0,1,0)));
	//shapes.push_back(new Triangle(VEC3(-100, -100, 2), VEC3(100, -100, 2), VEC3(0, 100, 2), VEC3(0, 1, 0));
	//sphereCenters.clear();
	//sphereRadii.clear();
	//sphereColors.clear();

	lights.clear();													// REMOVE; LIGHTS NEVER NEED TO MOVE
	lights.push_back(Light{ VEC3(-3, 1.5, 1), VEC3(1, 1, 1) });//VEC3(-1, 1.5, 3), VEC3(7, 2.5, 1) });
	lights.push_back(Light{ VEC3(1, 2.5, -1), VEC3(1, 1, 1) });//VEC3(-1, 1.5, 3), VEC3(7, 2.5, 1) });

	displayer.ComputeBonePositions(DisplaySkeleton::BONES_AND_LOCAL_FRAMES);

	// retrieve all the bones of the skeleton
	vector<MATRIX4>& rotations = displayer.rotations();
	vector<MATRIX4>& scalings  = displayer.scalings();
	vector<VEC4>& translations = displayer.translations();
	vector<float>& lengths     = displayer.lengths();

	// build a sphere list, but skip the first bone, 
	// it's just the origin
	int totalBones = rotations.size();
	for (int x = 1; x < totalBones; x++)
	{
		MATRIX4& rotation = rotations[x];
		MATRIX4& scaling = scalings[x];
		VEC4& translation = translations[x];

		// get the endpoints of the cylinder
		VEC4 leftVertex(0,0,0,1);
		VEC4 rightVertex(0,0,lengths[x],1);

		leftVertex = rotation * scaling * leftVertex + translation;
		rightVertex = rotation * scaling * rightVertex + translation;

		// get the direction vector
		VEC3 direction = (rightVertex - leftVertex).head<3>();
		const float magnitude = direction.norm();
		direction *= 1.0 / magnitude;

		// how many spheres?
		const float sphereRadius = 0.05;
		const int totalSpheres = magnitude / (2.0 * sphereRadius);
		const float rayIncrement = magnitude / (float)totalSpheres;

		// store the spheres
		VEC3 center = (rightVertex.head<3>() + leftVertex.head<3>()) / 2;
		VEC3 up = rightVertex.head<3>() - leftVertex.head<3>();
		shapes.push_back(new Cylinder(center, 0.05, lengths[x], up, plastic, VEC3(1, 0, 0)));
		//shapes.push_back(new Sphere(leftVertex.head<3>(), 0.05, VEC3(1,0,0), 10));
		//shapes.push_back(new Sphere(rightVertex.head<3>(), 0.05, VEC3(1, 0, 0), 10));
		////shapes.push_back(new Sphere(leftVertex.head<3>(), 0.05, VEC3(1,0,0), 10));
		////shapes.push_back(new Sphere(rightVertex.head<3>(), 0.05, VEC3(1, 0, 0), 10));

		//sphereCenters.push_back(leftVertex.head<3>());
		//sphereRadii.push_back(0.05);
		//sphereColors.push_back(VEC3(1,0,0));
		/*
		//sphereCenters.push_back(rightVertex.head<3>());
		//sphereRadii.push_back(0.05);
		//sphereColors.push_back(VEC3(1,0,0));
		for (int y = 0; y < totalSpheres; y++)
		{
			VEC3 center = ((float)y + 0.5) * rayIncrement * direction + leftVertex.head<3>();
			////shapes.push_back(new Sphere(center, 0.05, VEC3(1, 0, 0), 10));
			//sphereCenters.push_back(center);
			//sphereRadii.push_back(0.05);
			//sphereColors.push_back(VEC3(1,0,0));
		} 
		*/
	}
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
int main(int argc, char** argv)
{
	// Initialise the random generator
	srand(time(NULL));

	string skeletonFilename("01.asf");
	string motionFilename("126_11.amc");
	//string skeletonFilename("02.asf");
	//string motionFilename("02_05.amc");
	
	// load up skeleton stuff
	skeleton = new Skeleton(skeletonFilename.c_str(), MOCAP_SCALE);
	skeleton->setBasePosture();
	displayer.LoadSkeleton(skeleton);

	// load up the motion
	motion = new Motion(motionFilename.c_str(), MOCAP_SCALE, skeleton);
	displayer.LoadMotion(motion);
	skeleton->setPosture(*(displayer.GetSkeletonMotion(0)->GetPosture(0)));

	// Setup the stars
	initialiseStars();

	// Note we're going 4 frames at a time, otherwise the animation
	// is really slow.
	int frameIncrement = 4;
	for (int x = 0; x < 1200; x += frameIncrement)
	{
		setSkeletonsToSpecifiedFrame(x);
		buildScene(x / frameIncrement);
		//cout << "finished building scene" << endl;
		incrementCamera(x / frameIncrement);

		char buffer[256];
		sprintf(buffer, "./frames/frame.%04i.ppm", x / 4);
		//renderImage(windowWidth, windowHeight, buffer);
		renderImage(buffer);
		cout << "Rendered " + to_string(x / frameIncrement) + " frames" << endl;
	}

	return 0;
}

// NEXT UP:
	// Make raytracer getClosestIntersection its own class (e.g. shapes wrapper class)
	//	So shader and raytracer can both import it with no issues
	// Get full phong shading working
	// Get cylinders working.



//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
// Professor Kim's original code:

/*
bool raySphereIntersect(const VEC3& center, 
												const float radius, 
												const VEC3& rayPos, 
												const VEC3& rayDir,
												float& t)
{
	const VEC3 op = center - rayPos;
	const float eps = 1e-8;
	const float b = op.dot(rayDir);
	float det = b * b - op.dot(op) + radius * radius;

	// determinant check
	if (det < 0) 
		return false; 
	
	det = sqrt(det);
	t = b - det;
	if (t <= eps)
	{
		t = b + det;
		if (t <= eps)
			t = -1;
	}

	if (t < 0) return false;
	return true;
}
*/

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
/*
void rayColor(const VEC3& rayPos, const VEC3& rayDir, VEC3& pixelColor) 
{
	pixelColor = VEC3(1,1,1);

	// look for intersections
	//int hitID = -1;
	const Sphere *hitSphere = NULL;
	float tMinFound = FLT_MAX;
	for (int y = 0; y < shapes.size(); y++)
	{
		const Sphere *sphere = (Sphere *) shapes[y];
		float smallest = FLT_MAX;

		//if (raySphereIntersect(sphereCenters[y], sphereRadii[y], rayPos, rayDir, tMin))
		const Ray ray = Ray(rayPos, rayDir.normalized(), 10);
		if (sphere->intersects(ray, smallest))
		{ 
			// is the closest so far?
			if (smallest < tMinFound)
			{
				tMinFound = smallest;
				//hitID = y;
				hitSphere = sphere;
			}
		}
	}
	
	// No intersection, return white
	if (hitSphere == NULL) {
		return;
	}
	//if (hitID == -1)
		//return;

	// set to the sphere color
	//pixelColor = sphereColors[hitID];
	pixelColor = hitSphere->colour;
}
*/

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
/*
void renderImage(int& xRes, int& yRes, const string& filename) 
{
	// allocate the final image
	const int totalCells = xRes * yRes;
	float* ppmOut = new float[3 * totalCells];

	// compute image plane
	const float halfY = (lookingAt - eye).norm() * tan(45.0f / 360.0f * M_PI);
	const float halfX = halfY * 4.0f / 3.0f;

	const VEC3 cameraZ = (lookingAt - eye).normalized();
	const VEC3 cameraX = up.cross(cameraZ).normalized();
	const VEC3 cameraY = cameraZ.cross(cameraX).normalized();

	for (int y = 0; y < yRes; y++) 
		for (int x = 0; x < xRes; x++) 
		{
			const float ratioX = 1.0f - x / float(xRes) * 2.0f;
			const float ratioY = 1.0f - y / float(yRes) * 2.0f;
			const VEC3 rayHitImage = lookingAt + 
															 ratioX * halfX * cameraX +
															 ratioY * halfY * cameraY;
			const VEC3 rayDir = (rayHitImage - eye).normalized();

			// get the color
			VEC3 color;
			Ray ray = Ray(eye, rayDir, 10);
			color = raytracer.calculateColour(ray);
			//rayColor(eye, rayDir, color);

			// set, in final image
			ppmOut[3 * (y * xRes + x)] = clamp(color[0]) * 255.0f;
			ppmOut[3 * (y * xRes + x) + 1] = clamp(color[1]) * 255.0f;
			ppmOut[3 * (y * xRes + x) + 2] = clamp(color[2]) * 255.0f;
		}
	writePPM(filename, xRes, yRes, ppmOut);

	delete[] ppmOut;
}
*/
