#ifndef GRID_H
#define GRID_H

#include <stdlib.h>

class Grid{
private:
	unsigned int w, h;

public:
	float* grid;
	
	Grid(unsigned int width, unsigned int height);
	virtual ~Grid();

	unsigned int getWidth();
	unsigned int getHeight();
	float get(unsigned int x, unsigned int y);
	//Get x dimension, for array indexing
	float* operator[](int idx);
	
	void updateCell(unsigned int x, unsigned int y, bool observation);
};

#endif
