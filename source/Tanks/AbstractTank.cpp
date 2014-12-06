#include "Protocol.h"
#include "Tanks/AbstractTank.h"
#include "math.h"

AbstractTank::AbstractTank(int i, Board *b){
	idx = i;
	board = b;
}
AbstractTank::~AbstractTank(){}

double AbstractTank::getAngVel(Vector2d angle){
	return getAngVel(atan2(angle[1], angle[0]));
}
double AbstractTank::getAngVel(double theta){
	double currentAngle = atan2(dir[1], dir[0]);	
	double angDiff = currentAngle - theta;
	while (angDiff < -M_PI)
		angDiff += 2*M_PI;
	while (angDiff > M_PI)
		angDiff -= 2*M_PI;
	return -angDiff/M_PI;
}
void AbstractTank::updateDynamics(double delta_t, float x, float y, float theta){
	//Set direction
	dir = Vector2d(cos(theta), sin(theta));
	//Starting position
	if (delta_t == 0){
		pos[0] = x;
		pos[1] = y;
		vel = Vector2d(0);
		acc = Vector2d(0);
	}
	//Kalman filter code
	//TODO
	
}
