#include "Grid.h"

static float truePositive = .97;
static float trueNegative = .9;

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
	float prior = grid[x*w + y];
	float prob1 = 0;
	float prob2 = 0;
	
	if (observation)
	{
		prob1 = truePositive;
		prob2 = (truePositive * prior) + ((1-truePositive) * (1-prior));
	}
	else
	{
		prob1 = 1 - trueNegative;
		prob2 = ((1-truePositive) * prior) + (truePositive * (1-prior));
	}
	
	grid[x*w + y] = prior * prob1 / prob2;
}
