#include "Protocol.h"
#include "Tanks/AbstractTank.h"
#include "math.h"

AbstractTank::AbstractTank(int i, Board *b){
	idx = i;
	board = b;
	
	F << 1, deltaT, deltaT * deltaT/2, 0, 0, 0,
		0, 1, deltaT, 0, 0, 0,
		0, 0, 1, 0, 0, 0,
		0, 0, 0, 1, deltaT, deltaT * deltaT/2,
		0, 0, 0, 0, 1, deltaT,
		0, 0, 0, 0, 0, 1;
	
	sigmaX << 	0.1, 0.0, 0.0, 0.0, 0.0, 0.0,
				0.0, 0.1, 0.0, 0.0, 0.0, 0.0,
				0.0, 0.0, 5, 0.0, 0.0, 0.0,
				0.0, 0.0, 0.0, 0.1, 0.0, 0.0,
				0.0, 0.0, 0.0, 0.0, 0.1, 0.0,
				0.0, 0.0, 0.0, 0.0, 0.0, 5;

	sigmaT << 	5, 0.0, 0.0, 0.0, 0.0, 0.0,
				0.0, 0.1, 0.0, 0.0, 0.0, 0.0,
				0.0, 0.0, 0.1, 0.0, 0.0, 0.0,
				0.0, 0.0, 0.0, 5, 0.0, 0.0,
				0.0, 0.0, 0.0, 0.0, 0.1, 0.0,
				0.0, 0.0, 0.0, 0.0, 0.0, 0.1;
				
	sigma0 << 	5, 0.0, 0.0, 0.0, 0.0, 0.0,
				0.0, 0.1, 0.0, 0.0, 0.0, 0.0,
				0.0, 0.0, 0.1, 0.0, 0.0, 0.0,
				0.0, 0.0, 0.0, 5, 0.0, 0.0,
				0.0, 0.0, 0.0, 0.0, 0.1, 0.0,
				0.0, 0.0, 0.0, 0.0, 0.0, 0.1;
				
	I << 		1, 0.0, 0.0, 0.0, 0.0, 0.0,
				0.0, 1, 0.0, 0.0, 0.0, 0.0,
				0.0, 0.0, 1, 0.0, 0.0, 0.0,
				0.0, 0.0, 0.0, 1, 0.0, 0.0,
				0.0, 0.0, 0.0, 0.0, 1, 0.0,
				0.0, 0.0, 0.0, 0.0, 0.0, 1;
				
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
	return -angDiff/M_PI;
}
void AbstractTank::updateDynamics(double delta_t, float x, float y, float theta){
	//Set direction
	dir = Vector2d(cos(theta), sin(theta));
	//Starting position
	if (delta_t == 0){
		muT[0] = x;
		muT[3] = y;
	}
	//Do kalman stuff
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
	
	if (itsSinceReset > 25)
	{
		sigmaT = sigma0 * I;
		itsSinceReset = -1;
	}
	itsSinceReset++;
}
