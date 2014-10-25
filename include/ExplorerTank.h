#ifndef EXPLORERTANK_H
#define	EXPLORERTANK_H

#include <vector>
#include <math.h>
#include "Field.h"
#include "Vector2d.h"
#include "Protocol.h"
#include "Circle.h"
#include "Flag.h"

class Protocol;
struct GameConstants;

using namespace std;

class ExplorerTank {
private:
	int idx;
public:
	static Protocol protocol;
	//Stategy variables
	TankMode mode;
	//TODO: how to make this into a field, so we can have different types of goals
	vector<Flag*> goals;
	//Location and direction
	Vector2d loc, dir;
	//Velocity (linear and angular)
	Vector2d vel_linear;
	double vel_angular;
	
	ExplorerTank();
	ExplorerTank(int i);
	ExplorerTank(const ExplorerTank& orig);
	virtual ~ExplorerTank();
	
	//Decides which commands to give to the tank
	void evalPfield(GameConstants &gc,
		Grid g);
};

#endif
