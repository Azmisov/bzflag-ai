#ifndef CLAYPIGEON_H
#define CLAYPIGEON_H

#include "Tanks/AbstractTank.h"

class ClayPigeon : public AbstractTank{
private:
	bool stationary;
	double target_dir;
public:
	ClayPigeon(int i, Board *b) : AbstractTank(i, b){}
	
	virtual void coordinate(double delta_t);
	virtual void move(double delta_t);
};

#endif
