#include "physicsWorld.h"

PhysicsWorld::PhysicsWorld(const vector<const Shape*> &shapes) 
	: shapes(shapes)
{}

bool PhysicsWorld::existsClosestIntersection(const Ray &ray, const Shape *&intersectShape, VEC3 &point) const {
	float closestTime = -1;  // the t of the closest intersection             // CAN THIS EVER FAIL???

	// Check intersection for each shape
	for (const Shape *shape : shapes) {											// CAN WE MAKE THIS MORE EFFICIENT??
		// Check if ray intersects with the shape
		float latestPoint = 0;
		bool didIntersect = shape->intersects(ray, latestPoint);
		bool isClosest = (closestTime == -1) or latestPoint < closestTime;

		// Save intersection
		if (didIntersect and isClosest) {
			//cout << latestPoint << endl;;
			closestTime = latestPoint;
			intersectShape = shape;
		}
	}

	// Calculate intersection point
	if (closestTime != -1) {
		point = ray.o + closestTime * ray.d;
		return true;
	}
	return false;
}