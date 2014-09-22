#ifndef Vector2D_H
#define	Vector2D_H

class Vector2d {
public:
	//Variables
	double data[2];
	
	//Constructors
	Vector2d();
	Vector2d(double val);
	Vector2d(double x, double y);
	Vector2d(const Vector2d& orig);
	virtual ~Vector2d();
	
	//Operations
	void setData(double val);
	void setData(double x, double y);
	void setData(const Vector2d &v);
	
	void normalize();
	const double dot(const Vector2d &v) const;
	const double sum() const;
	const double product() const;
	const double length() const;
	const double length_squared() const;
	
	//OVERLOADS
	//Unary negation
	const Vector2d operator-() const;
	//Array subscripts
	double& operator[](int idx);
	const double& operator[](int idx) const;
	
	//SCALAR OVERLOADS
	//Vector * Scalar
	const Vector2d operator*(const double& c) const;
	Vector2d& operator*=(const double& c);
	//Vector / Scalar
	const Vector2d operator/(const double& c) const;
	Vector2d& operator/=(const double& c);
	//Vector + Scalar
	const Vector2d operator+(const double& c) const;
	Vector2d& operator+=(const double& c);
	//Vector - Scalar
	const Vector2d operator-(const double& c) const;
	Vector2d& operator-=(const double& c);
	
	//VECTOR OVERLOADS
	//Vector / Vector (piecewise division)
	const Vector2d operator/(const Vector2d& v) const;
	Vector2d& operator/=(const Vector2d& v);
	//Vector * Vector (dot product)
	const Vector2d operator*(const Vector2d& v) const;
	Vector2d& operator*=(const Vector2d& v);
	//Vector ^ Vector (cross product)
	const Vector2d operator^(const Vector2d& v) const;
	Vector2d& operator^=(const Vector2d& v);
	//Vector + Vector
	const Vector2d operator+(const Vector2d& v) const;
	Vector2d& operator+=(const Vector2d& v);
	//Vector - Vector
	const Vector2d operator-(const Vector2d& v) const;
	Vector2d& operator-=(const Vector2d& v);
};

//Scalar operations
const Vector2d operator*(const double& c, const Vector2d& v);
const Vector2d operator/(const double& c, const Vector2d& v);
const Vector2d operator-(const double& c, const Vector2d& v);
const Vector2d operator+(const double& c, const Vector2d& v);

#endif

