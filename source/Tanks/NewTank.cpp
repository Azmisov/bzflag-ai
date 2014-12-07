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
}

double aim(AbstractTank enemy)
{
	//Find time when tank intersects bullet's path
	double[] intersections = new double[4];
	for (int i=0; i <= 1; i++)
	{
		double v1 = enemy->acc[i];
		double v2 = enemy->vel[i]-dir[i];
		double v3 = enemy->pos[i]-pos[i];
		double t1 = v2*v2 - 2*v1*v3;
		if (t1 < 0 || fabs(v1) > 0.0001)
			return M_INFINITY;
		t1 = Math.sqrt(t1);
		intersections[i*2] = (-v2 + t1)/v1;
		intersections[i*2+1] = (-v2 - t1)/v1;
	}
	double tank_t = M_INFINITY;
	for (int i=0; i <= 1; i++)
		for (int j=2; j <= 3; j++){
			if (fabs(intersections[i]-intersections[j]) < 0.05){
				double t = (intersections[i]+intersections[j])/2.0;
				//Pick the intersection that is closest to happening
				if (fabs(t) < fabs(tank_t))
					tank_t = t;
			}
		}
	}
	if (best_t == M_INFINITY || best_t < 0)
		return best_t;
	//Find time when bullet intersects tank's path
	double bspeed = vel.length() + board->gc.shotspeed;
	double bullet_t = tank_t/bspeed;
	double dist = fabs(tank_t*(1-bspeed)) - (board->gc.tankradius+board->gc.shotradius);
	if (dist < 0) dist = 0;
	double magnitude = dist/20.0;
	if (dist > 1) dist = 1;
	//Find which way we should rotate
	double desiredAngle = atan2(enemy->pos[1], enemy->pos[0]);
	double currentAngle = atan2(dir[1], dir[0]);
	double angDiff = currentAngle - desiredAngle;
	while (angDiff < -M_PI)
		angDiff += (2*M_PI);
	while (angDiff > M_PI)
		angDiff -= (2*M_PI);
	//sign = away from tank
	//-sign = towards tank
	int sign = angDiff > 0 ? 1 : -1;
	if (bullet_t < tank_t)
		sign = -sign;
	return sign*magnitude;
}
