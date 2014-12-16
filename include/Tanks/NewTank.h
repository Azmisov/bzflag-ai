#ifndef NEWTANK_H
#define NEWTANK_H

#include "Tanks/AbstractTank.h"

class NewTank : public AbstractTank{
public:
	int target_tank = -1;
	int target_flag = -1;
	bool defending = false;
	bool obstacles = false;

	NewTank(int i, Board *b) : AbstractTank(i, b){}
	
	virtual void initialize();
	virtual void coordinate(double delta_t);
	virtual void move(double delta_t);
	double aim(AbstractTank *enemy);
};

#endif
