#ifndef POLYGON_H
#define	POLYGON_H

#include <vector>
#include <math.h>
#include "Vector2d.h"
#include "Field.h"

class Field;

class Polygon : public Field {
private:
	double area_cache;
	Vector2d center;
	std::vector<Vector2d> vertices;
	
public:
	Polygon();
	Polygon(bool attract);
	Polygon(const Polygon& orig);
	virtual ~Polygon();
	
	//Add vertex to back of vertex list (clockwise vertex convention)
	void addPoint(Vector2d p);
	void addPoint(double x, double y);
	//Point access
	int size();
	const Vector2d& operator[](int idx) const;
	
	//Does this shape contain this point
	bool contains(const Vector2d &p) const;
	bool contains(double x, double y) const;
	//Compute area of shape
	double area();
	//Bounding box for shape
	void bounds(double bounds[4]);
	//Get center of shape
	Vector2d centroid();
	//Check if line segment intersects polygon
	bool isect(Vector2d l1, Vector2d l2) const;
	//Compute convex hull of polygon
	const Polygon convexHull();
	//Join this poly with another, if they intersect; returns true if they intersect
	bool join(const Polygon &shape);
	//Compute potential field of polygon at point
	const Vector2d potentialField(const Vector2d &station, const Vector2d &dir) const;
};

#endif

