#include "Tanks/AbstractTank.h"

AbstractTank::AbstractTank(int i, Board *b){
	idx = i;
	board = b;
}
AbstractTank::~AbstractTank(){}

AbstractTank::updateDynamics(double delta_t, float x, float y, float theta){
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
