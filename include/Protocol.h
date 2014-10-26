#ifndef PROTOCOL_H
#define	PROTOCOL_H

#define DEBUG 0
#define BLUR_GRID 1

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
#include <math.h>

#include "Vector2d.h"
#include "Polygon.h"
#include "ExplorerTank.h"
#include "Flag.h"
#include "Grid.h"

class ExplorerTank;

const int kBufferSize = 1024;

using namespace std;

//There are a couple other constants available, but I don't think we'll need them
typedef struct GameConstants {
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
        
	//Tank commands
	bool shoot(int idx);
	bool speed(int idx, double val);
	bool angvel(int idx, double val);
	bool accelx(int idx, double val);
	bool accely(int idx, double val);

	//Fetch information
	bool initialBoard(
		GameConstants &gc,
		Polygon &base,
		vector<ExplorerTank*> &tanks,
		vector<Flag*> &flags,
		vector<ExplorerTank*> &enemy_tanks,
		vector<Flag*> &enemy_flags,
		vector<Polygon*> &obstacles,
		bool use_obstacles
	);
	bool updateBoard(
		GameConstants &gc,
		vector<ExplorerTank*> &tanks,
		vector<Flag*> &flags,
		vector<ExplorerTank*> &enemy_tanks,
		vector<Flag*> &enemy_flags
		//vector<Shot*> &shots
	);
	bool updateGrid(
		GameConstants &gc,
		Grid &g,
		int tank_idx,
		char* origin,
		float* blurred
	);
};

#endif
