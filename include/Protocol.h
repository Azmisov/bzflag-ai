#ifndef PROTOCOL_H
#define	PROTOCOL_H

#define DEBUG 0
#define OCC_BLUR 2

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <map>
#include <math.h>

#include "Vector2d.h"
#include "Tanks/AbstractTank.h"
#include "Polygon.h"
#include "Flag.h"
#include "Grid.h"

class Protocol;
const int kBufferSize = 1024;

using namespace std;

//There are a couple other constants available, but I don't think we'll need them
typedef struct GameConstants {
	//Defined as command line options
	bool friendlyfire;
	bool grabownflag;
	bool usegrid;
	int gridwidth;
	//Defined by server
	string mycolor;
	string tankalive;
	string tankdead;
	double worldsize;
	double tankradius;
	double tankangvel;
	double tankspeed;
	double linearaccel;
	double angularaccel;
	double shotradius;
	double shotrange;
	double shotspeed;
	double flagradius;
	double truenegative;
	double truepositive;
} GameConstants;

typedef struct Board{
	Protocol *p;
	GameConstants gc;
	Polygon base;
	Grid *grid;
	vector<AbstractTank*> tanks;
	vector<Flag*> flags;
	vector<AbstractTank*> enemy_tanks;
	vector<Flag*> enemy_flags;
	vector<Polygon*> obstacles;
} Board;

class Protocol {
private:
	//Server name, port, and socket
	const char *hostname;
	int port;
	int sd;
	//Has the connection been established?
	bool initialized;
	//Message buffer data
	char ReplyBuffer[kBufferSize];
	vector<string> list_buffer;
	int LineFeedPos;
	int start;

	//Initialize connection with server
	bool init();
	//Server communications
	bool sendLine(const char *LineText);	// Send line to server
	bool readReply(char *Reply);			// read line back from server
	void readLine(char *LineText);			// Only read one line of text from ReplyBuffer
	void resetReplyBuffer();				// Reset the ReplyBuffer
	bool handshake();						// Perform handshake with the server
	vector <string> readArr();				// read line into vector
	void readAck();							// read Acknowledgement
	bool readBool();						// read "ok"
	void printLine();						// Receive and print another line

public:
	Protocol();
	Protocol(const char* host, int port);
	virtual ~Protocol();
	bool isConnected();
	bool close();
    
    //General commands
    bool taunt(char* message);
    
	//Tank commands
	bool shoot(int idx);
	bool speed(int idx, double val);
	bool angvel(int idx, double val);
	bool accelx(int idx, double val);
	bool accely(int idx, double val);

	//Fetch information
	template <class TANK>
	bool initialBoard(Board &board);
	bool updateBoard(double delta_t, Board &board);
	bool updateGrid(Board &board);
};

template <class TANK>
bool Protocol::initialBoard(Board &board){
	board.p = this;

	//Request a dictionary of game constants
	sendLine("constants");
	readAck();
	vector<string> v = readArr();
	if (v.at(0) != "begin")
		return false;
	v.clear();
	v = readArr();
	while (v.at(0) == "constant"){
		string &name = v.at(1);
		string val = v.at(2);
		double dval = atof(val.c_str());
		//Yeah, this is inefficient; but we only run it once
		if (name == "team") board.gc.mycolor = val;
		else if (name == "tankalive") board.gc.tankalive = val;
		else if (name == "tankdead") board.gc.tankdead = val;
		else if (name == "worldsize") board.gc.worldsize = dval;
		else if (name == "tankradius") board.gc.tankradius = dval;
		else if (name == "tankangvel") board.gc.tankangvel = dval;
		else if (name == "tankspeed") board.gc.tankspeed = dval;
		else if (name == "linearaccel") board.gc.linearaccel = dval;
		else if (name == "angularaccel") board.gc.angularaccel = dval;
		else if (name == "shotradius") board.gc.shotradius = dval;
		else if (name == "shotrange") board.gc.shotrange = dval;
		else if (name == "shotspeed") board.gc.shotspeed = dval;
		else if (name == "flagradius") board.gc.flagradius = dval;
		else if (name == "truenegative") board.gc.truenegative = dval;
		else if (name == "truepositive") board.gc.truepositive = dval;
		v.clear();
		v = readArr();
	}
	if (v.at(0) != "end")
		return false;
	
	//Get a list of teams to populate tank/enemy vectors
	sendLine("teams");
	readAck();
	v = readArr();
	if (v.at(0) != "begin")
		return false;
	v.clear();
	v = readArr();
	while (v.at(0) == "team"){
		int count = atoi(v.at(2).c_str());
		//This is our team
		if (v.at(1) == board.gc.mycolor){
			for (int idx=0; idx<count; idx++)
				board.tanks.push_back(new TANK(idx, &board));
		}
		//This is the enemy
		else{
			for (int idx=0; idx<count; idx++)
				board.enemy_tanks.push_back(new AbstractTank(-1, &board));
		}
		v.clear();
		v = readArr();
	}
	if (v.at(0) != "end")
		return false;
	
	//Initialize the grid object
	if (board.gc.usegrid)
		board.grid = new Grid(board.gc);
	//Get a list of obstacles
	else{
		sendLine("obstacles");
		readAck();
		v = readArr();
		if (v.at(0) != "begin")
			return false;
		v.clear();
		v = readArr();
		while (v.at(0) == "obstacle"){
			Polygon *p = new Polygon();
			for (int i=1; i<v.size(); i+=2){
				p->addPoint(
					atof(v.at(i).c_str()),
					atof(v.at(i+1).c_str())
				);
			}
			board.obstacles.push_back(p);
			v.clear();
			v = readArr();
		}
		if (v.at(0) != "end")
			return false;
	}
	
	//Get our base
	sendLine("bases");
	readAck();
	v = readArr();
	if (v.at(0) != "begin")
		return false;
	v.clear();
	v = readArr();
	while (v.at(0) == "base"){
		//We only care about our own base; don't store other players' bases
		if (v.at(1) == board.gc.mycolor){
			for (int i=2; i<v.size(); i+=2){
				board.base.addPoint(
					atof(v.at(i).c_str()),
					atof(v.at(i+1).c_str())
				);
			}
		}
		v.clear();
		v = readArr();
	}
	if (v.at(0) != "end")
		return false;
	
	//Get a list of flags
	sendLine("flags");
	readAck();
	v = readArr();
	if (v.at(0) != "begin")
		return false;
	v.clear();
	v = readArr();
	while (v.at(0) == "flag"){
		Flag *f = new Flag();
		f->radius = board.gc.flagradius;
		if (v.at(1) == board.gc.mycolor)
			board.flags.push_back(f);
		else board.enemy_flags.push_back(f);		
		v.clear();
		v = readArr();
	}
	if (v.at(0) != "end")
		return false;
		
	//Hooray, no errors!!
	return true;
}

#endif
