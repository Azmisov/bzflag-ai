#ifndef GRID_H
#define GRID_H

#define BASE_PRIOR 0.5
#define STABLE_PROB 0.97

#include "Protocol.h"

struct GameConstants;
typedef unsigned int uint;

class Grid{
private:
	uint size, chunk_width, chunk_size;
	//True positive and negative
	GameConstants &gc;
	//Chunks hold confidence that a particular square has correct probability
	float* chunks;			//summed probabilities
	bool* chunks_stable;	//is chunk stable?
	bool* chunks_dirty;		//stableness of chunk needs updating
	//Global stability counts
	uint num_chunks, num_chunks_stable;
	//Number of cells and cells per chunk
	uint ccells, ccells_middle, ccells_edge, ccells_corner;

public:
	//Temporary buffer for blurred occgrid values
	int* buffer;
	//Posterior probabilities that cell is occupied
	float* grid;
	
	Grid(GameConstants &gc);
	virtual ~Grid();

	//Checks if grid is stable in a particular region
	bool isStable(uint x, uint y);
	bool isStable();
	
	//Converts raster grid into vector polygons
	bool canVectorize();
	std::vector<Polygon*> vectorize();
	
	inline void updateCell(uint x, uint y, bool observation);
};

#endif
