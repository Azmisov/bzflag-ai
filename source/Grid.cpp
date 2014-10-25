#include "Grid.h"

Grid::Grid(unsigned int width, unsigned int height){
	w = width;
	h = height;
	grid = (float*) malloc(sizeof(float)*width*height);
}
virtual Grid::~Grid(){
	free(grid);
}

int getWidth(){ return w; }
int getHeight(){ return h; }

float* Grid::operator[](int idx){
	return &(grid[idx*w]);
}

void updateCell(int x, int y, int observation){
	float old = grid[x*width + y];
	//TODO: update based on bayes rule
}
