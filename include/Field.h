#ifndef FIELD_H
#define	FIELD_H

#include <vector>
#include <math.h>
#include "Vector2d.h"

#define EPSILON 1e-5
#define PI 3.14159265359
#define PI_2 6.28318530718
//30: 0.86602540378 0.5
//45: 0.70710678118 0.70710678118
#define COS_TANGENTIAL 0.86602540378
#define SIN_TANGENTIAL .5
#define TANG_THRESHOLD 25

class Field {
private:
	double area_cache;
	std::vector<Vector2d> vertices;
	
public:
	//Is this field an attractive field?
	bool attractive;
	
	Field(){ attractive = true; }
	Field(bool attract){ attractive = attract; }
	//Compute potential field of polygon at point
	const Vector2d potentialField(const Vector2d &station, const Vector2d &dir) const{}
};

#endif
