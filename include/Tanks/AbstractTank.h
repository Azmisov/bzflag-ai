#ifndef ABSTRACT_TANK
#define ABSTRACT_TANK

#include "Eigen.h"
#include "Vector2d.h"
struct Board;

//These are the only states we're allowed to use for tanks
enum TankMode {
	SPY_FETCH,		//assigned to fetch flag
	SPY_RETRIEVE,	//returning a flag to base
	OFFENSE,		//defending the spy
	DEFENSE,		//defending the flag
	IDLE,			//tank that just came alive and has not been assigned a mode
	DEAD			//tank is dead
};

class AbstractTank{
protected:
	int idx;
	float deltaT = 0.1;
	int itsSinceReset = 0;
	
	Eigen::Matrix<float, 6, 6> F;
	Eigen::Matrix<float, 6, 6> I;
	Eigen::Matrix<float, 6, 6> sigmaX;
	Eigen::Matrix<float, 2, 2> sigmaZ;
	Eigen::Matrix<float, 2, 6> H;
	Eigen::Matrix<float, 6, 1> muT;
	Eigen::Matrix<float, 6, 6> sigmaT;
	Eigen::Matrix<float, 6, 6> sigma0;

public:
	Board *board;
	//Stategy variables
	TankMode mode;
	//Dynamics
	Vector2d pos, vel, acc, dir;
	
	AbstractTank(int i, Board *b);
	virtual ~AbstractTank();
	
	//Computes angular velocity to best obtain desired angle
	double getAngVel(Vector2d angle);
	double getAngVel(double theta);
	//Coordinates actions between tanks (called once for each timestep)
	virtual void coordinate(double delta_t){};
	//Decides which commands to give to the tank
	virtual void move(double delta_t){};
	//Update tank dynamics using kalman filter
	virtual void updateDynamics(double delta_t, float x, float y, float theta);
};

#endif
