#include "spaceship.h"

extern const Plastic plastic;
extern const Metal metal;
extern const GlossyPlastic glossyPlastic;

extern const Texture brushedMetal;
extern const Texture marbleCheckerboard;
extern const Texture blueWood;

const float floorLevel = 0;
const VEC3 center(5, 0.5, 1);	// Center of the spaceship

// Makes the floor of the spaceship
void createFloor(vector<const Shape*> &world) {
	// Create the floor
	for (int x = -6; x < 4; x+=2) {
		for (int z = -1; z < 3; z+=2) {
			Triangle *triangle1 = new Triangle(VEC3(x, floorLevel, z), VEC3(x, floorLevel, z+2), VEC3(x+2, floorLevel, z+2), metal, &marbleCheckerboard);//VEC3(0.5, 0.5, 0.5)));//VEC3(0.5, 0.5, 0.5), 10));
			Triangle *triangle2 = new Triangle(VEC3(x, floorLevel, z), VEC3(x+2, floorLevel, z), VEC3(x+2, floorLevel, z+2), metal, &marbleCheckerboard);//VEC3(0, 1, 0)));//VEC3(0, 1, 0), 10));

			triangle1->setTextureCoords(VEC2(0, 0), VEC2(1, 0), VEC2(1, 1));
			triangle2->setTextureCoords(VEC2(0, 0), VEC2(0, 1), VEC2(1, 1));

			world.push_back(triangle1);
			world.push_back(triangle2);
		}
	}
}

// Creates all objects in the spaceship, adding them to the world vector
//	note world vector may already contain other shapes
void createWalls(vector<const Shape*> &world) {
	VEC3 entranceColour(1, 0, 0);
	//VEC3 entranceColour = VEC3(182.0/255.0, 184.0/255.0, 191.0/255.0);

	// Create entrance corridor
	float distFromCenter = 0.4;		// Horizontal distance of walls from center
	float entranceLevel = -2.5;		// Depth of start of spaceship entrance
	float entranceLength = 1;		// Length of corridor into spaceship
	float entranceHeight = 1;
	//world.push_back(new Triangle(VEC3(entranceLevel, floorLevel, center[2] - distFromCenter), VEC3(entranceLevel + entranceLength, floorLevel, center[2] - distFromCenter), VEC3(entranceLevel, floorLevel + entranceHeight, center[2] - distFromCenter), metal, entranceColour));
	//world.push_back(new Triangle(VEC3(entranceLevel + entranceLength, floorLevel, center[2] - distFromCenter), VEC3(entranceLevel, floorLevel + entranceHeight, center[2] - distFromCenter), VEC3(entranceLevel + entranceLength, floorLevel + entranceHeight, center[2] - distFromCenter), metal, entranceColour));
	//world.push_back(new Triangle(VEC3(entranceLevel, floorLevel, center[2] + distFromCenter), VEC3(entranceLevel + entranceLength, floorLevel, center[2] + distFromCenter), VEC3(entranceLevel, floorLevel + entranceHeight, center[2] + distFromCenter), metal, entranceColour));
	//world.push_back(new Triangle(VEC3(entranceLevel + entranceLength, floorLevel, center[2] + distFromCenter), VEC3(entranceLevel, floorLevel + entranceHeight, center[2] + distFromCenter), VEC3(entranceLevel + entranceLength, floorLevel + entranceHeight, center[2] + distFromCenter), metal, entranceColour));
	
	// Create first walls
	// The spaceship "joints" are the places where two walls connect, at floor level
	VEC3 joint1Left(entranceLevel+entranceLength, floorLevel, center[2] - distFromCenter);	// First vertex of the spaceship on the left
	VEC3 joint1Right(entranceLevel+entranceLength, floorLevel, center[2] + distFromCenter);	// First vertex of the spaceship on the right
	float wall1Outwards = 0.7;	// How much the first wall moves outwards
	float wall1Depth = 1;	// How deep the first wall goes
	float wall1Height = entranceHeight;
	Triangle *triangle1 = new Triangle(joint1Left, VEC3(joint1Left[0]+wall1Depth, joint1Left[1], joint1Left[2]-wall1Outwards), VEC3(joint1Left[0], joint1Left[1]+wall1Height, joint1Left[2]), metal, &marbleCheckerboard);
	Triangle *triangle2 = new Triangle(VEC3(joint1Left[0]+wall1Depth, joint1Left[1], joint1Left[2]-wall1Outwards), VEC3(joint1Left[0], joint1Left[1]+wall1Height, joint1Left[2]), VEC3(joint1Left[0]+wall1Depth, joint1Left[1]+wall1Height, joint1Left[2]-wall1Outwards), metal, &marbleCheckerboard);
	
	triangle1->setTextureCoords(VEC2(0, 0), VEC2(1, 0), VEC2(1, 1));
	triangle2->setTextureCoords(VEC2(0, 0), VEC2(0, 1), VEC2(1, 1));

	world.push_back(triangle1);
	world.push_back(triangle2);	

	world.push_back(new Triangle(joint1Right, VEC3(joint1Right[0]+wall1Depth, joint1Right[1], joint1Right[2]+wall1Outwards), VEC3(joint1Right[0], joint1Right[1]+wall1Height, joint1Right[2]), metal, entranceColour));
	world.push_back(new Triangle(VEC3(joint1Right[0]+wall1Depth, joint1Right[1], joint1Right[2]+wall1Outwards), VEC3(joint1Right[0], joint1Right[1]+wall1Height, joint1Right[2]), VEC3(joint1Right[0]+wall1Depth, joint1Right[1]+wall1Height, joint1Right[2]+wall1Outwards), metal, entranceColour));

	// Create second walls
	VEC3 joint2Left(joint1Left[0]+wall1Depth, floorLevel, joint1Left[2]-wall1Outwards);	// First vertex of the spaceship on the left
	VEC3 joint2Right(joint1Right[0]+wall1Depth, floorLevel, joint1Right[2]+wall1Outwards);	// First vertex of the spaceship on the right
	float wall2Outwards = 1.3;	// How much the first wall moves outwards
	float wall2Depth = 0.8;	// How deep the first wall goes
	float wall2Height = entranceHeight;
	Triangle *triangle3 = new Triangle(joint2Left, VEC3(joint2Left[0]+wall2Depth, joint2Left[1], joint2Left[2]-wall2Outwards), VEC3(joint2Left[0], joint2Left[1]+wall2Height, joint2Left[2]), metal, &marbleCheckerboard);
	Triangle *triangle4 = new Triangle(VEC3(joint2Left[0]+wall2Depth, joint2Left[1], joint2Left[2]-wall2Outwards), VEC3(joint2Left[0], joint2Left[1]+wall2Height, joint2Left[2]), VEC3(joint2Left[0]+wall2Depth, joint2Left[1]+wall2Height, joint2Left[2]-wall2Outwards), metal, &marbleCheckerboard);
	
	triangle3->setTextureCoords(VEC2(0, 0), VEC2(1, 0), VEC2(1, 1));
	triangle4->setTextureCoords(VEC2(0, 0), VEC2(0, 1), VEC2(1, 1));

	world.push_back(triangle3);
	world.push_back(triangle4);	

	world.push_back(new Triangle(joint2Right, VEC3(joint2Right[0]+wall2Depth, joint2Right[1], joint2Right[2]+wall2Outwards), VEC3(joint2Right[0], joint2Right[1]+wall2Height, joint2Right[2]), metal, entranceColour));
	world.push_back(new Triangle(VEC3(joint2Right[0]+wall2Depth, joint2Right[1], joint2Right[2]+wall2Outwards), VEC3(joint2Right[0], joint2Right[1]+wall2Height, joint2Right[2]), VEC3(joint2Right[0]+wall2Depth, joint2Right[1]+wall2Height, joint2Right[2]+wall2Outwards), metal, entranceColour));

}


// Creates all objects in the spaceship, adding them to the world vector
//	note world vector may already contain other shapes
void createSpaceship(vector<const Shape*> &world) {
	createFloor(world);
	createWalls(world);
}
