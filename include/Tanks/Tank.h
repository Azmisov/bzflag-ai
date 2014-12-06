#ifndef TANK_H
#define	TANK_H

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

class Tank {
private:
	int idx;
public:
	static Protocol protocol;
	//TODO: how to make this into a field, so we can have different types of goals
	vector<Flag*> goals;
	
	Tank();
	Tank(int i);
	Tank(const Tank& orig);
	virtual ~Tank();
	
	//Decides which commands to give to the tank
	void evalPfield(GameConstants &gc,
		Polygon &base,
		vector<Tank*> &tanks,
		vector<Flag*> &flags,
		vector<Tank*> &enemy_tanks,
		vector<Flag*> &enemy_flags,
		vector<Polygon*> &obstacles);
		
	const Vector2d potentialField(const Vector2d &station, const Vector2d &dir) const;
};

#endif
