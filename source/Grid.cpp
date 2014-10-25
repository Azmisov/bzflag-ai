#include "Grid.h"

Grid::Grid(unsigned int width, unsigned int height){
	w = width;
	h = height;
	grid = (float*) malloc(sizeof(float)*width*height);
}
Grid::~Grid(){
	free(grid);
}

unsigned int Grid::getWidth(){ return w; }
unsigned int Grid::getHeight(){ return h; }

float* Grid::operator[](int idx){
	return &(grid[idx*w]);
}

void Grid::updateCell(unsigned int x, unsigned int y, bool observation){
	float old = grid[x*w + y];
	//TODO: update based on bayes rule
}
