
// Blake G. Sloan
// Oh dear god. Not again!

#include "vec3d.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#define MCB_EPSILON 0.000001

namespace g3d 
{
	// ctors
	vector::vector( float x_, float y_, float z_ ):_x(x_), _y(y_), _z(z_){}
	vector::vector( const vector &v ):_x(v.x()), _y(v.y()), _z(v.z()){}
	vector::vector( void ):_x(0.f), _y(0.f), _z(0.f){}

	// dtor
	vector::~vector(){}

	// methods
	float vector::x() const { return _x; }
	float vector::y() const { return _y; }
	float vector::z() const { return _z; }

	float &vector::x()  { return _x; }
	float &vector::y()  { return _y; }
	float &vector::z()  { return _z; }


	std::ostream& operator<<( std::ostream  &os , const vector &v)
	{
		os << v._x << " " << v._y << " " << v._z;
	}
	std::istream& operator>>( std::istream  &is,  vector &v)
	{
		is >> v._x;
		is >> v._y;
		is >> v._z;
	}

	 void vector::operator+=( const vector &v )
	{
	  _x+=v.x();
	  _y+=v.y();
	  _z+=v.z();
	}

	void vector::operator-=( const vector &v )
	{
	  _x-=v.x();
	  _y-=v.y();
	  _z-=v.z();
	}

	void vector::operator*=( const float f )
	{
	  _x*=f;
	  _y*=f;
	  _z*=f;
	}

	void vector::operator/=( const float f )
	{
	  if(f != 0.f)
	  {
	    _x/=f;
	    _y/=f;
	    _z/=f;
	  }
	}
	void vector::damp(float d)
	{
		if(this->magnitude() < d)
		{
			_x = 0.f;
			_y = 0.f;
			_z = 0.f;
		}	
	}
	float vector::magnitude() const
	{
	  return sqrt( _x * _x + _y * _y + _z * _z ); 
	}

	void vector::normalize()
	{
	  float m = this->magnitude();
	  (*this)/=m;
	}
	
	// static methods


	float vector::dot( const vector &a, const vector &b )
	{
	  return a.x() * b.x() + a.y() * b.y() + a.z() * b.z();
	}
	
	vector vector::cross( const vector &a, const vector &b )
	{
	  return vector::vector ( a.y() * b.z() - a.z() * b.y(), 
					  a.z() * b.x() - a.x() * b.z(), 
					  a.x() * b.y() - a.y() * b.x());
	}

  	bool operator==( const vector &a, const vector &b )
	{
		return distance(a,b) < MCB_EPSILON;	
	}
	vector operator+( const vector &a, const vector &b )
	{
	  return vector( a.x() + b.x(), a.y() + b.y(), a.z() + b.z());
	}

	vector operator-( const vector &a, const vector &b )
	{
	  return vector( a.x() - b.x(), a.y() - b.y(), a.z() - b.z());
	}

	vector operator*( const vector &a, const float f )
	{
	  return vector( a.x() * f, a.y() * f, a.z() * f);
	}

	vector operator*( const float f,  const vector &a )
	{
	  return vector( a.x() * f, a.y() * f, a.z() * f);
	}

	vector operator/( const float f,  const vector &a )
	{
	  vector v(a);
	  if(f != 0.f)
	  {
		v/=f;
	  }
	  return v; 
	}
	float distance(const vector &v1, const vector &v2)
	{
		vector v = v1 - v2;
		return v.magnitude();	
	}
}		// namespace g3d

