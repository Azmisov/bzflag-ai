#include "Tanks/NewTank.h"
#include "Protocol.h"
#include <cstdlib>
#include <math.h>

void NewTank::coordinate(double delta_t){
	return;
	}
}
void NewTank::move(double delta_t){
	double pi = 3.1415926435;
	Vector2d result = Vector2d(0);
	
	for (int i=0; i < board->obstacles.size(); i++){
		result += 0.4*board->obstacles[i]->potentialField(loc,dir);
	}

	result -= 60*board->goals[board->goals.size()-1]->potentialField(loc,dir);	
	
	double desiredAngle = atan2(result[1], result[0]);
	double desiredMagnitude = result.length();
	double currentAngle = atan2(dir[1], dir[0]);
	
	Tank::protocol.speed(idx, 1);
	
	double angDiff = currentAngle - desiredAngle;
	while (angDiff < -1 * pi){
		angDiff += (2*pi);
	}
	while (angDiff > pi){
		angDiff -= (2*pi);
	}
	
	Tank::protocol.angvel(idx, -angDiff/pi);
	Tank::protocol.shoot(idx);
}
