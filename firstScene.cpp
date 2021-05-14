
#include "firstScene.h"

float FLOOR_LEVEL = -1;

extern float STICKFIGURE_SPEED;	// How fast the stickfigure moves 


extern DisplaySkeleton displayer;    
extern Skeleton* skeleton;
extern Motion* motion;

extern const Plastic plastic;
extern const Metal metal;
extern const GlossyPlastic glossyPlastic;

extern const Texture brushedMetal;
extern const Texture marbleCheckerboard;
extern const Texture blueWood;
extern const Texture swimmingFloor;
extern const Texture swimmingWall;
extern const Texture swimmingMarble;
extern const Texture emptyFrame;

// Creates the triangles for the floor
void createFloor(vector<const Shape *> shapes) {
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

void createWall(vector<const Shape *> shapes) {
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
void createGlossyCube(vector<const Shape *> shapes) {
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


void buildFirstScene(vector<const Shape *> shapes, int frameNumber)
{
	shapes.clear();															// DO WE NEED TO DELETE THE SPHERES?

	createFloor(shapes);
	createWall(shapes);
	createGlossyCube(shapes);

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
