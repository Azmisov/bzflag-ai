#include "Protocol.h"
#include "Tanks/AbstractTank.h"
#include "math.h"

AbstractTank::AbstractTank(int i, Board *b){
	idx = i;
	board = b;
	const double
		P = .001,
		V = .01,
		A = .5;
	sigmaX << 	P, 0.0, 0.0, 0.0, 0.0, 0.0,
				0.0, V, 0.0, 0.0, 0.0, 0.0,
				0.0, 0.0, A, 0.0, 0.0, 0.0,
				0.0, 0.0, 0.0, P, 0.0, 0.0,
				0.0, 0.0, 0.0, 0.0, V, 0.0,
				0.0, 0.0, 0.0, 0.0, 0.0, A;

	I << 		1, 0.0, 0.0, 0.0, 0.0, 0.0,
				0.0, 1, 0.0, 0.0, 0.0, 0.0,
				0.0, 0.0, 1, 0.0, 0.0, 0.0,
				0.0, 0.0, 0.0, 1, 0.0, 0.0,
				0.0, 0.0, 0.0, 0.0, 1, 0.0,
				0.0, 0.0, 0.0, 0.0, 0.0, 1;

	sigmaT << 	15, 0.0, 0.0, 0.0, 0.0, 0.0,
				0.0, 10, 0.0, 0.0, 0.0, 0.0,
				0.0, 0.0, .05, 0.0, 0.0, 0.0,
				0.0, 0.0, 0.0, 15, 0.0, 0.0,
				0.0, 0.0, 0.0, 0.0, 10, 0.0,
				0.0, 0.0, 0.0, 0.0, 0.0, .05;
				
	sigma0 << 	sigmaT;
				
	muT << 0, 0, 0, 0, 0, 0;
				
	sigmaZ << 25, 0, 0, 25;
	
	H << 1, 0.0, 0.0, 0.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 1, 0.0, 0.0;
}
AbstractTank::~AbstractTank(){}

double AbstractTank::getAngVel(Vector2d angle){
	return getAngVel(atan2(angle[1], angle[0]));
}
double AbstractTank::getAngVel(double theta){
	double currentAngle = atan2(dir[1], dir[0]);	
	double angDiff = currentAngle - theta;
	while (angDiff < -M_PI)
		angDiff += 2*M_PI;
	while (angDiff > M_PI)
		angDiff -= 2*M_PI;
	angDiff *= 4*M_1_PI;
	if (angDiff > 1) angDiff = 1;
	int sign = angDiff > 0 ? -1 : 1;
	const double squash = 3.3;
	return sign/(1+exp(-squash*2*fabs(angDiff)+squash)) - .03;
}
void AbstractTank::updateDynamics(double delta_t, float x, float y, float theta){
	//Set direction
	dir = Vector2d(cos(theta), sin(theta));
	//Starting position
	if (delta_t == 0){
		muT[0] = x;
		muT[3] = y;
	}
	Vector2d est = pos+vel*delta_t + delta_t*delta_t/2*acc;
	raw_pos.setData(x, y);
	//Do kalman stuff
	F << 1, delta_t, delta_t * delta_t/2, 0, 0, 0,
		0, 1, deltaT, 0, 0, 0,
		0, 0, 1, 0, 0, 0,
		0, 0, 0, 1, delta_t, delta_t * delta_t/2,
		0, 0, 0, 0, 1, delta_t,
		0, 0, 0, 0, 0, 1;
	Eigen::Vector2f zT(x, y);
	Eigen::Matrix<float, 6, 6> L;
	Eigen::Matrix<float, 6, 2> K;
	L = (F * sigmaT * F.transpose()) + sigmaX;
	K = L * H.transpose() * (((H * L * H.transpose()) + sigmaZ).inverse());
	
	Eigen::Matrix<float, 6, 1> mu;
	Eigen::Matrix<float, 6, 6> sigma;
	mu = (F*muT) + (K * (zT-(H * F * muT)));
	sigma = (I - (K * H)) * (F * sigmaT * F.transpose() + sigmaX);
	
	muT = mu;
	sigmaT = sigma;
	//Assign member variables
	for (int i=0; i<2; i++){
		pos[i] = muT[i*3];
		vel[i] = muT[i*3+1];
		acc[i] = muT[i*3+2];
	}
	//*
	if (itsSinceReset > 300){
		sigmaT = sigma0 * I;
		itsSinceReset = -1;
	}
	//*/
	//std::cout << sigmaT << std::endl;
	itsSinceReset++;
	/*
	if (itsSinceReset % 5 == 0 && pos[1] > -280)
		printf("(%d,%d)  (%d,%d)  (%d,%d)  (%f,%f)\n",
			(int)round(est[0]), (int)round(est[1]),
			(int)round(x), (int)round(y),
			(int)round(pos[0]), (int)round(pos[1]),
			sigmaT(0,0), sigmaT(3,3));
	*/
}
