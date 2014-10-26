#include <cstdio>
#include <cmath>
#include <vector>
#include "Vector2d.h"
#include "Polygon.h"
#include "Tank.h"
#include "dumbTank.h"
#include "Flag.h"
#include "Protocol.h"
#include "Grid.h"
#include "ExplorerTank.h"
#include <unistd.h>
#include "glfw3/glfw3.h"
#include "freeimage/FreeImage.h"
#include <sys/stat.h>

#define SCREENCAST_DIR "screencast/"
#define OCCGRID_WIDTH 200
#define DO_SCREENCAST 1

using namespace std;

GameConstants gc;
Polygon base;
Grid *grid;
vector<ExplorerTank*> tanks;
vector<Flag*> flags;
vector<ExplorerTank*> enemy_tanks;
vector<Flag*> enemy_flags;
vector<Polygon*> obstacles;

unsigned char* img_buffer;
char grid_origin[OCCGRID_WIDTH*OCCGRID_WIDTH];
float grid_blurred[OCCGRID_WIDTH*OCCGRID_WIDTH];
int WIN_SIZE;
bool SHOULD_CLOSE = false;

void graphFields();
void *visualize(void *args);
void save_buffer(int time);
void exit_program();

int main(int argc, char** argv){
	//Connect to the server
	char* hostname = NULL;
	int port = 50100;
	if (argc == 2) port = atoi(argv[1]);
	else if (argc == 3){
		hostname = argv[1];
		port = atoi(argv[2]);
	}
	Tank::protocol = Protocol(hostname, port);
	dumbTank::protocol = Tank::protocol;
	if (!Tank::protocol.isConnected()){
		cout << "Can't connect to BZRC server!" << endl;
		exit(1);
	}
	//Initialize the board
	if (!Tank::protocol.initialBoard(gc, base, tanks, flags, enemy_tanks, enemy_flags, obstacles, false)){
		cout << "Failed to initialize board!" << endl;
		exit(1);
	}
	Tank::protocol.updateBoard(gc, tanks, flags, enemy_tanks, enemy_flags);
	Grid::truepositive = gc.truepositive;
	Grid::truenegative = gc.truenegative;
	WIN_SIZE = (int) gc.worldsize;
	grid = new Grid(WIN_SIZE, WIN_SIZE);
	
	//Start visualization thread
	pthread_t viz_thread;
	pthread_create(&viz_thread, NULL, visualize, NULL);
	
	//Randomly assign goals
	/*
	int goal = 0, num_flags = enemy_flags.size();
	for (int i=0; i<tanks.size(); i++){
		tanks[i]->goals.push_back(enemy_flags[goal]);
		goal = (goal+1) % num_flags;
	}
	*/
	
	int idx = 0;
	while(true){
		Tank::protocol.updateBoard(gc, tanks, flags, enemy_tanks, enemy_flags);
		for (int i=0; i < tanks.size(); i++){
			tanks[i]->evalPfield(idx, gc, *grid);
			Tank::protocol.updateGrid(gc, *grid, i, grid_origin, grid_blurred);
		}
		usleep(3000);
		idx++;
	}
	
	SHOULD_CLOSE = true;
	while (SHOULD_CLOSE) ;
	delete grid;
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
				force += .4*obstacles[p]->potentialField(Vector2d(i, j), dummy_dir);
			force -= 40*enemy_flags[0]->potentialField(Vector2d(i, j), dummy_dir);
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
	
	int tank_size = tanks.size();
	int frame = 0, frame_num = 0;
	while (!glfwWindowShouldClose(window)){
		glClear(GL_COLOR_BUFFER_BIT);
		glDrawPixels(WIN_SIZE, WIN_SIZE, GL_LUMINANCE, GL_FLOAT, grid->grid);
		//Tanks
		glColor3f(1,.5,0);
		glBegin(GL_POINTS);
		for (int i=0; i<tank_size; i++)
			glVertex2dv(tanks[i]->loc.data);
		glEnd();
		//Goals
		glColor3f(0,1,0);
		glBegin(GL_POINTS);
		for (int i=0; i<tank_size; i++){
			for (int j=0; j<tanks[i]->goals.size(); j++)
				glVertex2dv(tanks[i]->goals[j]->loc.data);
		}
		glEnd();
		//Obstacles
		glColor3f(0,0,1);
		glBegin(GL_POINTS);
		for (int i=0; i<tank_size; i++){
			for (int j=0; j<tanks[i]->obstacles.size(); j++)
				glVertex2dv(tanks[i]->obstacles[j]->loc.data);
		}
		glEnd();
		glfwSwapBuffers(window);
		usleep(10000);
		if (DO_SCREENCAST && frame++ % 400 == 0)
			save_buffer(frame_num++);
		glfwPollEvents();
		if (SHOULD_CLOSE) break;
	}

	//Exit
	FreeImage_DeInitialise();
	delete[] img_buffer;
	
	glfwDestroyWindow(window);
	glfwTerminate();
	SHOULD_CLOSE = false;
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
