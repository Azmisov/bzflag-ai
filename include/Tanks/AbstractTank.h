#ifndef ABSTRACT_TANK
#define ABSTRACT_TANK
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
	int idx;
	float deltaT = 1;
	
	Eigen::Matrix<float, 6, 6> F;
	Eigen::Matrix<float, 6, 6> I;
	Eigen::Matrix<float, 6, 6> sigmaX;
	Eigen::Matrix<float, 2, 2> sigmaZ;
	Eigen::Matrix<float, 2, 6> H;
	Eigen::Matrix<float, 6, 1> muT;
	Eigen::Matrix<float, 6, 6> sigmaT;

public:
	Board *board;
	//Stategy variables
	TankMode mode;
	//Dynamics
	Vector2d pos, vel, acc, dir;
	
	AbstractTank(int i, Board *b);
	~AbstractTank();
	
	//Decides which commands to give to the tank
	void move(double delta_t);
	//Update tank dynamics using kalman filter
	void updateDynamics(double delta_t, float x, float y, float theta);
};

#endif
