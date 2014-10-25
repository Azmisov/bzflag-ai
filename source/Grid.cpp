#include "Grid.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <algorithm>

float Grid::truepositive = 1;
float Grid::truenegative = 1;

Grid::Grid(unsigned int width, unsigned int height){
	w = width;
	h = height;
	int gsize = width*height;
	grid = (float*) malloc(sizeof(float)*gsize);
	for (int i=0; i<gsize; i++)
		grid[i] = .5;
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
	
	if (observation){
		prob1 = Grid::truepositive;
		prob2 = (Grid::truepositive * prior) + ((1-Grid::truepositive) * (1-prior));
	}
	else{
		prob1 = 1 - Grid::truenegative;
		prob2 = ((1-Grid::truepositive) * prior) + (Grid::truepositive * (1-prior));
	}
	
	grid[x*w + y] = prior * prob1 / prob2;
}
