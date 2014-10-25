#include "ExplorerTank.h"

Protocol ExplorerTank::protocol = Protocol();
ExplorerTank::ExplorerTank(){}
ExplorerTank::ExplorerTank(int i){
	idx = i;
}
ExplorerTank::ExplorerTank(const ExplorerTank& orig){}
ExplorerTank::~ExplorerTank(){}

void ExplorerTank::evalPfield(GameConstants &gc, Grid g)
{			
	double pi = 3.1415926435;
	Vector2d result = Vector2d(0);
	
	vector<Circle*> goals;
	vector<Circle*> obstacles;
	
	for (int i=0; i < g.getHeight(); i++)
		for (int j=0; j < g.getWidth(); j++)
		{
			double value = g.get(i,j);
			Vector2d position = Vector2d(i,j);
			if (value < 0.75 && value > 0.25)
				goals.push_back(new Circle(position, 1.0, true));
			else if (value > 0.9)
				obstacles.push_back(new Circle(position, 1.0, false));
		}
	
	for (int i=0; i < obstacles.size(); i++){
		result += 0.4*obstacles[i]->potentialField(loc,dir);
	}

	result -= 60*goals[goals.size()-1]->potentialField(loc,dir);	
	
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
