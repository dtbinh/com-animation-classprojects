#ifndef MATH3D_H
#define MATH3D_H

#include <math.h>

const double PI = 3.14159265358979323846f;		// Pi
#define	DEGTORAD(x)	( ((x) * PI) / 180.0f )
#define	RADTODEG(x)	( ((x) * 180.0f) / PI )
#define deg2rad PI / 180.0f
#define rad2deg 180.0f/ PI

class Vector3
{

public:
	Vector3(); //default constructor
	Vector3(Vector3& v);
	Vector3(const Vector3& v);
	Vector3(double x, double y, double z);

	void flip(); // reverse this vector
	void set(Vector3 v);
	void set(double x, double y, double z);
	void setDiff(Vector3& a, Vector3& b);
	Vector3 normalize();

	Vector3 cross(Vector3 b); //return this cross b
	double dot(Vector3 b); // return this dotted with b

	Vector3 operator +(const Vector3 & rhs);
	Vector3 operator -(const Vector3 & rhs);
	Vector3 operator *(const double scalar) const;
	Vector3 operator /(const double scalar) const;
	Vector3& operator =(const Vector3 & rhs);
	static Vector3 interpolate(Vector3 v0, Vector3 v1, double t);

	inline double norm(){return x*x+y*y+z*z;}
	inline double length(){return sqrt(norm());}

public:
	double x,y,z;
};

class Matrix4 
{
public:	
	// Default Constructor
	Matrix4();
	// Additional constructor for easy initialization
	Matrix4 (double f00, double f01, double f02, double f03,
		double f10, double f11, double f12, double f13, 
		double f20, double f21, double f22, double f23,
		double f30, double f31, double f32, double f33);
	// Additional constructor for easy initialization
	Matrix4(const double mat[16]);
	Matrix4(const Matrix4& pCopy);
	// Destructor
	~Matrix4(){};

	// Switches to Identity matrix
	void loadIdentity();
	// From matrix to Euler angle
	Vector3 ToEulerAngle(const Matrix4& m);
	// Frome Euler to matrix
	Matrix4 FromEulerAngle(const Vector3& v);

	// Negates
	void negate();
	// returns Transpose
	Matrix4 getTranspose() const;
	// Switches to inverse of this matrix.
	Matrix4 getInverseTransform() const;
	// double* cast overloaded
	operator double*() {return &m_data[0];};
	// double* cast overloaded
	operator const double*() const {return &m_data[0];}
	// * operator overloaded (scalar)
	Matrix4 operator *(const double scalar) const;
	// * operator overloaded (vector)
	Vector3 operator *(const Vector3& rhs) const;
	// * operator overloaded (matrix)
	Matrix4 operator *(const Matrix4& rhs);
	// () operator overloaded
	double& operator() (unsigned int i, unsigned int j){return m_data[i*4 + j];};
	// [] operator overloaded
	double& operator[](int i){return m_data[i];};
	// [] operator overloaded (const version)
	double operator[](int i) const {return m_data[i];};
	// = operator overloaded
	Matrix4& operator=(const Matrix4 & rhs);
	const Matrix4& operator*= (double scale);


	// Accessor function for the data member
	const double* getMatrix(){return &m_data[0];};
	void SetElement(int row, int col, double value) {m_data[4*row + col] = value;}
	double GetElement(int row, int col) {return m_data[4*row + col];}

	static Matrix4 Zrotate(double theta);
	static Matrix4 Xrotate(double theta);
	static Matrix4 Yrotate(double theta);
private:
	// Matrix data stored here
	double m_data[16];
};

class Quaternion  
{
public:
	Quaternion() : x(0), y(0), z(0), w(1.0f) {};
	Quaternion(double ix, double iy, double iz, double iw);
	Quaternion(const Quaternion & rhs);

	void Identity();
	Quaternion& FromRotationMatrix(Matrix4 &mat);
	Matrix4 ToRotationMatrix() const;
    Quaternion& FromEulerAngle(Vector3 &v);
	Quaternion& FromEulerAngle_1(Vector3 &v);
	Vector3 ToEulerAngle() const;
	// Compute the rotation quaternion from v0 to v1
	Quaternion& RotationArc(Vector3 v0, Vector3 v1);
	
	inline double Norm() const {return x*x + y*y + z*z + w*w;}
	Quaternion Normalize();
	double Length(){return sqrt(Norm());}
	bool IsReal();
	Quaternion Log();
	static Quaternion Double(Quaternion a, Quaternion b);
	static Quaternion Bisect(Quaternion a, Quaternion b);
	double DotProduct(const Quaternion& other) const;
	Quaternion Inverse() const;
	Quaternion Conjugate() const;

	friend Quaternion operator -(Quaternion const&);
	Quaternion& operator =(const Quaternion& rhs);
	Quaternion operator +(const Quaternion& rhs) const;
	Quaternion operator -(const Quaternion& rhs) const;
	Quaternion operator *(double s) const;
	Quaternion operator *(const Quaternion& rhs) const;
	friend Quaternion operator*( double, Quaternion const& );
	Quaternion& operator *=(double s);
	Quaternion operator /(double s) const;

	static void ShortArcCheck(Quaternion& q0, Quaternion& q1) {if(q0.DotProduct(q1) < 0) q1 = q1*-1;}
	// Interpolates the quaternion between to quaternions based on time
	static Quaternion Slerp(Quaternion q1, Quaternion q2, double time);

public:
	double x, y, z, w;
};

#endif