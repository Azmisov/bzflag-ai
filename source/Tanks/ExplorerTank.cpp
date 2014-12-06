#include "ExplorerTank.h"

Protocol ExplorerTank::protocol = Protocol();
ExplorerTank::ExplorerTank(){
	SEARCH_RADIUS = 125;
	SAMPLE_RATE = 1200;
	ROTATE = false;
}
ExplorerTank::ExplorerTank(int i){
	idx = i;
	SEARCH_RADIUS = 125;
	SAMPLE_RATE = 1200;
	ROTATE = false;
}
ExplorerTank::ExplorerTank(const ExplorerTank& orig){
	idx = orig.idx;
	SEARCH_RADIUS = orig.SEARCH_RADIUS;
	SAMPLE_RATE = 1200;
	ROTATE = orig.ROTATE;
}
ExplorerTank::~ExplorerTank(){}

void ExplorerTank::evalPfield(int time, GameConstants &gc, Grid &g)
{
	double pi = 3.1415926435;
	Vector2d result = Vector2d(0);
	
	//ADJUST EXPLORING GOALS
	
	int MAX_OBST = 100;
	int half_w = g.getWidth()/2, half_h = g.getHeight()/2;
	
	if (time % 5 == 0){
		bool found_goal = !(time % 25 == 0);
		if (!found_goal){
			for (int i=0; i<goals.size(); i++)
				delete goals[i];
			goals.clear();
		}
		for (int i=0; i<obstacles.size(); i++)
			delete obstacles[i];
		obstacles.clear();
		
		int min_xs = loc[0]-SEARCH_RADIUS, min_ys = loc[1]-SEARCH_RADIUS;
		if (min_xs < -half_w) min_xs = -half_w;
		if (min_ys < -half_h) min_ys = -half_h;
		if (loc[0]+SEARCH_RADIUS > half_w)
			min_xs = half_w - 2*SEARCH_RADIUS;
		if (loc[1]+SEARCH_RADIUS > half_h)
			min_ys = half_h - 2*SEARCH_RADIUS;
		
		//Randomly sample
		int iters = 0;
		while (iters++ < SAMPLE_RATE){
			int val_x = rand() % (SEARCH_RADIUS*2) + min_xs;
			int val_y = rand() % (SEARCH_RADIUS*2) + min_ys;
			Vector2d pos = Vector2d(val_x, val_y);
			//printf("%d, %d\n", val_x, val_y);
			double value = g.get(val_y+half_h, val_x+half_w);
			if (!found_goal){
				if (value > 0.25 && value < 0.75){
					//printf("Pos = %f, %f -- Goal = %f, %f\n", loc[0], loc[1], pos[0], pos[1]);
					goals.push_back(new Circle(pos, 1.0, true));
					found_goal = true;
				}
			}
			if (obstacles.size() < MAX_OBST){
				if (value > 0.6)
					obstacles.push_back(new Circle(pos, 1.0, false));
			}
			else if (found_goal) break;
		}
		
		//Check positions for change
		if (!time) old_loc = loc;
		else ROTATE = (loc - old_loc).length() < 50;
	}
	
	//NAVIGATE
	
	for (int i=0; i < obstacles.size(); i++){
		result += obstacles[i]->potentialField(loc,dir);
	}
	
	if (goals.size())
		result -= 3*goals[0]->potentialField(loc,dir);
	else if (!DONE_EXPLORING){
		//increase search radius
		SEARCH_RADIUS += 100;
		SAMPLE_RATE += 500;
		DONE_EXPLORING = SEARCH_RADIUS > half_w || SEARCH_RADIUS > half_h;
		if (DONE_EXPLORING) printf("Tank #%d is done!!!\n", idx);
		if (SEARCH_RADIUS > half_w)
			SEARCH_RADIUS = half_w;
		if (SEARCH_RADIUS > half_h)
			SEARCH_RADIUS = half_h;
	}
	
	if (ROTATE){
		result = Vector2d(-result[1], result[0]);
		result += Vector2d(
			((double) rand() / (RAND_MAX)) - .5,
			((double) rand() / (RAND_MAX)) - .5
		);
	}
	
	double desiredAngle = atan2(result[1], result[0]);
	double desiredMagnitude = result.length();
	double currentAngle = atan2(dir[1], dir[0]);
	
	Tank::protocol.speed(idx, 1); //desiredMagnitude*.4
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
