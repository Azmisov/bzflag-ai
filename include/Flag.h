#ifndef FLAG_H
#define	FLAG_H

#include <string>
#include "Vector2d.h"

class Flag {
public:
	static double radius;
	//Location
	Vector2d loc;
	//Velocity (linear and angular)
	bool isPossessed;
	bool havePosession;
	
	Flag();
	Flag(const Flag& orig);
	virtual ~Flag();
};

#endif
