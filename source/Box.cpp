#include "Box.h"

Box::Box(Vector2d min, Vector2d max){
	this->min = min;
	this->max = max;
}
Box::Box(const Box& orig){}
Box::~Box(){}

double Box::minX(){ return min[0]; }
double Box::maxX(){ return max[0]; }
double Box::minY(){ return min[1]; }
double Box::maxY(){ return max[1]; }
