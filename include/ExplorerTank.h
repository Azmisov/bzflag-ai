#ifndef EXPLORERTANK_H
#define	EXPLORERTANK_H

#include <vector>
#include <math.h>
#include "Field.h"
#include "Vector2d.h"
#include "Protocol.h"
#include "Circle.h"
#include "Flag.h"
#include "Tank.h"
#include "Grid.h"
#include <stdlib.h>

class Protocol;
struct GameConstants;

using namespace std;

class ExplorerTank {
private:
	int idx;
public:
	static Protocol protocol;
	//Stategy variables
	//TankMode mode;
	//TODO: how to make this into a field, so we can have different types of goals
	//vector<Flag*> goals;
	//Location and direction
	Vector2d loc, dir;
	//Velocity (linear and angular)
	Vector2d vel_linear;
	double vel_angular;
	
	vector<Circle*> goals;
	vector<Circle*> obstacles;
	int SEARCH_RADIUS, SAMPLE_RATE;
	bool ROTATE;
	Vector2d old_loc;
	bool DONE_EXPLORING;
	
	ExplorerTank();
	ExplorerTank(int i);
	ExplorerTank(const ExplorerTank& orig);
	virtual ~ExplorerTank();
	
	//Decides which commands to give to the tank
	void evalPfield(int time, GameConstants &gc, Grid &g);
};

#endif
