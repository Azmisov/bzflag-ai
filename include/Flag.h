#ifndef FLAG_H
#define	FLAG_H

#include <string>
#include "Circle.h"
#include "Vector2d.h"

class Circle;

class Flag : public Circle{
public:
	//Who posses this flag?
	bool isPossessed;
	bool havePossession;
	
	Flag();
	Flag(bool attract);
	Flag(const Flag& orig);
	virtual ~Flag();
};

#endif
