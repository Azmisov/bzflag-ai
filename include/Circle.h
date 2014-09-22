#ifndef POLYGON_H
#define	POLYGON_H

#include <vector>
#include <math.h>
#include "Vector2d.h"
#include "Obstacle.h"

#define EPSILON 1e-5
#define PI_2 6.28318530718
#define PI 3.141592
//30: 0.86602540378 0.5
//45: 0.70710678118 0.70710678118
#define COS_TANGENTIAL 0.86602540378
#define SIN_TANGENTIAL .5
#define TANG_THRESHOLD .3

class Circle : public Obstacle {
private:
	double area_cache;
	Vector2d position;
	double radius;
	
public:
	Circle();
	Circle(Vector2d position, double radius);
	virtual ~Polygon();
		
	
	//Compute area of shape
	double area();
	//Bounding box for shape
	void bounds(double bounds[4]);
	//Compute potential field of polygon at point
	const Vector2d potentialField(const Vector2d &station, const Vector2d &dir) const;
};

#endif

