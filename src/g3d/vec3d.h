#ifndef __VEC3D_H_INCLUDED__
#define __VEC3D_H_INCLUDED__

// Blake G. Sloan
// Oh dear god. Not again!

#include <iostream>

namespace g3d 
{
  class vector
  {
	public:
	// ctors
	vector( float x_, float y_, float z_ );
	vector( const vector &v );
	vector( void );

	// dtor
	~vector();

	// methods
	float x() const;
	float y() const;
	float z() const;

	float &x(); 
	float &y();
	float &z();

	friend std::ostream& operator<<( std::ostream  &os , const vector &v);
	friend std::istream& operator>>( std::istream  &is,  vector &v);

	 void operator+=( const vector &v );

	void operator-=( const vector &v );

	void operator*=( const float f );

	void operator/=( const float f );
	void damp(float d);
	float magnitude() const;

	friend float distance(const vector &v1, const vector &v2);
	void normalize();
	
	// static methods


	static float dot( const vector &a, const vector &b );
	
	static vector cross( const vector &a, const vector &b );

	private:
	float _x;
	float _y;
	float _z;

  };	//class vector
  vector operator+( const vector &a, const vector &b );
  bool operator==( const vector &a, const vector &b );

  vector operator-( const vector &a, const vector &b );

  vector operator*( const vector &a, const float f );

  vector operator*( const float f,  const vector &a );

  vector operator/( const float f,  const vector &a );
  float distance(const vector &v1, const vector &v2);
}		// namespace g3d

#endif
