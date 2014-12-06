#include <iostream>
#include <vector>
#include "Eigen.h"
using namespace std;

int main()
{
	float deltaT = 1;
	
	Eigen::Matrix<float, 6, 6> F;
	Eigen::Matrix<float, 6, 1> I;
	Eigen::Matrix<float, 6, 1> sigmaX;
	Eigen::Matrix<float, 2, 2> sigmaZ;
	Eigen::Matrix<float, 2, 6> H;
	Eigen::Matrix<float, 6, 1> muT;
	Eigen::Matrix<float, 6, 6> sigmaT;
	Eigen::Matrix<float, 2, 1> zT;
	
	sigmaX << 0.1, 0.1, 100, 0.1, 0.1, 100;
	I << 1, 1, 1, 1, 1, 1;
	
	F << 1, deltaT, deltaT * deltaT/2, 0, 0, 0,
		0, 1, deltaT, 0, 0, 0,
		0, 0, 1, 0, 0, 0,
		0, 0, 0, 1, deltaT, deltaT * deltaT/2,
		0, 0, 0, 0, 1, deltaT,
		0, 0, 0, 0, 0, 1;
				
	sigmaT << 100, 0.0, 0.0, 0.0, 0.0, 0.0,
				0.0, 0.1, 0.0, 0.0, 0.0, 0.0,
				0.0, 0.0, 0.1, 0.0, 0.0, 0.0,
				0.0, 0.0, 0.0, 100, 0.0, 0.0,
				0.0, 0.0, 0.0, 0.0, 0.1, 0.0,
				0.0, 0.0, 0.0, 0.0, 0.0, 0.1;
				
	muT << 0, 0, 0, 0, 0, 0;
				
	sigmaZ << 25, 0, 0, 25;
	
	H << 1, 0.0, 0.0, 0.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 1, 0.0, 0.0;
		
	zT << 29, 44;
	
	Eigen::Matrix<float, 6, 6> L;
	Eigen::Matrix<float, 6, 2> K;
	L = (F * sigmaT * F.transpose()) + Eigen::Matrix<float, 6, 6>(sigmaX.asDiagonal());
	K = L * H.transpose() * (((H * L * H.transpose()) + sigmaZ).inverse());
	
	Eigen::Matrix<float, 6, 1> mu;
	Eigen::Matrix<float, 6, 6> sigma;
	mu = (F*muT) + (K * (zT-(H * F * muT)));
	sigma = (I.asDiagonal() - (K * H)) * (F * sigmaT * F.transpose() + Eigen::Matrix<float, 6, 6>(sigmaX.asDiagonal()));
	cout << sigma << endl << endl;
	cout << mu << endl;
	
    return 0;
}

