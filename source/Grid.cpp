#include "Grid.h"
#include "Protocol.h"
#include "Polygon.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <algorithm>

Grid::Grid(GameConstants &gc) : gc(gc){
	size = gc.worldsize;
	chunk_width = gc.gridwidth*0.9;
	chunk_size = size/chunk_width;
	if (!chunk_size){
		chunk_size++;
		chunk_width = size;
	}
	ccells = size*size;
	num_chunks = chunk_size*chunk_size;
	int gsize = sizeof(float)*ccells,
		csize = sizeof(float)*num_chunks,
		bsize = sizeof(int)*gc.gridwidth*gc.gridwidth;
	grid = (float*) malloc(gsize);
	chunks = (float*) malloc(csize);
	chunks_stable = (bool*) malloc(csize);
	chunks_dirty = (bool*) malloc(csize);
	buffer = (int*) malloc(bsize);
	memset(grid, gsize, BASE_PRIOR);
	memset(chunks_stable, csize, false);
	memset(chunks_dirty, csize, false);
	//Chunking stuff
	num_chunks_stable = 0;
	ccells_middle = chunk_width*chunk_width;
	int border = size % chunk_width;
	ccells_edge = ccells + chunk_width*border;
	ccells_corner = ccells + border*border;
	//Default chunk values
	int idx = 0;
	float v = ccells_middle*BASE_PRIOR,
		v_edge = ccells_edge*BASE_PRIOR;
	for (int i=0; i<chunk_size-1; i++){
		for (int j=0; j<chunk_size-1; j++)
			chunks[idx++] = v;
		// right row
		chunks[idx++] = v_edge;
	}
	// bottom row
	for (int i=0; i<chunk_size-1; i++)
		chunks[idx++] = v_edge;
	// corner
	chunks[idx] = ccells_corner*BASE_PRIOR;
}
Grid::~Grid(){
	free(grid);
	free(chunks);
	free(buffer);
}

//Check grid for stability
bool Grid::isStable(uint x, uint y){
	int i = x/chunk_width,
		j = y/chunk_width,
		idx = i*chunk_size + j;
	//Use cached stability
	if (chunks_stable[idx])
		return true;
	int tot = ccells_middle;
	if (i == chunk_size-1 && j == chunk_size-1)
		tot = ccells_corner;
	else if (i == chunk_size-1 || j == chunk_size-1)
		tot = ccells_edge;
	bool stable = abs(2*(chunks[idx]/(float)tot)-1) >= STABLE_PROB;
	//Cache stability
	if (stable){
		chunks_stable[idx] = true;
		num_chunks_stable++;
		//TODO: Vectorize this chunk
		/*
		iters = chunk_width/10;
		for (int m=0; m<iters; m++){
			for (int n=0; n<iters; n++){
			
			}
		}
		*/
	}
	return stable;
}
bool Grid::isStable(){
	return num_chunks == num_chunks_stable;
}

std::vector<Polygon*> Grid::vectorize(){
	vector<Polygon*> polys;
	for (int i=0; i<num_chunks; i++){
		//We already have a vectorization of this
		if (chunks_stable[i]){
			
		}
		//Approximate obstacles by randomly creating circles
		
	}
}

//Updates probability that a cell is occupied, given the new observation
void Grid::updateCell(uint x, uint y, bool observation){
	float prior = grid[x*size + y];
	float prob1 = 0;
	float prob2 = 0;
	
	if (observation){
		prob1 = gc.truepositive;
		prob2 = (gc.truepositive * prior) + ((1-gc.truepositive) * (1-prior));
	}
	else{
		prob1 = 1 - gc.truenegative;
		prob2 = ((1-gc.truepositive) * prior) + (gc.truepositive * (1-prior));
	}
	
	float posterior = prior * prob1 / prob2;
	grid[x*size + y] = posterior;
	
	//Update chunk, to check stability
	int idx = (x/chunk_width)*chunk_size + y/chunk_width;
	chunks[idx] += posterior-prior;
	chunks_dirty[idx] = true;
}
