/*
TODO:
- make Abstract tank
- commandline args
- pre/post agent code
- port over to Eigen library
- Kalman filter
- shots/intersection code
- intelligent filtering of obstacles
- vectorize method in Grid
*/

#include <cstdio>
#include <cmath>
#include <ctime>
#include <vector>
#include <unistd.h>
#include "glfw3/glfw3.h"
#include "freeimage/FreeImage.h"
#include <sys/stat.h>

#include "Vector2d.h"
#include "Polygon.h"
#include "Flag.h"
#include "Protocol.h"
#include "Grid.h"
#include "Tanks/WildTank.h"

#define SCREENCAST_DIR "screencast/"
#define DO_SCREENCAST 1

using namespace std;

Board board;
unsigned char* img_buffer;
int WIN_SIZE;
bool SHOULD_CLOSE = false;

void graphFields();
void *visualize(void *args);
void save_buffer(int time);
void exit_program();

int main(int argc, char** argv){
	srand(time(NULL));
	//Parse command line arguments
	if (argc > 0){
		for (int i=0; i<argc; i++){
			printf("%s\n", argv[i]);
		}
	}
	//Connect to the server
	char* hostname = NULL;
	int port = 50100;
	if (argc == 2) port = atoi(argv[1]);
	else if (argc == 3){
		hostname = argv[1];
		port = atoi(argv[2]);
	}
	Protocol *p = new Protocol(hostname, port);
	if (!p->isConnected()){
		cout << "Can't connect to BZRC server!" << endl;
		exit(1);
	}
	//Initialize the board
	board.p = p;
	board.gc.friendlyfire = true;
	board.gc.grabownflag = true;
	board.gc.usegrid = false;
	board.gc.gridwidth = 200;
	if (!p->initialBoard<WildTank>(board)){
		cout << "Failed to initialize board!" << endl;
		exit(1);
	}
	if (board.tanks.size()){
		WIN_SIZE = (int) board.gc.worldsize;
		p->updateBoard(0, board);
		
		//Start visualization thread
		pthread_t viz_thread;
		pthread_create(&viz_thread, NULL, visualize, NULL);

		int idx = 0;
		while (!SHOULD_CLOSE){
			if (idx % 50)
				p->updateGrid(board);
			double time = glfwGetTime();
			glfwSetTime(0);
			p->updateBoard(time, board);
			board.tanks[0]->coordinate(time);
			for (int i=0; i < board.tanks.size(); i++)
				board.tanks[i]->move(time);
			usleep(1000*100);
			idx++;
		}
	}

	delete board.grid;
	delete board.p;
	return 0;
}

/*** VISUALIZATION CODE ***/

void graphFields(){
	//Generate gnu-plot data for the field
	Vector2d min = Vector2d(-400), max = Vector2d(400);
	double spacing = 20;
	FILE *f = fopen("fields.gpi", "w+");
	//Initialize graph
	fprintf(f, "set xrange [%f: %f]\n", min[0], max[0]);
	fprintf(f, "set yrange [%f: %f]\n", min[1], max[1]);
	fprintf(f, "unset key\nset size square\n");
	//Draw obstacles
	fprintf(f, "unset arrow\n");
	for (int p=0; p<board.obstacles.size(); p++){
		Polygon *poly = board.obstacles[p];
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
			
			for (int p=0; p<board.obstacles.size(); p++)
				force += .4*board.obstacles[p]->potentialField(Vector2d(i, j), dummy_dir);
			force -= 40*board.enemy_flags[0]->potentialField(Vector2d(i, j), dummy_dir);
			force *= .8;
			fprintf(f, "%f %f %f %f\n", i, j, force[0], force[1]);
		}
	}
	//Close plot file
	fclose(f);
}

//Print all errors to console
static void error_callback(int error, const char* description){
	printf("\nError: %s",description);
}
void *visualize(void *args){
	//Create GLFW window
	GLFWwindow* window;
	glfwSetErrorCallback(error_callback);
	if (!glfwInit())
		exit(EXIT_FAILURE);
	window = glfwCreateWindow(WIN_SIZE, WIN_SIZE, "Tank Visualizer", NULL, NULL);
	if (!window){
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);
	
	//Center window on screen
	const GLFWvidmode* monitor = glfwGetVideoMode(glfwGetPrimaryMonitor());
	glfwSetWindowPos(window, (monitor->width-WIN_SIZE)/2, (monitor->height-WIN_SIZE)/2);
	
	//Setup OpenGL context
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, WIN_SIZE, WIN_SIZE);
	float div_2 = WIN_SIZE/2.0;
	glOrtho(-div_2, div_2, -div_2, div_2, 0, 1);
	glPointSize(4);
	//glEnable(GL_POINT_SMOOTH);
	
	//Drawing & event loop
	//Create directory to save buffers in
	mkdir(SCREENCAST_DIR,0777);
	FreeImage_Initialise();
	img_buffer = new unsigned char[WIN_SIZE*WIN_SIZE*3];
	
	int tank_size = board.tanks.size();
	int enemy_tank_size = board.enemy_tanks.size();
	int frame = 0, frame_num = 0;
	while (!glfwWindowShouldClose(window)){
		glClear(GL_COLOR_BUFFER_BIT);
		if (board.gc.usegrid)
			glDrawPixels(WIN_SIZE, WIN_SIZE, GL_LUMINANCE, GL_FLOAT, board.grid->grid);
		//Obstacles
		glColor3f(0,0,1);
		glBegin(GL_POINTS);
		for (int i=0; i<board.obstacles.size(); i++){
			for (int p=0; p<board.obstacles[i]->size(); p++)
				glVertex2dv((*board.obstacles[i])[p].data);
		}
		glEnd();
		//Tanks
		glColor3f(0,1,0);
		glBegin(GL_POINTS);
		for (int i=0; i<tank_size; i++)
			glVertex2dv(board.tanks[i]->pos.data);
		glEnd();
		//Enemy tanks
		glColor3f(1,0,0);
		glBegin(GL_POINTS);
		for (int i=0; i<enemy_tank_size; i++)
			glVertex2dv(board.enemy_tanks[i]->pos.data);
		glEnd();
		/*
		//Goals
		glColor3f(0,1,0);
		glBegin(GL_POINTS);
		for (int i=0; i<tank_size; i++){
			for (int j=0; j<board.tanks[i]->goals.size(); j++)
				glVertex2dv(board.tanks[i]->goals[j]->loc.data);
		}
		glEnd();
		*/
		glfwSwapBuffers(window);
		usleep(10000);
		if (DO_SCREENCAST && frame++ % 400 == 0)
			save_buffer(frame_num++);
		glfwPollEvents();
	}

	//Exit
	FreeImage_DeInitialise();
	delete[] img_buffer;
	
	glfwDestroyWindow(window);
	glfwTerminate();
	SHOULD_CLOSE = true;
	pthread_exit(NULL);
}

void save_buffer(int time){
	FILE *file;
	char fname[32];
	
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	sprintf(fname, "%st_%04d.png", SCREENCAST_DIR, time);
	printf("%s\n", fname);
	
	//Copy the image to buffer
	glReadBuffer(GL_BACK_LEFT);
	glReadPixels(0, 0, WIN_SIZE, WIN_SIZE, GL_BGR, GL_UNSIGNED_BYTE, img_buffer);
	FIBITMAP* img = FreeImage_ConvertFromRawBits(
		img_buffer, WIN_SIZE, WIN_SIZE, 3*WIN_SIZE,
		24, 0xFF0000, 0x00FF00, 0x0000FF, false
	);
	FreeImage_Save(FIF_PNG, img, fname, 0);
	FreeImage_Unload(img);
}
