#include "Tank.h"

Protocol Tank::protocol = Protocol();
Tank::Tank(){}
Tank::Tank(int i){
	idx = i;
}
Tank::Tank(const Tank& orig){}
Tank::~Tank(){}

void Tank::evalPfield(GameConstants &gc,
	Polygon &base,
	vector<Tank*> &tanks,
	vector<Flag*> &flags,
	vector<Tank*> &enemy_tanks,
	vector<Flag*> &enemy_flags,
	vector<Polygon*> &obstacles
){			
	double pi = 3.1415926435;
	Vector2d result = Vector2d(0);
	
	for (int i=0; i < obstacles.size(); i++){
		result += 0.4*obstacles[i]->potentialField(loc,dir);
	}
	/*for (int i=0; i < enemy_tanks.size(); i++)
	{
		result += enemy_tanks[i]->potentialField(loc,dir);
	}
	for (int i=0; i < enemy_flags.size(); i++)
	{
		result -= 30 * enemy_flags[i]->potentialField(loc,dir);
		cout << enemy_flags[i]->loc[0] << ", " << enemy_flags[i]->loc[1] << endl;
	}
	*/
	result -= 30*goals[goals.size()-1]->potentialField(loc,dir);	
	
	double desiredAngle = atan2(result[1], result[0]);
	double desiredMagnitude = result.length();
	double currentAngle = atan2(dir[1], dir[0]);
	
	Tank::protocol.speed(idx, desiredMagnitude*.4);
	//cout << desiredMagnitude *.1 << endl;
	
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

const Vector2d Tank::potentialField(const Vector2d &station, const Vector2d &dir) const
{
	Circle c = new Circle(loc, 3, false);
	return c.potentialField(station, dir);
}
