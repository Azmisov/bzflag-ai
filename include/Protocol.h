#ifndef PROTOCOL_H
#define	PROTOCOL_H

#define DEBUG 0

#ifdef WINDOWS
#define _CRT_SECURE_NO_DEPRECATE 1
#define _USE_MATH_DEFINES
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif

#include <stdio.h>
#include <string>
#include <sstream>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include "math.h"

const int MAX_OBSTACLE_CORNERS = 10;
const int kBufferSize = 1024;

using namespace std;

typedef struct team_t {
	string color;
	int count;
} team_t;
typedef struct base_t {
        string color;
	double base_corner[4][2];
} base_t;
typedef struct obstacle_t {
	double o_corner[MAX_OBSTACLE_CORNERS][2];
        int courner_count;
} obstacle_t;
typedef struct flag_t {
	string color;
	string poss_color;
	double pos[2];
} flag_t;
typedef struct shot_t {
	double pos[2];
	double velocity[2];
} shot_t;
typedef struct tank_t {
	int index;
	string callsign;
	string status;
	int shots_avail;
	double time_to_reload;
	string flag;
	double pos[2];
	double angle;
	double velocity[2];
	double angvel;
} tank_t;
typedef struct otank_t {
	string callsign;
	string color;
	string status;
	string flag;
	double pos[2];
	double angle;
} otank_t;
typedef struct constant_t {
	string name;
	string value;
} constant_t;

class Protocol {
private:
	const char *pcHost;
	int nPort;
	bool debug;
	bool InitStatus;
	char ReplyBuffer[kBufferSize];
	vector<string> list_buffer;
	int LineFeedPos;
	int start;

#ifdef WINDOWS
	SOCKET sd;
#else
	int sd;
#endif

	// Initializing connection.
	int init();
	
	int sendLine(const char *LineText);	// Send line to server
	int readReply(char *Reply)			// read line back from server
	void readLine(char *LineText);		// Only read one line of text from ReplyBuffer
	void resetReplyBuffer();			// Reset the ReplyBuffer
	int handshake();					// Perform handshake with the server
	vector <string> readArr();			// read line into vector
	void readAck();						// read Acknowledgement
	bool readBool();					// read "ok"
	void printLine();					// Receive and print another line

public:
	Protocol(const char *host, int port);
	virtual ~Protocol();

	// Self check
	int getPort();
	const char *getHost();
	bool getStatus();
        
	//Tank commands
	bool shoot(int index);
	bool speed(int index, double value);
	bool angvel(int index, double value);
	bool accelx(int index, double value);
	bool accely(int index, double value);

	//Fetch information
	bool getBases(vector <base_t> *AllBases);
	bool getTeams(vector <team_t> *AllTeams);
	bool getObstacles(vector <obstacle_t> *AllObstacles);
	bool getFlags(vector <flag_t> *AllFlags);
	bool getShots(vector <shot_t> *AllShots);
	bool getTanks(vector <tank_t> *AllMyTanks);
	bool getOpponents(vector <otank_t> *AllOtherTanks);
	bool getConstants(vector <constant_t> *AllConstants);
};

#endif
