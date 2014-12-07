#ifndef NEWTANK_H
#define NEWTANK_H

#include "Tanks/AbstractTank.h"

class NewTank : public AbstractTank{
private:
public:
	int target_tank = -1;
	Vector2d bullet_pos;
	Vector2d enemy_pos;
	bool no_intersect = true;
	
	NewTank(int i, Board *b) : AbstractTank(i, b){}
	
	virtual void coordinate(double delta_t);
	virtual void move(double delta_t);
	double aim(AbstractTank *enemy);
};

#endif
