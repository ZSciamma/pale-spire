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
#include <ctime>
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

// The starting parameters for the camera
VEC3 sEYE(-2, 2, -1);	// behind stickfigure
VEC3 sLOOKINGAT(0.5, 0.5, 1);
VEC3 sUP(0,1,0);

float nearPlane = 40;
float fovy = 60;

// Scene controls
float FLOOR_LEVEL = -1;
float STICKFIGURE_SPEED = 0.05;	// How fast the stickfigure moves 
int FRAME_INCREMENT = 8;	// How many frames of the stickfigure to skip per frame

int SCENE_CHANGE_FRAME = 100;

// Stick-man classes
DisplaySkeleton displayer;    
Skeleton* skeleton;
Motion* motion;

extern const int WINDOW_WIDTH;
extern const int WINDOW_HEIGHT;

//VEC3 eye(-3, 0.5, 1);	// original
//VEC3 eye(-6, 0.5, 1);

// Current parameters for the camera
VEC3 eye;
VEC3 lookingAt;
VEC3 up;

vector<const Shape *> shapes;
vector<const Light> lights;

// Materials for rendering
RayTracer *tracer = NULL;
RayTracer *&rayTracer = tracer;
extern const Plastic plastic(10.0);
extern const Metal metal(0.2, 0.5);
extern const GlossyPlastic glossyPlastic(10.0, rayTracer);

extern const Texture brushedMetal("textures/demo_brushed_metal.ppm", 800, 533);
extern const Texture marbleCheckerboard("textures/marble_checkerboard.ppm", 1200, 802);
extern const Texture blueWood("textures/wooden_blue_ground.ppm", 1920, 1126);
extern const Texture swimmingFloor("textures/swimming_floor_1.ppm", 1920, 1440);
extern const Texture swimmingWall("textures/swimming_wall_1.ppm", 1920, 1279);
extern const Texture swimmingMarble("textures/swimming_marble_1.ppm", 1920, 1285);
extern const Texture emptyFrame("textures/empty_frame.ppm", 1920, 1309);

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



// Calculates the camera position and direction for this frame
void setCamera(int frame) {
	// Camera starting position
	eye = sEYE;
	lookingAt = sLOOKINGAT;
	up = sUP;

	// Increment position of camera for every previous frame
	for (int curFrame = 0; curFrame < frame; curFrame++)
	{
		if (curFrame < 20) {
			eye += VEC3(0.03, 0, 0);
		} else if (curFrame < 80) {
			eye += VEC3(0, 0, -0.05);
		} else {
			eye += VEC3(-0.03, 0, -0.03);
		}
	}
}



//////////////////////////////////////// FIRST SCENE ////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// Creates the triangles for the floor
void createFloor() {
	for (int x = -4; x < 8; x+=2) {
		for (int z = -2; z < 8; z+=2) {
			//shapes.push_back(new Sphere(VEC3(x, floorLevel-1, z), 1, VEC3(0.5, 0.5, 0.5), 10));
			//shapes.push_back(new Sphere(VEC3(x+1, floorLevel-0.95, z+1), 1, VEC3(0, 0, 1), 10));
			Triangle *triangle1 = new Triangle(VEC3(x, FLOOR_LEVEL, z), VEC3(x, FLOOR_LEVEL, z+2), VEC3(x+2, FLOOR_LEVEL, z+2), metal, &swimmingFloor);//VEC3(0.5, 0.5, 0.5)));//VEC3(0.5, 0.5, 0.5), 10));
			Triangle *triangle2 = new Triangle(VEC3(x, FLOOR_LEVEL, z), VEC3(x+2, FLOOR_LEVEL, z), VEC3(x+2, FLOOR_LEVEL, z+2), metal, &swimmingFloor);//VEC3(0, 1, 0)));//VEC3(0, 1, 0), 10));

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

void createWall() {
	// Create ridge at base of back wall
	float ridgeHeight = 0.4;	// The height and depth of the ridge
	Triangle *triangle1 = new Triangle(VEC3(8, FLOOR_LEVEL, -2), VEC3(8, FLOOR_LEVEL, 8), VEC3(8+ridgeHeight, FLOOR_LEVEL+ridgeHeight, -2), metal, &swimmingMarble);//VEC3(0.5, 0.5, 0.5)));//VEC3(0.5, 0.5, 0.5), 10));
	Triangle *triangle2 = new Triangle(VEC3(8, FLOOR_LEVEL, 8), VEC3(8+ridgeHeight, FLOOR_LEVEL+ridgeHeight, -2), VEC3(8+ridgeHeight, FLOOR_LEVEL+ridgeHeight, 8), metal, &swimmingMarble);//VEC3(0, 1, 0)));//VEC3(0, 1, 0), 10));

	triangle1->setTextureCoords(VEC2(0, 0.2), VEC2(0, 1), VEC2(0.4, 0.2));
	triangle2->setTextureCoords(VEC2(0, 1), VEC2(0.4, 0.2), VEC2(0.4, 1));

	shapes.push_back(triangle1);
	shapes.push_back(triangle2);

	// Create back wall
	float wallDepth = 8 + ridgeHeight;	// The depth of the entire back wall
	float wallBase = FLOOR_LEVEL + ridgeHeight;	// The base level of the wall
	float wallHeight = 5;	// Height of a single triangle
	Triangle *triangle3 = new Triangle(VEC3(wallDepth, wallBase, -2), VEC3(wallDepth, wallBase, 8), VEC3(wallDepth, wallBase+wallHeight, -2), metal, &swimmingWall);//VEC3(0.5, 0.5, 0.5)));//VEC3(0.5, 0.5, 0.5), 10));
	Triangle *triangle4 = new Triangle(VEC3(wallDepth, wallBase, 8), VEC3(wallDepth, wallBase+wallHeight, -2), VEC3(wallDepth, wallBase+wallHeight, 8), metal, &swimmingWall);//VEC3(0, 1, 0)));//VEC3(0, 1, 0), 10));

	triangle3->setTextureCoords(VEC2(0, 0.2), VEC2(0, 1), VEC2(0.4, 0.2));
	triangle4->setTextureCoords(VEC2(0, 1), VEC2(0.4, 0.2), VEC2(0.4, 1));

	shapes.push_back(triangle3);
	shapes.push_back(triangle4);

	// Create ridge at base of right wall
	float ridgeRight = 8;	// How far on the right the ridge is
	Triangle *triangle5 = new Triangle(VEC3(8, FLOOR_LEVEL, ridgeRight), VEC3(8, FLOOR_LEVEL+ridgeHeight, ridgeRight+ridgeHeight), VEC3(-4, FLOOR_LEVEL, ridgeRight), metal, &swimmingMarble);//VEC3(0.5, 0.5, 0.5)));//VEC3(0.5, 0.5, 0.5), 10));
	Triangle *triangle6 = new Triangle(VEC3(8, FLOOR_LEVEL+ridgeHeight, ridgeRight+ridgeHeight), VEC3(-4, FLOOR_LEVEL, ridgeRight), VEC3(-4, FLOOR_LEVEL+ridgeHeight, ridgeRight+ridgeHeight), metal, &swimmingMarble);//VEC3(0, 1, 0)));//VEC3(0, 1, 0), 10));

	triangle5->setTextureCoords(VEC2(0, 0.2), VEC2(0, 1), VEC2(0.4, 0.2));
	triangle6->setTextureCoords(VEC2(0, 1), VEC2(0.4, 0.2), VEC2(0.4, 1));

	shapes.push_back(triangle5);
	shapes.push_back(triangle6);

	// Create right wall
	Triangle *triangle7 = new Triangle(VEC3(8, wallBase, ridgeRight+ridgeHeight), VEC3(8, wallBase+wallHeight, ridgeRight+ridgeHeight), VEC3(-4, wallBase, ridgeRight+ridgeHeight), metal, &swimmingWall);//VEC3(0.5, 0.5, 0.5)));//VEC3(0.5, 0.5, 0.5), 10));
	Triangle *triangle8 = new Triangle(VEC3(8, wallBase+wallHeight, ridgeRight+ridgeHeight), VEC3(-4, wallBase, ridgeRight+ridgeHeight), VEC3(-4, wallBase+wallHeight, ridgeRight+ridgeHeight), metal, &swimmingWall);//VEC3(0, 1, 0)));//VEC3(0, 1, 0), 10));

	triangle7->setTextureCoords(VEC2(0, 0.2), VEC2(0, 1), VEC2(0.4, 0.2));
	triangle8->setTextureCoords(VEC2(0, 1), VEC2(0.4, 0.2), VEC2(0.4, 1));

	shapes.push_back(triangle7);
	shapes.push_back(triangle8);
}

// Creates a glossy, surreal cube
void createGlossyCube() {
	VEC3 botLeft(2, 0, 3);	// Bottom left corner of the cube
	float side = 2;	// Side length of the cube

	// Left face
	shapes.push_back(new Triangle(botLeft, VEC3(botLeft[0]-side, botLeft[1], botLeft[2]), VEC3(botLeft[0], botLeft[1]+side, botLeft[2]), glossyPlastic, VEC3(0, 0, 0)));
	shapes.push_back(new Triangle(VEC3(botLeft[0]-side, botLeft[1], botLeft[2]), VEC3(botLeft[0], botLeft[1]+side, botLeft[2]), VEC3(botLeft[0]-side, botLeft[1]+side, botLeft[2]), glossyPlastic, VEC3(0, 0, 0)));

	// Front face
	shapes.push_back(new Triangle(VEC3(botLeft[0]-side, botLeft[1], botLeft[2]), VEC3(botLeft[0]-side, botLeft[1], botLeft[2]+side), VEC3(botLeft[0]-side, botLeft[1]+side, botLeft[2]), glossyPlastic, VEC3(0, 0, 0)));
	shapes.push_back(new Triangle(VEC3(botLeft[0]-side, botLeft[1], botLeft[2]+side), VEC3(botLeft[0]-side, botLeft[1]+side, botLeft[2]), VEC3(botLeft[0]-side, botLeft[1]+side, botLeft[2]+side), glossyPlastic, VEC3(0, 0, 0)));

	// Top face
	shapes.push_back(new Triangle(botLeft, VEC3(botLeft[0]-side, botLeft[1], botLeft[2]), VEC3(botLeft[0], botLeft[1]+side, botLeft[2]), glossyPlastic, VEC3(0, 0, 0)));
	shapes.push_back(new Triangle(VEC3(botLeft[0]-side, botLeft[1], botLeft[2]), VEC3(botLeft[0], botLeft[1]+side, botLeft[2]), VEC3(botLeft[0]-side, botLeft[1]+side, botLeft[2]), glossyPlastic, VEC3(0, 0, 0)));

}


// Calculates the vector to add to the stickfigure's position this frame
VEC3 computeStickfigureMovement(int frame)
{
	return VEC3(0, 0, (float) frame * STICKFIGURE_SPEED);
}


void buildFirstScene(int frameNumber)
{
	shapes.clear();															// DO WE NEED TO DELETE THE SPHERES?

	createFloor();
	createWall();
	createGlossyCube();

	lights.clear();													// REMOVE; LIGHTS NEVER NEED TO MOVE
	lights.push_back(Light{ VEC3(-3, 1.5, 1), VEC3(1, 1, 1) });//VEC3(-1, 1.5, 3), VEC3(7, 2.5, 1) });
	lights.push_back(Light{ VEC3(1, 2.5, -1), VEC3(1, 1, 1) });//VEC3(-1, 1.5, 3), VEC3(7, 2.5, 1) });

	displayer.ComputeBonePositions(DisplaySkeleton::BONES_AND_LOCAL_FRAMES);

	// retrieve all the bones of the skeleton
	vector<MATRIX4>& rotations = displayer.rotations();
	vector<MATRIX4>& scalings  = displayer.scalings();
	vector<VEC4>& translations = displayer.translations();
	vector<float>& lengths     = displayer.lengths();

	// Get stickfigure movement vector (to add to position)
	VEC3 stickfigureMovement = computeStickfigureMovement(frameNumber);

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
		shapes.push_back(new Cylinder(center + stickfigureMovement, 0.05, lengths[x], up, plastic, VEC3(1, 0, 0)));
	}
}


//////////////////////////////////////// SECOND SCENE ////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


void buildSecondScene(int frameNumber) {

}


//////////////////////////////////////////////////////////////////////////////////
// Build a list of spheres in the scene
//////////////////////////////////////////////////////////////////////////////////

void buildScene(int frameNumber) {
	if (frameNumber < SCENE_CHANGE_FRAME) {
		buildFirstScene(frameNumber);
	} else {
		buildSecondScene(frameNumber);
	}
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
int main(int argc, char** argv)
{

	// Set frames between which to render, inclusive
	int startFrame = 0;
	int endFrame = 299;
	if (argc > 1) {
		startFrame = atoi(argv[1]);
		cout << "startFrame: " << startFrame << endl;
	}
	if (argc > 2) {
		endFrame = atoi(argv[2]);
	}

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
	//initialiseStars();

	// Note we're going 4 frames at a time, otherwise the animation
	// is really slow.
	int FRAME_INCREMENT = 8;
	for (int x = startFrame; x <= endFrame; x++)
	{
		time_t start_time = time(NULL);

		setSkeletonsToSpecifiedFrame(x * FRAME_INCREMENT);
		buildScene(x);
		//cout << "finished building scene" << endl;
		setCamera(x);

		char buffer[256];
		sprintf(buffer, "./frames/frame.%04i.ppm", x);
		//renderImage(windowWidth, windowHeight, buffer);
		renderImage(buffer);

		time_t end_time = time(NULL);

		cout << "Rendered " + to_string(x) + " frames (" << end_time - start_time << "s)" << endl;
	}

	return 0;
}



/*
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

*/



/*
	//cout << "building scene" << endl;
	shapes.clear();															// DO WE NEED TO DELETE THE SPHERES?
	//shapes.push_back(new Sphere(VEC3(5, 0.5, 2), 1, VEC3(0,1,0), 10));
	//shapes.push_back(new Sphere(VEC3(0.8, 0, 0.8), 0.6, metal, VEC3(0,1,0)));
	//shapes.push_back(new Triangle(VEC3(-3, 0.5, -1), VEC3(-3, 0.5, 3), VEC3(-1, 1.5, 1), VEC3(0, 0, 1), 10));
	createFloor();
	createGlossyCube();
	//createSpaceship(shapes);
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


			//shapes.push_back(new Sphere(leftVertex.head<3>(), 0.05, VEC3(1,0,0), 10));
		//shapes.push_back(new Sphere(rightVertex.head<3>(), 0.05, VEC3(1, 0, 0), 10));
		////shapes.push_back(new Sphere(leftVertex.head<3>(), 0.05, VEC3(1,0,0), 10));
		////shapes.push_back(new Sphere(rightVertex.head<3>(), 0.05, VEC3(1, 0, 0), 10));

		//sphereCenters.push_back(leftVertex.head<3>());
		//sphereRadii.push_back(0.05);
		//sphereColors.push_back(VEC3(1,0,0));
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

