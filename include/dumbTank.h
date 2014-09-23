#ifndef DUMBTANK_H
#define	DUMBTANK_H

#include <vector>
#include <math.h>
#include "Vector2d.h"
#include "Protocol.h"

class Protocol;

using namespace std;

class dumbTank {
private:
	int idx;
	bool turning;
public:
	static Protocol protocol;
	double nextMove;
	double nextShot;
	
	dumbTank();
	dumbTank(int i);
	dumbTank(const dumbTank& orig);
	virtual ~dumbTank();
	
	//Decides which commands to give to the tank
	void evalPfield(double);
};

#endif
