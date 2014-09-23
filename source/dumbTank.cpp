#include "dumbTank.h"

Protocol dumbTank::protocol = Protocol();
dumbTank::dumbTank(){}
dumbTank::dumbTank(int i){
	idx = i;
	nextMove = (((double)rand()/RAND_MAX)*5)+3;
	nextShot = ((double)rand()/RAND_MAX)+1.5;
	turning = false;
	dumbTank::protocol.speed(idx, 1.0);
}
dumbTank::dumbTank(const dumbTank& orig){}
dumbTank::~dumbTank(){}

void dumbTank::evalPfield(double seconds)
{
	if (seconds > nextMove && !turning)
	{
		turning = true;
		dumbTank::protocol.angvel(idx, 1);
		nextMove += 1;
	}
	else if (seconds > nextMove && turning)
	{
		nextMove += (((double)rand()/RAND_MAX)*5)+3;
		dumbTank::protocol.angvel(idx, 0);
		turning = false;
	}
	if (seconds > nextShot)
	{
		dumbTank::protocol.shoot(idx);
		nextShot += ((double)rand()/RAND_MAX)+1.5;
	}
	
	
}
