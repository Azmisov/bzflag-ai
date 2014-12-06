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

class Grid;
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

#endif
