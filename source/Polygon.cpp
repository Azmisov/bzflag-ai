#include "Polygon.h"

Polygon::Polygon(){
	area_cache = -1;
}
Polygon::Polygon(bool attract) : Field(attract){
	area_cache = -1;
}
Polygon::Polygon(const Polygon& orig){}
Polygon::~Polygon(){}

int Polygon::size(){
	return vertices.size();
}
const Vector2d& Polygon::operator[](int idx) const{
	return vertices[idx];
}

void Polygon::addPoint(Vector2d p){
	area_cache = -1;
	vertices.push_back(p);
}
void Polygon::addPoint(double x, double y){
	area_cache = -1;
	vertices.push_back(Vector2d(x, y));
}
bool Polygon::contains(const Vector2d &p) const{
	return contains(p[0], p[1]);
}
bool Polygon::contains(double x, double y) const{
	//Source: http://www.ecse.rpi.edu/Homepages/wrf/Research/Short_Notes/pnpoly.html	
	bool result = false;
	int len = vertices.size();
	Vector2d vi = vertices[len-1], vj;
	for (int i=0; i<len; i++){
		vj = vi;
		vi = vertices[i];
		if ((vi[1] > y) != (vj[1] > y) && (x < (vj[0] - vi[0]) * (y - vi[1]) / (vj[1]-vi[1]) + vi[0])) {
			result = !result;
		}
	}
	return result;
}
double Polygon::area(){
	if (area_cache != -1)
		return area_cache;
	//Source: http://www.mathopenref.com/coordpolygonarea2.html
	area_cache = 0;
	int len = vertices.size(), j = len-1;
	for (int i=0; i<len; j = i++){
		Vector2d &vi = vertices[i], &vj = vertices[j];
		area_cache += (vj[0]+vi[0]) * (vj[1]-vi[1]);
    }
	return fabs(area_cache/2);
}
void Polygon::bounds(double bounds[4]){
	bounds[0] = vertices[0][0]; bounds[1] = bounds[0];
	bounds[2] = vertices[0][1]; bounds[3] = bounds[2];
	for (int i=1, len=vertices.size(); i<len; i++){
		Vector2d &p = vertices[i];
		//X-bounds
		if (p[0] < bounds[0])
			bounds[0] = p[0];
		else if (p[0] > bounds[1])
			bounds[1] = p[0];
		//Y-bounds
		if (p[1] < bounds[2])
			bounds[2] = p[1];
		else if (p[1] > bounds[3])
			bounds[3] = p[1];
	}
}
//Compute potential field of polygon at point; it adds a tangential field based on point direction
const Vector2d Polygon::potentialField(const Vector2d &station, const Vector2d &dir) const{
	//Based on paper: Won and Bevis, 1987
	double min_dist = INFINITY;
	bool tang_dir = false;
	Vector2d force = Vector2d(0), edge_force;
	int len = vertices.size();
	Vector2d v2 = vertices[len-1]-station, v1;
	for (int i=0; i<len; i++){
		v1 = v2;
		v2 = vertices[i]-station;
		bool same_dir = dir.dot(v2-v1) >= 0; 
		//Station on top of vertex
		if ((fabs(v1[0]) < EPSILON && fabs(v1[1]) < EPSILON) || (fabs(v2[0]) < EPSILON && fabs(v2[1]) < EPSILON)){
			min_dist = 0;
			tang_dir = same_dir;
			continue;
		}
		//Compute angles from station to edge
		double diff_theta = atan2(v1[1], v1[0]) - atan2(v2[1], v2[0]),
			x1y2 = v1[0]*v2[1],
			x2y1 = v2[0]*v1[1];
		//Station in between edge
		if ((v1[1] < 0) != (v2[1] < 0)){
			if (fabs(x1y2-x2y1) < EPSILON){
				min_dist = 0;
				tang_dir = same_dir;
				continue;
			}
			if (x1y2 < x2y1 && v2[1] >= 0)
				diff_theta += PI_2;
			else if (x1y2 > x2y1 && v1[1] >= 0)
				diff_theta -= PI_2;
		}
		//Compute forces
		double diff_x = v2[0]-v1[0],
			lnr = log(v2.length()/v1.length());
		//Vertical edge force
		if (fabs(diff_x) < EPSILON){
			if (min_dist > v1[0]){
				min_dist = v1[0];
				tang_dir = same_dir;
			}
			edge_force = Vector2d(
				-v1[0]*diff_theta,
				v1[0]*lnr
			);
		}
		//Angled edge force
		else{
			double diff_y = v2[1]-v1[1],
				diff_square = diff_x*diff_x + diff_y*diff_y,
				dist = fabs(x2y1-x1y2)/sqrt(diff_square),
				decay = sqrt(diff_square);
			if (min_dist > dist){
				min_dist = dist;
				tang_dir = same_dir;
			}
			double A = diff_x*(x1y2-x2y1)/pow(decay,6),
				B = diff_y/diff_x;
			edge_force = Vector2d(
				A*(lnr - B*diff_theta),
				A*(B*lnr + diff_theta)
			);
		}
		//Reverse force, if repulsive field
		if (!attractive)
			edge_force = -edge_force;
		//Rotate force for tangential circling
		//Rotation direction is determined by station's current direction (prefer less change in direction)
		/*
		if (same_dir != attractive){
			//Clockwise
			force[0] += COS_TANGENTIAL*edge_force[0] + SIN_TANGENTIAL*edge_force[1];
			force[1] += COS_TANGENTIAL*edge_force[1] - SIN_TANGENTIAL*edge_force[0];
		}
		else{
			//Counter-clockwise
			force[0] += COS_TANGENTIAL*edge_force[0] - SIN_TANGENTIAL*edge_force[1];
			force[1] += COS_TANGENTIAL*edge_force[1] + SIN_TANGENTIAL*edge_force[0];
		}
		//*/
		force += edge_force;
    }
    //Add secondary tangential field for better obstacle avoidance
    //*
    if (min_dist < TANG_THRESHOLD){
		if (tang_dir == attractive){
			//Clockwise
			force[0] = COS_TANGENTIAL*force[0] + SIN_TANGENTIAL*force[1];
			force[1] = COS_TANGENTIAL*force[1] - SIN_TANGENTIAL*force[0];
		}
		else{
			//Counter-clockwise
			force[0] = COS_TANGENTIAL*force[0] - SIN_TANGENTIAL*force[1];
			force[1] = COS_TANGENTIAL*force[1] + SIN_TANGENTIAL*force[0];
		}
	}
	//*/
    return force;
}
