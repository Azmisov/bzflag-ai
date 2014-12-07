#ifndef WILDTANK_H
#define WILDTANK_H

#include "Tanks/AbstractTank.h"

class WildTank : public AbstractTank{
public:
	WildTank(int i, Board *b) : AbstractTank(i, b){}

	virtual void move(double delta_t);
};

#endif
