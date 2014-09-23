#include "dumbTank.h"

dumbTank::dumbTank(int i){
	idx = i;
	nextMove = (((double)rand()/RAND_MAX)*5)+3;
	turning = false;
	p.speed(idx, 1.0);
}

void dumbTank::evalPfield(double seconds)
{
	if (seconds > nextMove && !turning)
	{
		turning = true;
		p.angvel(idx, 1);
		nextMove += 1;
	}
	else if (seconds > nextMove && turning)
	{
		nextMove += (((double)rand()/RAND_MAX)*5)+3;
		p.angvel(idx, 0);
		turning = false;
	}
	
	
}
