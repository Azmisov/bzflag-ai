#ifndef BOX_H
#define	BOX_H

#include "Vector2d.h"

class Box {
private:
	Vector2d min, max;
	
public:
	Box(Vector2d min, Vector2d max);
	Box(const Box& orig);
	virtual ~Box();
	
	//Size access
	double minX();
	double maxX();
	double minY();
	double maxY();
	
	//Bounding box contains point?
	bool contains(const Vector2d &p) const;
	bool contains(double x, double y) const;
	
	//Intersection tests
	bool intersectBox(const Box &b) const;
	bool intersectRay(const Vector2d &r) const;
};

#endif
