#include WildTank.h

void WildTank::move(double delta_t){
	float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * 2 - 1;
	if (r <= .2) {
		float v = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		board->p->speed(idx,(double) v);
	}
	float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * 2 - 1;
	if (r <= .2) {
		float ang_v = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		board->p->angvel(idx,(double) ang_v);
	}
	
}
