#include "Protocol.h"

Protocol::Protocol(){}
Protocol::Protocol(const char* hostname, int port){
	this->hostname = hostname;
	this->port = port;
	if (!init()) {
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
	char port_str[10];
	snprintf(port_str, 10, "%d", port);
	int error = getaddrinfo(hostname, port_str, &hint, &infop);
	if (error != 0){
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
    list_buffer.clear();
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
	//Perform a shoot request.
	char char_buff[20];
	sprintf(char_buff, "shoot %d", idx);
	sendLine(char_buff);
	readAck();
	return readBool();
}
bool Protocol::speed(int idx, double val){
	//Set the desired speed to the specified value.
	char char_buff[20];
	sprintf(char_buff, "speed %d %f", idx, val);
	sendLine(char_buff);
	readAck();
	return readBool();
}
bool Protocol::angvel(int idx, double val){
	//Set the desired angular velocity to the specified value.
	char char_buff[20];
	sprintf(char_buff, "angvel %d %f", idx, val);
	sendLine(char_buff);
	readAck();
	return readBool();
}
bool Protocol::accelx(int idx, double val){
	//Set the desired accelaration in x axis to the specified value in hovertank mode.
	char char_buff[20];
	sprintf(char_buff, "accelx %d %f", idx, val);
	sendLine(char_buff);
	readAck();
	return readBool();
}
bool Protocol::accely(int idx, double val){
	//Set the desired accelaration in x axis to the specified value in hovertank mode.
	char char_buff[20];
	sprintf(char_buff, "accely %d %f", idx, val);
	sendLine(char_buff);
	readAck();
	return readBool();
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
	if (v.at(0) != "begin")
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
		else if (name == "truenegative") gc.truenegative = dval;
		else if (name == "truepositive") gc.truepositive = dval;
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
		Flag *f = new Flag();
		f->radius = gc.flagradius;
		if (v.at(1) == gc.mycolor)
			flags.push_back(f);
		else enemy_flags.push_back(f);		
		v.clear();
		v = readArr();
	}
	if (v.at(0) != "end")
		return false;
		
	//Hooray, no errors!!
	return true;
}
bool Protocol::updateBoard(
	GameConstants &gc,
	vector<Tank*> &tanks,
	vector<Flag*> &flags,
	vector<Tank*> &enemy_tanks,
	vector<Flag*> &enemy_flags
	//vector<Shot*> &shots
){
	//Update tank positions
	sendLine("mytanks");
	readAck();
	vector<string> v = readArr();
	if (v.at(0) != "begin")
		return false;
	v.clear();
	v = readArr();
	vector<Tank*>::iterator tank_iter = tanks.begin();
	while(v.at(0) == "mytank"){
		//Update alive/dead status
		bool alive = v.at(3) == gc.tankalive;
		if (alive){
			if ((*tank_iter)->mode == DEAD)
				(*tank_iter)->mode = IDLE;
		}
		else (*tank_iter)->mode = DEAD;
		//Update position
		(*tank_iter)->loc = Vector2d(
			atof(v.at(7).c_str()),
			atof(v.at(8).c_str())
		);
		//Update angle
		double theta = atof(v.at(9).c_str());
		(*tank_iter)->dir = Vector2d(cos(theta), sin(theta));
		//Update velocities
		(*tank_iter)->vel_linear = Vector2d(
			atof(v.at(10).c_str()),
			atof(v.at(11).c_str())
		);
		(*tank_iter)->vel_angular = atof(v.at(12).c_str());
		//MyTank.shots_avail=atoi(v.at(4).c_str());
		//MyTank.time_to_reload=atof(v.at(5).c_str());
		//MyTank.flag=v.at(6);
		tank_iter++;
		v.clear();
		v=readArr();
	}
	if (v.at(0) != "end")
		return false;
	
	//Update enemy tanks
	sendLine("othertanks");
	readAck();
	v = readArr();
	if (v.at(0) != "begin")
		return false;
	v.clear();
	v = readArr();
	tank_iter = enemy_tanks.begin();
	while (v.at(0) == "othertank"){
		//Update alive/dead status
		bool alive = v.at(3) == gc.tankalive;
		(*tank_iter)->mode = alive ? IDLE : DEAD;
		//Update position
		(*tank_iter)->loc = Vector2d(
			atof(v.at(5).c_str()),
			atof(v.at(6).c_str())
		);
		//Update angle
		double theta = atof(v.at(7).c_str());
		(*tank_iter)->dir = Vector2d(cos(theta), sin(theta));
		//OtherTank.flag=v.at(4);
		tank_iter++;
		v.clear();
		v = readArr();
	}
	if (v.at(0) != "end")
		return false;
	
	//Update flag positions and possession
	sendLine("flags");
	readAck();
	v = readArr();
	if (v.at(0) != "begin")
		return false;
	v.clear();
	v = readArr();
	vector<Flag*>::iterator flags_iter = flags.begin();
	vector<Flag*>::iterator enemy_flags_iter = enemy_flags.begin();
	while (v.at(0) == "flag"){
		Vector2d pos = Vector2d(
			atof(v.at(3).c_str()),
			atof(v.at(4).c_str())
		);
		bool isPossessed = v.at(2) != "none";
		bool havePosession = v.at(2) == gc.mycolor;
		//vector<Flag*>::iterator &ref = v.at(1) == gc.mycolor ? flags_iter : enemy_flags_iter;
		if (v.at(1) == gc.mycolor){
			(*flags_iter)->loc = pos;
			(*flags_iter)->isPossessed = isPossessed;
			(*flags_iter)->havePosession = havePosession;
			flags_iter++;
		}
		else{
			(*enemy_flags_iter)->loc = pos;
			(*enemy_flags_iter)->isPossessed = isPossessed;
			(*enemy_flags_iter)->havePosession = havePosession;
			enemy_flags_iter++;
		}
		v.clear();
		v = readArr();
	}
	if (v.at(0) != "end")
		return false;
		
	//Hooray, there were no errors!
	return true;
}
bool Protocol::updateGrid(Grid &g, int tank_idx){
	//Get grid for a single tank
	char char_buff[13];
	sprintf(char_buff, "occgrid %d", tank_idx);
	sendLine(char_buff);
	readAck();
	vector<string> v = readArr();
	if (v.at(0) != "begin")
		return false;
	//Get occgrid dimensions
	v = readArr();
	int rx, ry, rw, rh;
	if (sscanf(v.at(1).c_str(), "%d,%d", &rx, &ry) != 2)
		return false;
	v = readArr();
	if (sscanf(v.at(1).c_str(), "%dx%d", &rw, &rh) != 2)
		return false;
	//Get occgrid values
	for (int i=0; i<rw; i++, rx++){
		v = readArr();
		string& occ = v.at(0);
		if (occ.size() != rh)
			return false;
		for (int j=0; j<rh; j++)
			g.updateCell(rx, ry+j, occ[j] == '1');
	}
	v = readArr();
	if (v.at(0) != "end")
		return false;
	
	//No errors!
	return true;
}
