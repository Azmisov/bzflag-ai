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
				double best_distance = INFINITY;
				for (int j=0; j<board->enemy_tanks.size(); j++){
					//Pick first tank that is not dead
					if (board->enemy_tanks[j]->mode != DEAD){
						double dist = (t->pos - board->enemy_tanks[j]->pos).length_squared();
						if (dist < best_distance){
							best_distance = dist;
							found = j;
						}
						//break;
					}
				}
			}
			//Assign the tank
			t->target_tank = found;
			if (found == -1) return;
		}
	}
}

void NewTank::move(double delta_t){
	Vector2d result = Vector2d(0);
	/*
	//Get best tank direction
	int obs_count = 0;
	for (int i=0; i < board->obstacles.size(); i++){
		//Only use this obstacle if it obstructs path to goal
		result += 0.4*board->obstacles[i]->potentialField(loc,dir);
		obs_count++;
	}
	//Attacking a tank
	if (!obs_cont && ){
	*/
		if (target_tank == -1)
			return;
		double ang_vel = aim(board->enemy_tanks[target_tank]);
		if (fabs(ang_vel) < .02){
			board->p->shoot(idx);
		}
		board->p->angvel(idx, ang_vel);
		//Speed proportional to angular speed
		board->p->speed(idx, .3); //fabs(ang_vel)
	/*
	}
	//Navigating
	else{
		result -= 60*board->goals[board->goals.size()-1]->potentialField(loc,dir);
		double ang_vel = getAngVel(result);
		board->p->angvel(idx, ang_vel);
		//Speed inversely proportional to angular speed
		board->p->speed(idx, 1-ang_vel);
	}
	*/
}

double NewTank::aim(AbstractTank *e){
	no_intersect = true;
	
	//If tank is not moving, just aim directly at it
	if (e->vel.length() < .5)
		return getAngVel(e->pos - pos);


	no_intersect = false;
	
	//Estimate the tank's future position, as a fallback
	//Extract component speed of bullet and use that to get time value for integration
	Vector2d towards_tank = e->pos - pos;
	towards_tank.normalize();
	int bidx = fabs(towards_tank[0]) > fabs(towards_tank[1]) ? 0 : 1;
	double bullet_speed = vel.length() + board->gc.shotspeed;
	double future_t = fabs((e->pos[bidx] - pos[bidx])/(towards_tank[bidx]*bullet_speed));
	Vector2d future_pos = e->pos + future_t*e->vel + 0.5*future_t*future_t*e->acc;
	double base_vel = getAngVel(future_pos - pos);
	int base_sgn = base_vel > 0 ? 1 : -1;

	tank_pos = future_pos;
	bullet_pos = e->pos;
	
	//Solve for x or y, depending on which gives more stable results
	int i = dir[0] > dir[1] ? 0 : 1, j = !i;
	double ax = e->acc[i], ay = e->acc[j];
	//Compute intersection between tank's direction and the tank's path
	double m = dir[j]/dir[i],
		ax2 = ax*ax,
		a_xy = ax*ay,
		jj = ax2*m - a_xy;
	//"a" term of quadratic
	double a = 2*jj*jj;
	double bb = pos[j]-m*pos[i],	//y intercept of bullet line
		vx2 = e->vel[i]*e->vel[i],
		vy_ax = ax*e->vel[j],
		k = ax2*(bb - e->pos[j]);
	k += vy_ax*e->vel[i];
	k -= vx2*ay;
	k += a_xy*e->pos[i];	
	double l = vy_ax - e->vel[i]*ay;
	l *= l;
	//We get a quadratic equation, with (a/2)x^2 + (-b)x + c = 0
	double lax = 2*l*ax,
		b = -2*jj*k + lax,
		c = k*k - l*vx2 + lax*e->pos[i],
		r = b*b - 2*a*c;
	//Check for imaginary solutions
	if (fabs(r) < 1e-3) r = 0;
	else if (r < 0)
		return base_vel*2;
	else r = sqrt(r);
	Vector2d isect[2];
	//Switch to log space, if b+r is very small or a is very large
	if (fabs(b+r) < 1e-3 || a > 1e6){
		int sign_a = a > 0 ? 1 : -1,
			sign_bp = b+r > 0 ? 1 : -1,
			sign_bm = b-r > 0 ? 1 : -1;
		double a_ln = log(fabs(a)),
			bp_ln = log(fabs(b+r)),
			bm_ln = log(fabs(b-r));
		isect[0][i] = exp(bp_ln-a_ln)*sign_bp*sign_a;
		isect[1][i] = exp(bm_ln-a_ln)*sign_bm*sign_a;
	}
	else{
		isect[0][i] = (b + r)/a;
		isect[1][i] = (b - r)/a;
	}
	isect[0][j] = m*isect[0][i] + bb;
	isect[1][j] = m*isect[1][i] + bb;
	//Calculate time for tank to reach isect
	int best_isect = 0;
	double tank_t = INFINITY;
	for (int ii=0; ii<2; ii++){
		double ta = e->acc[0] - e->acc[1],
			tb = e->vel[1] - e->vel[0],
			tc = e->pos[0] - isect[ii][0] - e->pos[1] + isect[ii][1],
			tr = tb*tb - 2*ta*tc;
		if (tr < EPSILON) tr = 0;
		else if (tr < 0) continue;
		else tr = sqrt(tr);
		//sol #1
		double s = (tb+tr)/ta;
		if (fabs(s) < fabs(tank_t)){
			double err = (isect[ii] - (e->pos + e->vel*s + e->acc*s*s/2.0)).length();
			if (err < 1){
				tank_t = s;
				best_isect = ii;
			}
		}
		//sol #2
		s = (tb-tr)/ta;
		if (fabs(s) < fabs(tank_t)){
			double err = (isect[ii] - (e->pos + e->vel*s + e->acc*s*s/2.0)).length();
			if (err < 1){
				tank_t = s;
				best_isect = ii;
			}
		}
	}
	//Sensitivity factor
	Vector2d tank_dir = e->pos - isect[best_isect];
	double theta = fabs(acos(tank_dir.dot(-dir)/tank_dir.length()));
	base_sgn = tank_dir.dot(Vector2d(-dir[1],dir[0])) > 0 ? -1 : 1;
	
	double error;
	Vector2d bullet_dir = isect[best_isect]-pos;
	if (bullet_dir.dot(dir) < 0 || tank_t < 0)
		return base_vel*3;
	else{
		//Calculate new position for bullet
		Vector2d offset = (dir*bullet_speed*tank_t);		
		double tdist = bullet_dir.length(),
			bdist = offset.length();
		error = tdist - bdist;
		//Undershoot, need to go away from tank
		if (error < 0)
			base_sgn = -base_sgn;
		//Get magnitude of turn angle
		error = fabs(error) - board->gc.tankradius - board->gc.shotradius;
		if (error < 0) error = 0;
		error = error/250.0;
		if (error > 1) error = 1;
		
		bullet_pos = offset + pos; 
		tank_pos = isect[best_isect];
	}
	if (theta > M_PI_2) theta = M_PI-theta;
	error *= theta*M_2_PI;
	//Best angvel
	return error*base_sgn;
}
