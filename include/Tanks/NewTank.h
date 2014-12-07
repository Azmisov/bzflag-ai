#ifndef NEWTANK_H
#define NEWTANK_H

#include "Tanks/AbstractTank.h"

class NewTank : public AbstractTank{
private:
	bool stationary;
	double target_dir;
public:
	NewTank(int i, Board *b) : AbstractTank(i, b){}
	
	virtual void coordinate(double delta_t);
	virtual void move(double delta_t);
};

#endif
