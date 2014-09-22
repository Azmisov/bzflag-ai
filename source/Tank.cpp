#include "Tank.h"

void Tank::evalPfield(vector<Obstacle> obstacles)
{
	double pi = 3.1415926435;
	Vector2d result = new Vector2d();
	for (int i=0; i < obstacles.size(); i++)
	{
		result += obstacles[i].potentialField(loc,dir);
	}
	
	double desiredAngle = atan2(result[1], result[0]);
	double desiredMagnitude = result.length();
	double currentAngle = atan2(dir[1], dir[0]);
	
	p.speed(idx, desiredMagnitude);
	
	double angDiff = currentAngle - desiredAngle;
	while (angDiff < -1 * pi)
	{
		angDiff += (2*pi);
	}
	while (angDiff > pi)
	{
		angDiff -= (2*pi)
	}
	
	p.angvel(idx, angDiff/pi);
	
	
}
