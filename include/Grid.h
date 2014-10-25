#ifndef GRID_H
#define GRID_H

#include <stdlib.h>

class Grid{
private:
	unsigned int w, h;

public:
	float* grid;
	
	Grid(int width, int height);
	virtual ~Grid();

	int getWidth();
	int getHeight();
	//Get x dimension, for array indexing
	float& operator[](int idx);
	
	void updateCell(int x, int y, int observation);
}

#endif
