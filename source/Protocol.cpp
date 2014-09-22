#include "Protocol.h"

Protocol::Protocol(const char* hostname, int port){
	this->hostname = hostname;
	this->port = port;
	if (init()) {
		cout << "BZRC initialization failed; could not esetablish server connection" << endl;
		close();
	}
}
Protocol::~Protocol(){
	close();
}

/// Establish connection to server; NOTE: I did not write any of this code
bool Protocol::init(){
	resetReplyBuffer();
	start = 0;

	struct addrinfo *infop = NULL;
	struct addrinfo hint;
	memset(&hint, 0, sizeof(hint));
	hint.ai_family = AF_INET;
	hint.ai_socktype = SOCK_STREAM;
	char port[10];
	snprintf(port, 10, "%d", port);
	if (getaddrinfo(hostname, port, &hint, &infop) != 0) {
		perror("Couldn't lookup host.");
		return false;
	}
	if ((sd = socket(infop->ai_family, infop->ai_socktype, infop->ai_protocol)) < 0){
		perror("Couldn't create socket.");
		return false;
	}
	if (connect(sd, infop->ai_addr, infop->ai_addrlen) < 0) {
		perror("Couldn't connect.");
		close();
		return false;
	}
	freeaddrinfo(infop);
	
	if (handshake()){
		cerr << "Handshake failed!" << endl;
		return false;
	}
	initialized = true;
	return true;
}
bool Protocol::isConnected(){
	return initialized;
}
bool Protocol::close(){
	// this line won't compile on linux.
	//close(sd);
	return true;
}


/// Communicate with the server; NOTE: I did not write any of this code
bool Protocol::sendLine(const char *line) {
	int length = (int) strlen(line);
	char command[kBufferSize];
	strcpy(command, line);
	command[length] = '\n';
	command[length+1] = '\0';
	if (DEBUG) cout << command;
	return send(sd, command, length+1, 0) < 0;
}
bool Protocol::readReply(char *reply){
	char acreadBuffer[kBufferSize];
	int nNewBytes = recv(sd, acreadBuffer, kBufferSize, 0);
	if (nNewBytes < 0) return -1;
	else if (nNewBytes == 0) {
		cerr << "Connection closed by peer." << endl;
		return false;
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
bool Protocol::handshake() {
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
		return false;
	}
	return true;
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
	}
	else if (DEBUG)
		cout << "Something went wrong. Exiting!" << endl;
	return false;
}
void Protocol::printLine() {
	char str[kBufferSize];
	char *LineText=str;
	readLine(LineText);
	if (DEBUG) cout << LineText << endl;
}
	
//Tank commands
bool Protocol::shoot(int idx){
	// Perform a shoot request.
	char char_buff[20];
	sprintf(char_buff, "shoot %d", index);	
	string str_buff = "shoot";
	str_buff.append(char_buff);
	const char *command = str_buff.c_str();
	sendLine(command);
	readAck();
	return readBool();
}
bool Protocol::speed(int idx, double val){
	
}
bool Protocol::angvel(int idx, double val){
	
}
bool Protocol::accelx(int idx, double val){
	
}
bool Protocol::accely(int idx, double val){
	
}

//Fetch information
bool Protocol::initialBoard(
	GameConstants &gc,
	Polygon &base,
	vector<Tank*> &tanks,
	vector<Flag*> &flags,
	vector<Tank*> &enemy_tanks,
	vector<Flag*> &enemy_flags,
	vector<Polygon*> &obstacles
){
	//Request a dictionary of game constants
	sendLine("constants");
	readAck();
	vector<string> v = readArr();
	if (v.at(0)!="begin")
		return false;
	v.clear();
	v = readArr();
	while (v.at(0) == "constant"){
		string &name = v.at(1);
		string val = v.at(2);
		double dval = atof(val.c_str());
		//Yeah, this is inefficient; but we only run it once
		if (name == "team") gc.mycolor = val;
		else if (name == "tankalive") gc.tankalive = val;
		else if (name == "tankdead") gc.tankdead = val;
		else if (name == "worldsize") gc.worldsize = dval;
		else if (name == "tankradius") gc.tankradius = dval;
		else if (name == "tankangvel") gc.tankangvel = dval;
		else if (name == "tankspeed") gc.tankspeed = dval;
		else if (name == "linearaccel") gc.linearaccel = dval;
		else if (name == "angularaccel") gc.angularaccel = dval;
		else if (name == "shotradius") gc.shotradius = dval;
		else if (name == "shotrange") gc.shotrange = dval;
		else if (name == "shotspeed") gc.shotspeed = dval;
		else if (name == "flagradius") gc.flagradius = dval;
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
		if (v.at(1) == gc.mycolor){
			for (int idx=0; idx<count; idx++)
				tanks.push_back(new Tank(idx));
		}
		//This is the enemy
		else{
			for (int idx=0; idx<count; idx++)
				enemy_tanks.push_back(new Tank(-1));
		}
		v.clear();
		v = readArr();
	}
	if (v.at(0) != "end")
		return false;
	
	//Get a list of obstacles
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
		obstacles.push_back(p);
		v.clear();
		v = readArr();
	}
	if (v.at(0) != "end")
		return false;
	
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
		if (v.at(1) == gc.mycolor){
			for (int i=2; i<v.size(); i+=2){
				base.addPoint(
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
		if (v.at(1) == gc.mycolor)
			flags.push_back(new Flag());
		else enemy_flags.push_back(new Flag());		
		v.clear();
		v = readArr();
	}
	if (v.at(0) != "end")
		return false;
		
	//Hooray, no errors!!
	return true;
}
bool Protocol::updateBoard(
	vector<Tank*> &tanks,
	vector<Flag*> &flags,
	vector<Tank*> &enemy_tanks,
	vector<Flag*> &enemy_flags
	//vector<Shot*> &shots
){
	
}
