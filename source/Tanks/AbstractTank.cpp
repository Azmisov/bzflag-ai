#include "Protocol.h"
#include "Tanks/AbstractTank.h"
#include "math.h"
#include "Eigen.h"

AbstractTank::AbstractTank(int i, Board *b){
	idx = i;
	board = b;
	
	F << 1, deltaT, deltaT * deltaT/2, 0, 0, 0,
		0, 1, deltaT, 0, 0, 0,
		0, 0, 1, 0, 0, 0,
		0, 0, 0, 1, deltaT, deltaT * deltaT/2,
		0, 0, 0, 0, 1, deltaT,
		0, 0, 0, 0, 0, 1;
	
	sigmaX << 0.1, 0.0, 0.0, 0.0, 0.0, 0.0,
				0.0, 0.1, 0.0, 0.0, 0.0, 0.0,
				0.0, 0.0, 100, 0.0, 0.0, 0.0,
				0.0, 0.0, 0.0, 0.1, 0.0, 0.0,
				0.0, 0.0, 0.0, 0.0, 0.1, 0.0,
				0.0, 0.0, 0.0, 0.0, 0.0, 100;
				
	sigmaT << 100, 0.0, 0.0, 0.0, 0.0, 0.0,
				0.0, 0.1, 0.0, 0.0, 0.0, 0.0,
				0.0, 0.0, 0.1, 0.0, 0.0, 0.0,
				0.0, 0.0, 0.0, 100, 0.0, 0.0,
				0.0, 0.0, 0.0, 0.0, 0.1, 0.0,
				0.0, 0.0, 0.0, 0.0, 0.0, 0.1;
				
	I << 1, 0.0, 0.0, 0.0, 0.0, 0.0,
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
		pos[0] = x;
		pos[1] = y;
		vel = Vector2d(0);
		acc = Vector2d(0);
	}

void Kalman(Eigen::Matrix<float, 2, 1> zT)
{
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
}
