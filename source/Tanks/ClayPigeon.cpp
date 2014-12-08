#include "Tanks/ClayPigeon.h"
#include "Protocol.h"
#include <cstdlib>
#include <math.h>

void ClayPigeon::coordinate(double delta_t){
	if (delta_t) return;
	for (int i=0; i<board->tanks.size(); i++){
		ClayPigeon* t = dynamic_cast<ClayPigeon*>(board->tanks[i]);
		t->stationary = false; //rand() % 2
		if (!t->stationary){
			//Random direction
			//double ra = (rand()/(double)RAND_MAX)*2 - 1;
			//t->target_dir = ra*M_PI;
			//Random speed
			double rs = rand()/(double) RAND_MAX;
			t->board->p->speed(i, .2); //rs
			t->board->p->angvel(i, .3);
		}
	}
}
void ClayPigeon::move(double delta_t){
	//if (stationary) return;
	//double v = getAngVel(target_dir);
	//if (v < .01) return;
	//board->p->angvel(idx, v);
}
