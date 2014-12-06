#ifndef BOARD_H
#define	BOARD_H

typedef struct Board{
	GameConstants &gc;
	Polygon &base;
	vector<Tank*> &tanks;
	vector<Flag*> &flags;
	vector<Tank*> &enemy_tanks;
	vector<Flag*> &enemy_flags;
	vector<Polygon*> &obstacles;
} Board;

#endif
