#include "Tank.h"

Protocol Tank::protocol = Protocol();
Tank::Tank(){}
Tank::Tank(int i){
	idx = i;
}
Tank::Tank(const Tank& orig){}
Tank::~Tank(){}

const Vector2d Tank::evalPfield(vector<Field> obstacles){
	double pi = 3.1415926435;
	Vector2d result = Vector2d(0);
	for (int i=0; i < obstacles.size(); i++){
		result += obstacles[i].potentialField(loc,dir);
	}
	
	double desiredAngle = atan2(result[1], result[0]);
	double desiredMagnitude = result.length();
	double currentAngle = atan2(dir[1], dir[0]);
	
	Tank::protocol.speed(idx, desiredMagnitude);
	
	double angDiff = currentAngle - desiredAngle;
	while (angDiff < -1 * pi){
		angDiff += (2*pi);
	}
	while (angDiff > pi){
		angDiff -= (2*pi);
	}
	
	Tank::protocol.angvel(idx, angDiff/pi);
	
	return result;
}
