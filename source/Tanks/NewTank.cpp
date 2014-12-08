#include "Tanks/NewTank.h"
#include "Protocol.h"
#include <cstdlib>
#include <math.h>

void NewTank::coordinate(double delta_t){
	int found = -1;
	//Assign an enemy tank to target
	for (int i=0; i<board->tanks.size(); i++){
		NewTank *t = dynamic_cast<NewTank*>(board->tanks[i]);
		if (t->target_tank == -1 || board->enemy_tanks[t->target_tank]->mode == DEAD){
			if (found == -1){
				for (int j=0; j<board->enemy_tanks.size(); j++){
					//Pick first tank that is not dead
					if (board->enemy_tanks[j]->mode != DEAD){
						found = j;
						break;
					}
				}
			}
			//Assign the tank
			t->target_tank = found;
			printf("Shooting at tank %d\n", found);
			if (found == -1) return;
		}
	}
}
void NewTank::move(double delta_t){\
	/*
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
	*/
	
	if (target_tank == -1)
		return;
	double ang_vel = aim(board->enemy_tanks[target_tank]);
	//printf("%f\n",ang_vel);
	//if (fabs(ang_vel) < .02)
	//	board->p->shoot(idx);
	//board->p->angvel(idx, ang_vel);
}

/*
double NewTank::aim(AbstractTank *enemy){
	no_intersect = false;
	//If we aren't intersecting the tank's path, always go towards the tank
	double magnitude = 1;
	bool toward_tank = true;
	//Temp vars
	double bspeed, bullet_t, tank_t = INFINITY, dist;
	//Find time when tank intersects bullet's path
	double intersections[4];
	for (int i=0; i <= 1; i++)
	{
		double v1 = enemy->acc[i];
		double v2 = enemy->vel[i]-dir[i];
		double v3 = enemy->pos[i]-pos[i];
		double t1 = v2*v2 - 2*v1*v3;
		if (t1 < 0 || fabs(v1) < 0.0001){
			printf("\tno isect %d, %f\n",i,v1);
			goto GETANGVEL;
		}
		t1 = sqrt(t1);
		intersections[i*2] = (-v2 + t1)/v1;
		intersections[i*2+1] = (-v2 - t1)/v1;
	}
	printf("\t(%f,%f) ? (%f,%f)\n",
		intersections[0],
		intersections[1],
		intersections[2],
		intersections[3]
	);
	if (intersections[0] > 0 || intersections[1] > 0){
		printf("found it!!!\n");
		double temp_t = intersections[intersections[1] > 0];
		enemy_pos = dir*temp_t + pos;
		//bullet_pos = dir*temp_t + pos;
	}
	for (int i=0; i <= 1; i++){
		for (int j=2; j <= 3; j++){
			if (fabs(intersections[i]-intersections[j]) < 100){
				double t = (intersections[i]+intersections[j])/2.0;
				//Pick the intersection that is closest to happening
				if (fabs(t) < fabs(tank_t))
					tank_t = t;
			}
		}
	}
	if (tank_t == INFINITY || tank_t < 0){
		printf("\tx and y do not meet %f\n",tank_t);
		goto GETANGVEL;
	}
	//Find time when bullet intersects tank's path
	bspeed = vel.length() + board->gc.shotspeed;
	bullet_t = tank_t/bspeed;
	
	//no_intersect = false;
	enemy_pos = dir*tank_t + pos;
	bullet_pos = dir*bullet_t + pos;
	
	dist = fabs(tank_t*(1-bspeed)) - (board->gc.tankradius+board->gc.shotradius);
	if (dist < 0) dist = 0;
	magnitude = dist/20.0;
	if (dist > 1) dist = 1;
	//This is the direction we should go to hit it
	toward_tank = bullet_t < tank_t;
	
	//Find which way we should rotate
	GETANGVEL:	
	double desiredAngle = atan2(enemy->pos[1]-pos[1], enemy->pos[0]-pos[0]);
	double currentAngle = atan2(dir[1], dir[0]);
	double angDiff = currentAngle - desiredAngle;
	while (angDiff < -M_PI)
		angDiff += (2*M_PI);
	while (angDiff > M_PI)
		angDiff -= (2*M_PI);
	int sign = angDiff > 0 ? 1 : -1;
	if (toward_tank)
		sign = -sign;
	return sign*angDiff/M_PI;
}
//*/

//*
double NewTank::aim(AbstractTank *e){
	no_intersect = true;
	Vector2d toward_tank = e->pos-pos;
	//If velocity is small, just aim for where tank is

	//Solve for x or y, depending on which gives more stable results
	int i = dir[0] > dir[1] ? 0 : 1, j = !i;
	//Compute intersection between tank's direction and the tank's path
	double m = dir[j]/dir[i],
		ax2 = e->acc[i]*e->acc[i],
		a_xy = e->acc.product(),
		jj = ax2*m - a_xy;
	//"a" term of quadratic
	double a = 2*jj*jj;
	//x intersection approaches infinity
	//if aligned with tank, we should shoot
	/*
	if (fabs(a) < EPSILON){
		printf("X is infinity, %f\n",a);
		return getAngVel(toward_tank) > 0 ? 1 : -1;
	}
	*/
	double bb = pos[j]-m*pos[i],	//y intercept of bullet line
		vx2 = e->vel[i]*e->vel[i],
		vy_ax = e->acc[i]*e->vel[j],
		k = ax2*(bb - e->pos[j]);
	k += vy_ax*e->vel[i];
	k -= vx2*e->acc[j];
	k += a_xy*e->pos[i];	
	double l = vy_ax - e->vel[i]*e->acc[j];
	l *= l;
	//We get a quadratic equation, with (a/2)x^2 + (-b)x + c = 0
	double lax = 2*l*e->acc[i],
		b = -2*j*k + lax,
		c = k*k - l*vx2 + lax*e->pos[i],
		r = b*b - 2*a*c;
	
	//Check for imaginary solutions
	if (fabs(r) < EPSILON) r = 0;
	else if (r < 0){
		printf("Imaginary, %f\n",r);
		return getAngVel(toward_tank);
	}
	else r = sqrt(r);
	Vector2d s1, s2;
	s1[i] = (b + r)/a;
	s2[i] = (b - r)/a;
	//Pick the solution that is closest to happening
	s1[j] = m*s1[i] + bb;
	s2[j] = m*s2[i] + bb;
	//Set intercept
	no_intersect = false;
	path[0] = m;
	path[1] = bb;
	enemy_pos = s1;
	bullet_pos = s2;
	
	//Find which way we should rotate
	return getAngVel(toward_tank);
}
//*/
