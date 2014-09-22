#ifndef TANK_H
#define	TANK_H

#include "Vector2d.h"

//These are the only states we're allowed to use for tanks
enum TankMode {
	SPY_FETCH,		//assigned to fetch flag
	SPY_RETRIEVE,	//returning a flag to base
	OFFENSE,		//defending the spy
	DEFENSE,		//defending the flag
	IDLE,			//tank that just came alive and has not been assigned a mode
	DEAD			//tank is dead
};

class Tank {
public:
	//
	TankMode mode;
	//Location and direction
	Vector2d loc, dir;
	//Velocity (linear and angular)
	Vector2d vel_linear;
	double vel_angular;
	
	Tank();
	Tank(const Tank& orig);
	virtual ~Tank();
	
	//Compute potential field of polygon at point
	const Vector2d pfieldGravity() const;
};

#endif
