#include "Vector2d.h"
#include <cmath>
#include <stdio.h>

//CONSTRUCTORS
Vector2d::Vector2d(){
	this->setData(0, 0);
}
Vector2d::Vector2d(double val){
	this->setData(val);
}
Vector2d::Vector2d(double x, double y){
	this->setData(x, y);
}
Vector2d::Vector2d(const Vector2d& orig) {
	this->setData(orig[0], orig[1]);
}
Vector2d::~Vector2d(){}

//OPERATIONS
void Vector2d::setData(double val){
	setData(val, val);
}
void Vector2d::setData(double x, double y){
	data[0] = x;
	data[1] = y;
}
void Vector2d::setData(const Vector2d &v){
	setData(v.data[0], v.data[1]);
}

void Vector2d::normalize(){
	*this /= this->length();
}
const double Vector2d::dot(const Vector2d &v) const{
	return v.data[0]*data[0] + v.data[1]*data[1];
}
const double Vector2d::sum() const{
	return data[0] + data[1];
}
const double Vector2d::product() const{
	return data[0] * data[1];
}
const double Vector2d::length() const{
	return sqrt(length_squared());
}
const double Vector2d::length_squared() const{
	double sum = 0;
	for (int i=0; i<2; i++)
		sum += data[i]*data[i];
	return sum;
}

//OVERLOADS
//Array subscripts
double& Vector2d::operator[](int idx){
	return data[idx];
}
const double& Vector2d::operator[](int idx) const{
	return data[idx];
}

//SCALAR OVERLOADS
//Vector * Scalar
const Vector2d operator*(const double& c, const Vector2d& v){
	return Vector2d(v)*c;
}
const Vector2d Vector2d::operator*(const double& c) const{
	return Vector2d(*this) *= c;
}
Vector2d& Vector2d::operator*=(const double& c){
	data[0] *= c;
	data[1] *= c;
	return *this;
}
//Vector / Scalar
const Vector2d operator/(const double& c, const Vector2d& v){
	return Vector2d(v)/c;
}
const Vector2d Vector2d::operator/(const double& c) const{
	return Vector2d(*this) /= c;
}
Vector2d& Vector2d::operator/=(const double& c){
	data[0] /= c;
	data[1] /= c;
	return *this;
}
//Vector + Scalar
const Vector2d operator+(const double& c, const Vector2d& v){
	return Vector2d(v)+c;
}
const Vector2d Vector2d::operator+(const double& c) const{
	return Vector2d(*this) += c;
}
Vector2d& Vector2d::operator+=(const double& c){
	data[0] += c;
	data[1] += c;
	return *this;
}
//Vector - Scalar
const Vector2d operator-(const double& c, const Vector2d& v){
	return Vector2d(v)-c;
}
const Vector2d Vector2d::operator-(const double& c) const{
	return Vector2d(*this) -= c;
}
Vector2d& Vector2d::operator-=(const double& c){
	data[0] -= c;
	data[1] -= c;
	return *this;
}

//VECTOR OVERLOADS
//Vector / Vector (piecewise division)
const Vector2d Vector2d::operator/(const Vector2d& v) const{
	return Vector2d(*this) /= v;
}
Vector2d& Vector2d::operator/=(const Vector2d& v){
	data[0] /= v.data[0];
	data[1] /= v.data[1];
	return *this;
}
//Vector * Vector (dot product)
const Vector2d Vector2d::operator*(const Vector2d& v) const{
	return Vector2d(*this) *= v;
}
Vector2d& Vector2d::operator*=(const Vector2d& v){
	data[0] *= v.data[0];
	data[1] *= v.data[1];
	return *this;
}
//Vector ^ Vector (cross product)
const Vector2d Vector2d::operator^(const Vector2d& v) const{
	return Vector2d(*this) ^= v;
}
Vector2d& Vector2d::operator^=(const Vector2d& v){
	//TODO: this may be incorrect...
	double v1 = data[0]*v.data[1],
		  v2 = -data[1]*v.data[0];
	data[0] = v1;
	data[1] = v2;
	return *this;
}
//Vector + Vector
const Vector2d Vector2d::operator+(const Vector2d& v) const{
	return Vector2d(*this) += v;
}
Vector2d& Vector2d::operator+=(const Vector2d& v){
	data[0] += v.data[0];
	data[1] += v.data[1];
	return *this;
}
//Vector - Vector
const Vector2d Vector2d::operator-(const Vector2d& v) const{
	return Vector2d(*this) -= v;
}
Vector2d& Vector2d::operator-=(const Vector2d& v){
	data[0] -= v.data[0];
	data[1] -= v.data[1];
	return *this;
}
//Unary negation
const Vector2d Vector2d::operator-() const{
	return Vector2d(-data[0], -data[1]);
}
