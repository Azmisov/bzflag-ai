#include <cstdio>
#include <cmath>
#include <vector>
#include "Vector2d.h"
#include "Polygon.h"
#include "Tank.h"
#include "Flag.h"
#include "Protocol.h"
#include <unistd.h>

using namespace std;

GameConstants gc;
Polygon base;
vector<Tank*> tanks;
vector<Flag*> flags;
vector<Tank*> enemy_tanks;
vector<Flag*> enemy_flags;
vector<Polygon*> obstacles;

int main(int argc, char** argv){
	//Connect to the server
	Tank::protocol = Protocol("localhost", 50100);
	if (!Tank::protocol.isConnected()){
		cout << "Can't connect to BZRC server." << endl;
		exit(1);
	}
	
	//Initialize the board
	Tank::protocol.initialBoard(gc, base, tanks, flags, enemy_tanks, enemy_flags, obstacles);
	
	while(true)
	{
		Tank::protocol.updateBoard(tanks, flags, enemy_tanks, enemy_flags);
		for (int i=0; i < tanks.size(); i++)
		{
			tanks[i].evalPfield(gc, base, tanks, flags, enemy_tanks, enemy_flags, obstacles);
		}
		usleep(30);
	}
	
	return 0;
}

/*
void graphFields(){
	Polygon p1 = Polygon();
	p1.addPoint(0, 0);
	p1.addPoint(.2,.5);
	p1.addPoint(.75, .3);
	p1.addPoint(1,-1);
	p1.addPoint(-1,-.5);
	Polygon p2 = Polygon();
	p2.addPoint(1, 1);
	p2.addPoint(1, 2);
	p2.addPoint(2, 2);
	p2.addPoint(2, 1);
	obstacles.push_back(&p1);
	obstacles.push_back(&p2);

	//Generate gnu-plot data for the field
	Vector2d min = Vector2d(-3), max = Vector2d(3);
	double spacing = .25;
	FILE *f = fopen("fields.gpi", "w+");
	//Initialize graph
	fprintf(f, "set xrange [%f: %f]\n", min[0], max[0]);
	fprintf(f, "set yrange [%f: %f]\n", min[1], max[1]);
	fprintf(f, "unset key\nset size square\n");
	//Draw obstacles
	fprintf(f, "unset arrow\n");
	for (int p=0; p<obstacles.size(); p++){
		Polygon *poly = obstacles[p];
		int len = poly->size();
		Vector2d v2 = (*poly)[len-1], v1;
		for (int i=0; i<len; i++){
			v1 = v2;
			v2 = (*poly)[i];
			fprintf(f, "set arrow from %f, %f to %f, %f nohead lt 3\n", v1[0], v1[1], v2[0], v2[1]);
		}
	}
	//Draw fields
	fprintf(f, "plot '-' with vectors head\n");
	Vector2d dummy_dir = Vector2d(-1,.5);
	for (double i=min[0]; i<max[0]; i+=spacing){
		for (double j=min[1]; j<max[1]; j+=spacing){
			Vector2d force = Vector2d(0);
			for (int p=0; p<obstacles.size(); p++)
				force += obstacles[p]->potentialField(Vector2d(i, j), dummy_dir);
			force *= .1;
			fprintf(f, "%f %f %f %f\n", i, j, force[0], force[1]);
		}
	}
	//Close plot file
	fclose(f);
}
//*/
