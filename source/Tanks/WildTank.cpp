#include "Tanks/WildTank.h"
#include "Protocol.h"
#include <cstdlib>
#include <math.h>

#define PROB .3

void WildTank::move(double delta_t){
	float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	if (r <= PROB) {
		float v = static_cast <float> (rand()) / static_cast <float> (RAND_MAX)* 2 - 1;
		board->p->speed(idx,(double) v);
	}
	r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	if (r <= PROB) {
		float ang_v = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * 2 - 1;
		board->p->angvel(idx,(double) ang_v);
	}
}
