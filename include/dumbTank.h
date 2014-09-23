#ifndef DUMBTANK_H
#define	DUMBTANK_H

#include <vector>
#include <math.h>
#include "Vector2d.h"
#include "Protocol.h"

class Protocol;

using namespace std;

class Tank {
private:
	int idx;
	bool turning;
public:
	static Protocol p;
	int nextMove;
	
	dumbTank();
	dumbTank(int i);
	dumbTank(const Tank& orig);
	virtual ~dumbTank();
	
	//Decides which commands to give to the tank
	void evalPfield();
};

#endif
