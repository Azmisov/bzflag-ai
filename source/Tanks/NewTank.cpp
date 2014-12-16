#include "Tanks/NewTank.h"
#include "Protocol.h"
#include <cstdlib>
#include <math.h>

void NewTank::initialize(){
	//This tank coordinates all others
	//memset(jobs, 0, 3*sizeof(char));
}
void NewTank::coordinate(double delta_t){
	double range = board->gc.shotrange/2.0,
		range2 = range*range;
	//Count enemy tanks
	bool has_enemy = false;
	for (int j=0; j<board->enemy_tanks.size(); j++){
		//Pick first tank that is not dead
		if (board->enemy_tanks[j]->mode != DEAD){
			has_enemy = true;
			break;
		}
	}
	//Count our tanks
	char jobs[3] = {0,0,0};
	for (int i=0; i<board->tanks.size(); i++){
		switch(board->tanks[i]->mode){
			case SPY_FETCH:
			case SPY_RETRIEVE:
				jobs[0]++;
				break;
			case OFFENSE:
				jobs[1]++;
				break;
			case DEFENSE:
				jobs[2]++;
				break;
		}
	}
	//Assign an enemy tank to target
	for (int i=0; i<board->tanks.size(); i++){
		NewTank *t = dynamic_cast<NewTank*>(board->tanks[i]);
		//If any tanks get a flag, we reassign them to be spies
		if (t->has_flag){
			t->mode = SPY_RETRIEVE;
			t->target_tank = -1;
		}
		//If no more enemies, send everyone to go get flags (either spy or offense)
		else if (!has_enemy && t->mode != OFFENSE)
			t->mode = IDLE;
		//Handle goal/strategy changes
		double best_d, d;
		bool defend_flag;
		Flag *f;
		REASSIGN:
		switch (t->mode){
			/*
				- fetch a flag
				- if someone else grabbed the flag, go into offense mode
				- if our own team grabbed it, pick a different flag
					if no other flags, go into offense
			*/
			case SPY_FETCH:
				//Someone else grabbed this flag
				f = NULL;
				if (t->target_flag == -1 || (f = board->enemy_flags[t->target_flag])->havePossession){
					//Pick a flag that is closest
					t->target_flag = -1;
					best_d = INFINITY;
					for (int j=0; j<board->enemy_flags.size(); j++){
						if (!board->enemy_flags[j]->isPossessed){
							d = (board->enemy_flags[j]->loc - t->pos).length_squared();
							if (d < best_d){
								best_d = d;
								t->target_flag = j;
							}
						}
					}
					//No flags to pick from, switch to offensive mode
					if (best_d == INFINITY){
						t->mode = OFFENSE;
						goto REASSIGN;
					}
				}
				//Attack the tank that has this flag
				else if (f->isPossessed){
					t->mode = OFFENSE;
					goto REASSIGN;
				}
				break;
			// Successfully returned flag
			case SPY_RETRIEVE:
				if (!t->has_flag)
					t->mode = SPY_RETRIEVE;
					break;
			/* pick an enemy flag; attack all enemies within shotradius
				distance of that flag; if no enemies, go directly for
				the flag
			*/
			case OFFENSE:
				//No need to defend our flag
				if (t->defending && !has_enemy){
					t->defending = false;
					t->target_flag = -1;
				}
				//Assign a flag to attack
				if (t->target_flag == -1){
					best_d = INFINITY;
					for (int j=0; j<board->enemy_flags.size(); j++){
						d = (board->enemy_flags[j]->loc - t->pos).length_squared();
						if (d < best_d){
							best_d = d;
							t->target_flag = j;
						}
					}
				}
				f = t->defending ? board->flags[t->target_flag] : board->enemy_flags[t->target_flag];
				defend_flag = f->isPossessed && !f->havePossession;
				//Assign an enemy to attack
				//Discard previous enemy, if it would be difficult to hit
				if (t->target_tank != -1){
					AbstractTank *e = board->enemy_tanks[t->target_tank];
					if (e->mode == DEAD || (e->pos-f->loc).length_squared() > range2)
						t->target_tank = -1;
				}
				if (t->target_tank == -1){
					//If the flag is possessed, pick enemies that are closest to the flag
					//Otherwise, pick the one that we have the best chance of aiming at
					Vector2d target = defend_flag ? f->loc : t->pos;
					best_d = 2*range2;
					for (int j=0; j<board->enemy_tanks.size(); j++){
						AbstractTank *e = board->enemy_tanks[j];
						if (e->mode != DEAD){
							Vector2d diff = t->pos - e->pos;
							d = (e->pos-f->loc).length_squared() + diff.length_squared();
							if (d < best_d){
								//If we aren't defending a flag, only fight enemies that we are facing
								if (!t->defending){
									if (diff.dot(Vector2d(-t->dir[1],t->dir[0])) < 0)
										continue;
								}
								best_d = d;
								t->target_tank = j;
							}
						}
					}
				}
				break;
			/*
				- hide out in a safe place close to the base
				- if there are no safe places, circle our flag
				- if we see anybody approaching, go into offensive mode and shoot them
				- if a flag has been taken, retrieve it
			*/
			case DEFENSE:
				//If we don't have anyone going for a flag, reassign to be a spy
				if (jobs[0] < board->enemy_flags.size()){
					jobs[0]++;
					t->mode = SPY_FETCH;
					goto REASSIGN;
				}
				//TODO: more intelligent defending
				t->defending = true;
				t->target_flag = 0;
				t->mode = OFFENSE;
				goto REASSIGN;
			//Assign strategy modes
			case IDLE:
				//At least as many spies as there are enemy flags
				if (jobs[0] < board->enemy_flags.size()){
					jobs[0]++;
					t->target_tank = -1;
					t->mode = SPY_FETCH;
				}
				//Offense and defense should have roughly equal resources
				else{
					if (jobs[1] > jobs[2]){
						jobs[2]++;
						t->mode = DEFENSE;
					}
					else{
						jobs[1]++;
						t->mode = OFFENSE;
					}
				}
				goto REASSIGN;
		}
	}
}

void NewTank::move(double delta_t){
	//Wait until tank is resurrected or assigned strategy
	if (mode == DEAD || mode == IDLE || mode == DEFENSE)
		return;
		
	double range = board->gc.shotrange/2.0;
	range *= range;

	//Flag reference
	Flag *flag = defending ? board->flags[target_flag] : board->enemy_flags[target_flag];
	//Go in circles, if offensively defending a flag
	if (mode == OFFENSE && target_tank == -1 && (flag->havePossession || defending)){
		double dist_flag = (flag->loc-pos).length_squared();
		if (dist_flag < range/4.0){
			board->p->angvel(idx, 1);
			board->p->speed(idx, 1);
			board->p->shoot(idx);
			return;
		}
	}
	//Get our target, be it flag or enemy
	Vector2d target;
	AbstractTank *enemy;
	Field *goal;
	if (target_tank == -1){
		if (mode == SPY_RETRIEVE){
			Polygon &p = board->base;
			target = p.centroid();
			goal = &p;
		}
		else{
			target = flag->loc;
			goal = flag;
		}
	}
	else{
		enemy = board->enemy_tanks[target_tank];
		target = enemy->pos;
	}
	//Get potential fields for obstacles
	Vector2d result = Vector2d(0);
	int obs_count = 0;
	for (int i=0; i < board->obstacles.size(); i++){
		//Only use this obstacle if it obstructs path to goal
		if (board->obstacles[i]->isect(pos, target)){
			result += 0.1*board->obstacles[i]->potentialField(pos, dir);
			obs_count++;
		}
	}
	obstacles = !!obs_count;
	double ang_vel = target_tank == -1 ? 0 : aim(enemy);
	//Navigating
	if (target_tank == -1 || obs_count){
		//Go towards flag
		if (target_tank == -1){
			if (obs_count){
				Vector2d vdir = dir;
				if (mode == SPY_RETRIEVE) vdir = -vdir;
				result += 60*goal->potentialField(pos,vdir);
				ang_vel = (ang_vel*2 + getAngVel(result))/3.0;
			}
			else{
				Vector2d vdir = target-pos;
				if (mode == SPY_RETRIEVE) vdir = -vdir;
				ang_vel = getAngVel(vdir);
			}
		}
		board->p->angvel(idx, ang_vel);
		//Speed inversely proportional to angular speed
		double speed = 1-ang_vel;
		if (mode == SPY_RETRIEVE) speed = -speed;
		board->p->speed(idx, speed);
		//Always shoot
		board->p->shoot(idx);
	}
	//Attacking a tank
	else{
		if (fabs(ang_vel) < .04)
			board->p->shoot(idx);
		board->p->angvel(idx, ang_vel);
		//Speed proportional to angular speed, if distance is small
		double distance = (enemy->pos - pos).length_squared();
		if (distance > range/4.0)
			board->p->speed(idx, 1);
		else if (distance > range/8.0)
			board->p->speed(idx, fabs(ang_vel));
		else board->p->speed(idx, .1);
	}
}

double NewTank::aim(AbstractTank *e){
	//no_intersect = true;
	
	//If tank is not moving, just aim directly at it
	if (e->vel.length() < .5)
		return getAngVel(e->pos - pos);

	//no_intersect = false;
	
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

	//tank_pos = future_pos;
	//bullet_pos = e->pos;
	
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
		
		//bullet_pos = offset + pos; 
		//tank_pos = isect[best_isect];
	}
	if (theta > M_PI_2) theta = M_PI-theta;
	error *= theta*M_2_PI;
	//Best angvel
	return error*base_sgn;
}
