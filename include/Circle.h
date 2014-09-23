#ifndef CIRCLE_H
#define	CIRCLE_H

#include <vector>
#include <math.h>
#include "Vector2d.h"
#include "Field.h"

class Field;

class Circle : public Field {	
public:
	Vector2d loc;
	double radius;

	Circle();
	Circle(bool attract);
	Circle(Vector2d l, double r);
	Circle(const Circle& orig);
	virtual ~Circle();
	
	//Compute area of shape
	double area();
	//Bounding box for shape
	void bounds(double bounds[4]);
	//Compute potential field of polygon at point
	const Vector2d potentialField(const Vector2d &station, const Vector2d &dir) const;
};

#endif

