#include <memory.h>
#include "Math3D.h"

Vector3::Vector3()
{
	x = 0;
	y = 0;
	z = 0;
}

Vector3::Vector3(Vector3& v)
{
	x = v.x;
	y = v.y;
	z = v.z;
}

Vector3::Vector3(const Vector3& v)
{
	x = v.x;
	y = v.y;
	z = v.z;
}

Vector3::Vector3(double x, double y, double z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

void Vector3::set(Vector3 v)
{
	x = v.x;
	y = v.y;
	z = v.z;
}

void Vector3::set(double x, double y, double z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

void Vector3::flip()
{
	x = -x;
	y = -y;
	z = -z;
}

void Vector3::setDiff(Vector3& a, Vector3& b)
{
	x = a.x - b.x;
	y = a.y - b.y;
	z = a.z - b.z;
}

Vector3 Vector3::normalize()
{
	if(norm() < 0.0000001)
	{
		set(1,1,1);
		normalize();
		return *this;
	}

	x /= length();
	y /= length();
	z /= length();
	return *this;
}

Vector3 Vector3::cross(Vector3 b) //return this cross b
{
	return Vector3(y*b.z - z*b.y, z*b.x - x*b.z, x*b.y - y*b.x);
}

double Vector3::dot(Vector3 b) // return this dotted with b
{
	return x * b.x + y * b.y + z * b.z;
}

Vector3 Vector3::operator +(const Vector3 & rhs)
{
	Vector3 r;

	r.x = x + rhs.x;
	r.y = y + rhs.y;
	r.z = z + rhs.z;

	return r;
}

Vector3 Vector3::operator -(const Vector3 & rhs)
{
	Vector3 r;

	r.x = x - rhs.x;
	r.y = y - rhs.y;
	r.z = z - rhs.z;

	return r;
}

Vector3 Vector3::operator *(double scalar) const
{
	Vector3 r;

	r.x = x * scalar;
	r.y = y * scalar;
	r.z = z * scalar;

	return r;
}

Vector3 Vector3::operator /(double scalar) const
{
	return (*this)*(1/scalar);
}

Vector3& Vector3::operator =(const Vector3 &rhs)
{
	// self assignment control
	if (this == &rhs) 
		return *this;

	x = rhs.x;
	y = rhs.y;
	z = rhs.z;

    return *this;  // Assignment operator returns left side.
}

Vector3 Vector3::interpolate(Vector3 v0, Vector3 v1, double t)
{
	Vector3 inter;
	inter.x = v0.x + (v1.x - v0.x) * t;
	inter.y = v0.y + (v1.y - v0.y) * t;
	inter.z = v0.z + (v1.z - v0.z) * t;
	return inter;
}

double Vector3::getAngleFromZAxis(Vector3& end)
{
	Vector3 axis;
	double angle = 0.0;

	if (end.x >= 0.0 && end.z > 0.0)
	{
		axis.set(0, 0, 1);
		angle = acos(axis.dot(*this))*rad2deg;
	}
	else if (end.x > 0.0 && end.z <= 0.0)
	{
		axis.set(1, 0, 0);
		angle = acos(axis.dot(*this))*rad2deg;
		angle += 90;
	}
	else if (end.x <= 0.0 && end.z < 0.0)
	{
		axis.set(0, 0, -1);
		angle = acos(axis.dot(*this))*rad2deg;
		angle += 180;
	}
	else if (end.x < 0 && end.z >=0)
	{
		axis.set(-1, 0, 0); 
		angle = acos(axis.dot(*this))*rad2deg;
		angle += 270;
	}
	return angle;

}

Matrix4::Matrix4()
{
	loadIdentity();
}

Matrix4::Matrix4 (	double f00, double f01, double f02, double f03,
					double f10, double f11, double f12, double f13, 
					double f20, double f21, double f22, double f23,
					double f30, double f31, double f32, double f33)
{
	m_data[0]  = f00; m_data[1]  = f01; m_data[2]  = f02; m_data[3]  = f03;
	m_data[4]  = f10; m_data[5]  = f11; m_data[6]  = f12; m_data[7]  = f13;
	m_data[8]  = f20; m_data[9]  = f21; m_data[10] = f22; m_data[11] = f23;
	m_data[12] = f30; m_data[13] = f31; m_data[14] = f32; m_data[15] = f33;
}

Matrix4::Matrix4(const double mat[16])
{
	memcpy(m_data, mat, 16*sizeof(double));
}

Matrix4::Matrix4(const Matrix4& pCopy)
{
	memcpy(m_data, pCopy.m_data, 16*sizeof(double));
}

void Matrix4::loadIdentity()
{
	memset(m_data, 0, 16*sizeof(double));
	m_data[0] = m_data[5] = m_data[10] = m_data[15] = 1.0f;
}

Vector3 Matrix4::ToEulerAngle(const Matrix4& m)
{
	Vector3 r;
	double siny = -m[2];
    double cosy = sqrt( 1.0f - siny*siny );

	double sinx;
	double cosx;

	double sinz;
	double cosz;

	if ( cosy > 0.0000001 )
	{
	    sinx = m[6] / cosy;
		cosx = m[10] / cosy;

		sinz = m[1] / cosy;
		cosz = m[0] / cosy;
	}
	else
	{
	    sinx = - m[9];
		cosx =   m[5];

		sinz = 0.0;
		cosz = 1.0;
	}

	r.x = atan2( sinx, cosx );
	r.y = atan2( siny, cosy );
	r.z = atan2( sinz, cosz );

    return r;
}

Matrix4 Matrix4::FromEulerAngle(const Vector3& v)
{
	//Matrix4 m = Xrotate(v.x) * Yrotate(v.y) * Zrotate(v.z);
	Matrix4 m = Zrotate(v.z) * Yrotate(v.y) * Xrotate(v.x);
	*this = m;
	return m;
}



void Matrix4::negate(void)
{
	for(int i=0; i<16; i++)
		m_data[i] = -m_data[i];
}

/*
	Returns the transpose of the current matrix data. 
*/
Matrix4 Matrix4::getTranspose() const
{
	return Matrix4(	m_data[0], m_data[4], m_data[8],  m_data[12],
					m_data[1], m_data[5], m_data[9],  m_data[13],
					m_data[2], m_data[6], m_data[10], m_data[14],
					m_data[3], m_data[7], m_data[11], m_data[15]);
}

Matrix4 Matrix4::getInverseTransform() const
{
	//////////////////////////////////////////////
	//// Calculate Transpose Of Source Matrix ////
	//////////////////////////////////////////////
 	
	Matrix4 temp;
	temp.m_data[0 ] = m_data[0];
	temp.m_data[1 ] = m_data[4];
	temp.m_data[2 ] = m_data[8];

	temp.m_data[4 ] = m_data[1];
	temp.m_data[5 ] = m_data[5];
	temp.m_data[6 ] = m_data[6];

	temp.m_data[8 ] = m_data[2];
	temp.m_data[9 ] = m_data[6];
	temp.m_data[10] = m_data[10];

	//////////////////////////////////////////////////////////////////////
	//// Transform Negated Source Position Into New Coordinate System ////
	//////////////////////////////////////////////////////////////////////

	temp.m_data[12] = (-m_data[12]*temp.m_data[0])+(-m_data[13]*temp.m_data[4])+(-m_data[14]*temp.m_data[8]);
	temp.m_data[13] = (-m_data[12]*temp.m_data[1])+(-m_data[13]*temp.m_data[5])+(-m_data[14]*temp.m_data[9]);
	temp.m_data[14] = (-m_data[12]*temp.m_data[2])+(-m_data[13]*temp.m_data[6])+(-m_data[14]*temp.m_data[10]);

	temp.m_data[3 ] = temp.m_data[7] = temp.m_data[11] = 0;
	temp.m_data[15] = 1.0f;

	return temp;
}

/*
	Matrix by Scalar Multiplication operator overloaded.
*/
Matrix4 Matrix4::operator *(double scalar) const
{
	Matrix4 result;

	for(int i = 0; i < 16; ++i)
		result[i] = m_data[i] * scalar;

	return result;
}

/*
	Matrix by Vector Multiplication operator overloaded.
*/
Vector3 Matrix4::operator *(const Vector3& rhs) const
{
	double tx = 
		  m_data[0] * rhs.x 
		+ m_data[4] * rhs.y 
		+ m_data[8] * rhs.z 
		+ m_data[12];
	double ty = 
		  m_data[1] * rhs.x 
		+ m_data[5] * rhs.y 
		+ m_data[9] * rhs.z 
		+ m_data[13];
	double tz = 
		  m_data[2] * rhs.x 
		+ m_data[6] * rhs.y 
		+ m_data[10]* rhs.z 
		+ m_data[14];
	return Vector3(tx,ty,tz);
}

/*
	Matrix by Matrix Multiplication operator overloaded.
*/
Matrix4 Matrix4::operator *(const Matrix4& rhs)
{
	Matrix4 result;

	double	sum;
	int	index, alpha, beta;		// loop vars
	
	for (index = 0; index < 4; index++)			// perform multiplcation the slow and safe way
	{
		for (alpha = 0; alpha < 4; alpha++)
		{
			sum = 0.0f;

			for (beta = 0; beta < 4; beta++)
				sum += m_data[index + beta*4] * rhs[alpha*4 + beta];

			result[index + alpha*4] = (double)sum;
		}	// end for alpha
	}	// end for index

	return result;
}

Matrix4& Matrix4::operator =(const Matrix4 &rhs)
{
	// self assignment control
	if (this == &rhs) 
		return *this;

	memcpy(m_data, rhs.m_data, 16*sizeof(double));
    
    return *this;  // Assignment operator returns left side.
}

const Matrix4& Matrix4::operator *=(double scale)
{
	for(int i=0;i<16;i++)
		m_data[i] *= scale;

	return *this;
}

Matrix4 Matrix4::Zrotate(double theta)
{
	Matrix4 mat(	cos(theta)	, sin(theta)	, 0, 0,
					-sin(theta), cos(theta)	, 0, 0,
					0			, 0				, 1, 0,
					0			, 0				, 0, 1);
	return mat;
}

Matrix4 Matrix4::Xrotate(double theta)
{
	Matrix4 mat(	1,				  0,				 0, 0,
					0,		cos(theta),	   sin(theta), 0,
					0, 	   -sin(theta),	   cos(theta), 0,
					0,			   	  0,				 0, 1);
	return mat;
}

Matrix4 Matrix4::Yrotate(double theta)
{
	Matrix4 mat(		cos(theta), 0, -sin(theta), 0,
								  0, 1,		       0, 0,
						sin(theta), 0,  cos(theta), 0,
								  0, 0,			   0, 1);
	return mat;
}

Quaternion::Quaternion(double ix, double iy, double iz, double iw)
{
	x = ix;
	y = iy;
	z = iz;
	w = iw;
}

Quaternion::Quaternion(const Quaternion & rhs)
{
	x = rhs.x;
	y = rhs.y;
	z = rhs.z;
	w = rhs.w;
}

void Quaternion::Identity()
{
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
	w = 1.0f;
}

/*
	Passage between quaternion->matrix representation.
	Return A valid 4x4 rotation matrix.
*/
Matrix4 Quaternion::ToRotationMatrix() const
{
	Matrix4 mat;
    double fTx  = 2.0f*x;
    double fTy  = 2.0f*y;
    double fTz  = 2.0f*z;
    double fTwx = fTx*w;
    double fTwy = fTy*w;
    double fTwz = fTz*w;
    double fTxx = fTx*x;
    double fTxy = fTy*x;
    double fTxz = fTz*x;
    double fTyy = fTy*y;
    double fTyz = fTz*y;
    double fTzz = fTz*z;

	mat[0] = 1.0f - (fTyy+fTzz);
    mat[1] = fTxy - fTwz;
    mat[2] = fTxz + fTwy;
    
	mat[4] = fTxy + fTwz;
    mat[5] = 1.0f - (fTxx+fTzz);
    mat[6] = fTyz - fTwx;
    
	mat[8] = fTxz - fTwy;
    mat[9] = fTyz + fTwx;
    mat[10] = 1.0f - (fTxx+fTyy);

	mat[3]  = mat[7] = mat[11] = 0.0f;
	mat[12] = mat[13] = mat[14] = 0.0f; 
    mat[15] = 1.0f;

	return mat;
}

/*
	Passage between matrix->quaternion representation
	param mat A valid 4x4 rotation matrix
*/
Quaternion& Quaternion::FromRotationMatrix(Matrix4 &mat)
{
    double w2 = (1 + mat[0] + mat[5] + mat[10])/4;
    if(w2 > 0)
    {
        w = sqrt(w2);
        x = ( mat[9] - mat[6] ) / (4*w);
        y = ( mat[2] - mat[8] ) / (4*w);
        z = ( mat[4] - mat[1] ) / (4*w);
    }
    else
    {
        w = 0;
		double x2 = -(mat[5] + mat[10])/2;
		if(x2 >0)
		{
			x = sqrt(x2);
			y = mat[1] / (2*x);
			z = mat[2] / (2*x);
		}
		else
		{
			x = 0;
			double y2 = (1 - mat[10])/2;
			if(y2 > 0)
			{
				y = sqrt(y2);
				z = mat[6] / (2*y);
			}
			else
			{
				y = 0;
				z = 1;
			}
		}
    }

	return *this;

}

Quaternion& Quaternion::FromEulerAngle(Vector3 &v)
{
	Vector3 t = v/2;
	w = cos(t.x) * cos(t.y) * cos(t.z) + sin(t.x) * sin(t.y) * sin(t.z);
	x = sin(t.x) * cos(t.y) * cos(t.z) - cos(t.x) * sin(t.y) * sin(t.z);
	y = cos(t.x) * sin(t.y) * cos(t.z) + sin(t.x) * cos(t.y) * sin(t.z);
	z = cos(t.x) * cos(t.y) * sin(t.z) - sin(t.x) * sin(t.y) * cos(t.z);
	Normalize();
	return *this;
}

Quaternion& Quaternion::FromEulerAngle_1(Vector3 &v)
{
	Vector3 t = v/2;
	double cosX = cos(t.x), cosY = cos(t.y), cosZ = cos(t.z);
	double sinX = sin(t.x), sinY = sin(t.y), sinZ = sin(t.z);

	w = cosX * cosY * cosZ + sinX * sinY * sinZ;
	x = sinX * cosY * cosZ - cosX * sinY * sinZ;
	y = cosX * sinY * cosZ + sinX * cosY * sinZ;
	z = cosX * cosY * sinZ - sinX * sinY * cosZ;
	Normalize();
	return *this;
}

Vector3 Quaternion::ToEulerAngle() const
{
	double	sqr_x = x * x;
	double	sqr_y = y * y;
	double	sqr_z = z * z;
	double	sin_y = 2.0 * (w * y - x * z);

	Vector3 result;

	result.y = asin (sin_y);	
	result.x = atan2 (2.0 * (y * z + w * x), 1.0 - 2.0 * (sqr_x + sqr_y));
	result.z = atan2 (2.0 * (x * y + w * z), 1.0 - 2.0 * (sqr_y + sqr_z));

	return result;
}

// Compute the rotation quaternion from v0 to v1
Quaternion& Quaternion::RotationArc(Vector3 v0, Vector3 v1)
{
	v0.normalize();
	v1.normalize();
	Vector3 c = v0.cross(v1);
	double d = v0.dot(v1);
	double s = sqrt((1+d)*2);
	x = c.x / s;
	y = c.y / s;
	z = c.z / s;
	w = s / 2.0f;
	return *this;
}

Quaternion Quaternion::Normalize()
{
	double len = sqrt(Norm());
	x = double(x / len);
	y = double(y / len);
	z = double(z / len);
	w = double(w / len);
	return *this;
}

bool Quaternion::IsReal()
{
	if(Norm() < 0.00000001)
		return true;
	else
		return false;
}

Quaternion Quaternion::Log()
{
	Quaternion q;
	q.w = 0;
	double theta = acos(w);
	double sin_theta = sin(theta);
	
	if ( fabs(sin_theta) > 0.0000001)
	{
		q.x = x / sin_theta*theta;
		q.y = y / sin_theta*theta;
		q.z = z / sin_theta*theta;
	}
	else
	{
		q.x = x;
		q.y = y;
		q.z = z;
	}
		
	return q;
}

Quaternion Quaternion::Double(Quaternion a, Quaternion b)
{
	Quaternion r = 2*(a.DotProduct(b)*b)-a;
	r.Normalize();
	return r;
}

Quaternion Quaternion::Bisect(Quaternion a, Quaternion b)
{
	return (a+b).Normalize();
}

double Quaternion::DotProduct(const Quaternion& q2) const
{
	return (x * q2.x) + (y * q2.y) + (z * q2.z) + (w * q2.w);
}

Quaternion Quaternion::Inverse() const
{
    double norm = Norm();
    if(norm > 0.0f)
    {
        double invNorm = 1.0f/norm;
        return Quaternion(-x*invNorm,-y*invNorm,-z*invNorm, w*invNorm);
    }
    else
        return Quaternion();
}

Quaternion Quaternion::Conjugate() const
{
	return Quaternion(-x, -y, -z, w);
}

Quaternion& Quaternion::operator =(const Quaternion& rhs)
{
	x = rhs.x;
	y = rhs.y;
	z = rhs.z;
	w = rhs.w;

	return *this;
}

Quaternion Quaternion::operator +(const Quaternion& rhs) const
{
	return Quaternion(rhs.x + x, rhs.y + y, rhs.z + z, rhs.w + w);
}

Quaternion Quaternion::operator -(const Quaternion& rhs) const
{
	return Quaternion(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
}

inline Quaternion Quaternion::operator *(double s) const
{
	return Quaternion(s*x, s*y, s*z, s*w);
}

Quaternion Quaternion::operator *(const Quaternion& rhs) const
{
	double rx, ry, rz, rw;
	double rhsX = rhs.x, rhsY = rhs.y, rhsZ = rhs.z, rhsW = rhs.w;

	rw = rhsW*w - rhsX*x - rhsY*y - rhsZ*z;

	rx = w*rhsX + rhsW*x + y*rhsZ - z*rhsY;
	ry = w*rhsY + rhsW*y + z*rhsX - x*rhsZ;
	rz = w*rhsZ + rhsW*z + x*rhsY - y*rhsX;

	return(Quaternion(rx, ry, rz, rw));
}

inline Quaternion& Quaternion::operator *=(double s)
{
	x *= s;
	y *= s;
	z *= s;
	w *= s;
	return *this;
}

inline Quaternion Quaternion::operator /(double s) const
{
	return Quaternion(x/s, y/s, z/s, w/s);
}

Quaternion Quaternion::Slerp(Quaternion q1, Quaternion q2, double time)
{
	Quaternion r, tmp;
	double angle = q1.DotProduct(q2);
	double scale, invScale;

	if (angle < 0.0f)
	{
		q1 *= -1.0f;
		angle *= -1.0f;
	}
	
	if ((angle + 1.0f) > 0.05f)
    {
        if ((1.0f - angle) >= 0.05f) // spherical interpolation
        {
             const double theta = acos(angle);
             const double invSinTheta = 1.0/sinf(theta);
             scale = sin(theta * (1.0f-time)) * invSinTheta;
             invScale = sin(theta * time) * invSinTheta;
        }
        else // linear interploation
        {
             scale = 1.0f - time;
             invScale = time;
        }
    }
    else
    {
		tmp.x = q1.y * (-1);
		tmp.y = q1.x;
		tmp.z = q1.w * (-1);
		tmp.w = q1.z;
        scale = sin(PI * (0.5f - time));
        invScale = sin(PI * time);
    }

	r = q1*scale + q2*invScale;

	return r;
}

Quaternion Quaternion::Slerp1(Quaternion q1, Quaternion q2, double time)
{
	double angle = q1.DotProduct(q2);
        if(angle < 0.0f)
        {
                q2 = -q2;
                angle *= -1.0f;
        }
        double scale;
        double invscale;

        if((1.0f - angle) >= 0.00000001f)  // spherical interpolation
        {
                double theta = acos(angle);
                double invsintheta = 1.0f / sin(theta);
                scale = sin(theta * (1.0f - time)) * invsintheta;
                invscale = sin(theta * time) * invsintheta;
        }
        else // linear interploation
        {
                scale = 1.0f - time;
                invscale = time;
        }


        Quaternion inter = (q1 * scale) + (q2 * invscale);
        inter.Normalize();

        return inter;
}

Quaternion operator* (double s, Quaternion const& b)
{
    return Quaternion(s*b.x, s*b.y, s*b.z, s*b.w);
}

Quaternion operator- (Quaternion const& a)
{
    return Quaternion(-a.x, -a.y, -a.z, -a.w);
}