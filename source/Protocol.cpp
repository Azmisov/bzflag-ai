#include "Protocol.h"

Protocol(const char *host, int port);
virtual ~Protocol();

/// Establish connection to server; NOTE: I did not write any of this code
int Protocol::init(){
	resetReplyBuffer();
	start = 0;

#ifdef WINDOWS

	//start up WinSock
	LPWSADATA lpmyWSAData;
	WORD VersionReqd;
	VersionReqd = MAKEWORD(2,0);
	lpmyWSAData = (LPWSADATA) malloc(sizeof(WSADATA));
	if ((int err = WSAstartup(VersionReqd, lpmyWSAData)) != 0){
		cerr << "WSAstartup() returned error code " << err << "." << endl;
		return 1;
	}
	if (LOBYTE(lpmyWSAData->wVersion) != 2 || HIBYTE(lpmyWSAData->wVersion) != 0) {
		WSACleanup();
		free(lpmyWSAData);
		return 0;
	}
	free(lpmyWSAData);
	
	//Find server's address
	u_long nRemoteAddr = inet_addr(pcHost);
	if (nRemoteAddr == INADDR_NONE) {
		// pcHost isn't a dotted IP, so resolve it through DNS
		hostent* pHE = gethostbyname(pcHost);
		if (!pHE){
			cerr << "Host cannot be found." << endl;
			return 1;
		}
		else nRemoteAddr = *((u_long*)pHE->h_addr_list[0]);
	}
	
	//Establish the connection
	// Create a stream socket
	sd = socket(AF_INET, SOCK_STREAM, 0);
	if (sd != INVALID_SOCKET) {
		sockaddr_in sinRemote;
		sinRemote.sin_family = AF_INET;
		sinRemote.sin_addr.s_addr = nRemoteAddr;
		sinRemote.sin_port = htons(nPort);
		//Invalid socket
		if (connect(sd, (sockaddr*)&sinRemote, sizeof(sockaddr_in)) == SOCKET_ERROR) {
			cerr << WSAGetLastError() << endl;
			return 1;
		}
	}

#else

	struct addrinfo *infop = NULL;
	struct addrinfo hint;
	memset(&hint, 0, sizeof(hint));
	hint.ai_family = AF_INET;
	hint.ai_socktype = SOCK_STREAM;
	char port[10];
	snprintf(port, 10, "%d", nPort);
	if (getaddrinfo(pcHost, port, &hint, &infop) != 0) {
		perror("Couldn't lookup host.");
		return 1;
	}
	if ((sd = socket(infop->ai_family, infop->ai_socktype, infop->ai_protocol)) < 0){
		perror("Couldn't create socket.");
		return 1;
	}
	if (connect(sd, infop->ai_addr, infop->ai_addrlen) < 0) {
		perror("Couldn't connect.");
		//close(sd);
	}
	freeaddrinfo(infop);
	
#endif

	if (handshake()){
		cerr << "Handshake failed!" << endl;
		return 1;
	}
	return 0;
}

/// Communicate with the server; NOTE: I did not write any of this code
int Protocol::sendLine(const char *line) {
	int length = (int) strlen(LineText);
	char command[kBufferSize];
	strcpy(command, LineText);
	command[length] = '\n';
	command[length+1] = '\0';
	if (DEBUG) cout << command;
	return send(sd, command, length+1, 0) < 0;
}
int Protocol::readReply(char *reply){
	char acreadBuffer[kBufferSize];
	int nNewBytes = recv(sd, acreadBuffer, kBufferSize, 0);
	if (nNewBytes < 0) return -1;
	else if (nNewBytes == 0) {
		cerr << "Connection closed by peer." << endl;
		return 0;
	}
	memcpy(reply, &acreadBuffer, nNewBytes);
	if (nNewBytes!=kBufferSize)
		reply[nNewBytes] = '\0';
	return nNewBytes;
}
void Protocol::readLine(char *LineText) {
	memset(LineText, '\0', kBufferSize);
	// Only read more from server when done wiht current ReplyBuffer
	if (strlen(ReplyBuffer)==0) {
		char *reply;
		reply = ReplyBuffer;
		readReply(reply);
	}
	int i=0;
	bool done=false;
	while (!done){
		for(i=LineFeedPos+1; (i<kBufferSize && ReplyBuffer[i]); i++) {
			if (ReplyBuffer[i] == '\n') {
				LineText[i-LineFeedPos-1+start] = '\0';
				LineFeedPos = i;
				start = 0;
				done = true;
				break;
			}
			LineText[i-LineFeedPos-1+start]=ReplyBuffer[i];
		}
		if (!done){
			start = (int)strlen(LineText);
			resetReplyBuffer();	
			char *Reply;
			Reply = ReplyBuffer;
			readReply(Reply);
		}
		else if(ReplyBuffer[i]=='\0') {
			done = true;
			start = 0;
			resetReplyBuffer();
		}
	}
}
void Protocol::resetReplyBuffer() {
	memset(ReplyBuffer, '\0', kBufferSize);
	LineFeedPos = -1;
}
int Protocol::handshake() {
	char str[kBufferSize];
	char *LineText;
	LineText = str;
	readLine(LineText);
	if (DEBUG) cout << LineText << endl;
	if (!strcmp(LineText, "bzrobots 1")){
		const char * Command="agent 1";
		int temp = sendLine(Command);
		if(temp == 1)  return 1;
		else resetReplyBuffer();
		return 0;
	}
	return 1;
}
vector<string> Protocol::readArr() {
	char str[kBufferSize];
	char *LineText=str;
	readLine(LineText);
	if (strlen(LineText) && DEBUG)
		cout << LineText << endl;
	while (!strlen(LineText)) {
		readLine(LineText);
		if (DEBUG) cout << LineText << endl;
	}
    string buf;
    stringstream ss(LineText);
    while (ss >> buf)
        list_buffer.push_back(buf);
	return list_buffer;
}
void Protocol::readAck() {
	vector<string> v = readArr();
	if (v.at(0) != "ack") {
		cout << "Didn't receive ack! Exit!" << endl;
		exit(1);
	}
}
bool Protocol::readBool() {
	vector <string> v = readArr();
	if (v.at(0)=="ok")
		return true;
	else if (v.at(0) == "fail"){
		if (DEBUG) cout << "Received fail. Exiting!" << endl;
		return false;
	}
	else {
		if (DEBUG) cout << "Something went wrong. Exiting!" << endl;
		return false;
	}
}
void Protocol::printLine() {
	char str[kBufferSize];
	char *LineText=str;
	readLine(LineText);
	if (DEBUG) cout << LineText << endl;
}

// Self check
int Protocol::getPort(){
	
}
const char* Protocol::getHost(){
	
}
bool Protocol::getStatus(){
	
}
	
//Tank commands
bool Protocol::shoot(int index){
	
}
bool Protocol::speed(int index, double value){
	
}
bool Protocol::angvel(int index, double value){
	
}
bool Protocol::accelx(int index, double value){
	
}
bool Protocol::accely(int index, double value){
	
}

//Fetch information
bool Protocol::getBases(vector <base_t> *AllBases){
	
}
bool Protocol::getTeams(vector <team_t> *AllTeams){
	
}
bool Protocol::getObstacles(vector <obstacle_t> *AllObstacles){
	
}
bool Protocol::getFlags(vector <flag_t> *AllFlags){
	
}
bool Protocol::getShots(vector <shot_t> *AllShots){
	
}
bool Protocol::getTanks(vector <tank_t> *AllMyTanks){
	
}
bool Protocol::getOpponents(vector <otank_t> *AllOtherTanks){
	
}
bool Protocol::getConstants(vector <constant_t> *AllConstants){
	
}
