#ifndef LAT3D_H_INCLUDED
#define LAT3D_H_INCLUDED

#include <math.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>

namespace g3d
{

template <class T>
class lattice
{
	public:
	lattice(unsigned int w, unsigned int h, unsigned int d)
	{
		_width = w;
		_height = h;
		_depth = d;
		array = new T [w*h*d];
	}
	lattice(const char *fname)
	{
	    	FILE *fp = fopen(fname, "r");
	    	int i,j,k,isvect = 0;
	    	char b[1023];
	    	if(fp)
	    	{	
	  		fgets(b, 1024, fp);
	  		fgets(b, 1024, fp);
			int vect = 0;
			if(strstr(b, "SoVectorField") != NULL)
			{
				isvect=1;
//				fprintf(stderr, "reading vectors\n");
			}
	  		fgets(b, 1024, fp);
	  		if( fscanf(fp, "%s %d %d %d\n",b, &_width, &_height, &_depth) != 4)
			{
//				fprintf(stderr, "Error: unable to read .iv file!\n");
				exit (1);
			}
	  		fgets(b, 1024, fp);
	  		fgets(b, 1024, fp);
			float xx, yy, zz;
//			fprintf(stderr, "wid = %d, hei = %d, dep = %d\n", _width, _height, _depth);
			array = new T [_width*_height*_depth];
	  		for( i = 0; i < _depth; i++)
	    		{ 
	  			for( j = 0; j < _height; j++)
	    			{ 
	  				for( k = 0; k < _width; k++)
	    				{
						int nc;
						if(isvect)
						{
							nc = fscanf(fp, "%f %f %f\n", &xx, &yy, &zz);
							*(array + (_width * _height * i) + (_width * j) + k) = 
									g3d::vector(xx,yy,zz);
						}
						else
						{
							T vv;
							nc = fscanf(fp, "%f\n", &vv);
							*(array + (_width * _height * i) + (_width * j) + k) = vv;
						}
	    				} 
	    			} 
	    		}
			fclose(fp);
	    	} 
	}
	~lattice()
	{
		delete [] array;
	}
	unsigned int width() const {return _width;}
	unsigned int height() const {return _height;}
	unsigned int depth() const {return _depth;}
	unsigned int size() const {return _depth * _width * _height;}

	void copy( lattice<T> &tgt)
	{
		unsigned int sz = _width * _height * _depth;	
		unsigned int osz = tgt.width() * tgt.height() * tgt.depth();

		if(sz == osz)
		{
			int i = 0;
			while(i<sz)
			{
				*(tgt.array + i) = *(array + i);
				i++;
			}
		}	
	}
	void interpolate( const lattice<T> &a, const lattice<T> &b, const float w)
	{
		unsigned int sz = _width * _height * _depth;	
		const T* A = a.data(); 
		const T* B = b.data(); 
		int j = 0;
		float i = 1.0 - w;
		while(j<sz)
		{
			*(array + j) = *(A + j) * i + *(B + j) * w;
			j++;
		}
	}
	void fill( const T val)
	{
		unsigned int sz = _width * _height * _depth;	

		int i = 0;
		while(i<sz)
		{
			*(array + i) = val;
			i++;
		}
	}

	void sum(T &s)
	{
	  int x, y, z;
	  for (x = 0; x < _width; x++)
	  {
		for (y = 0; y < _height; y++)
		{
		  for (z = 0; z < _depth; z++)
		  {
			s+= (*this)(x,y,z); 
		  }
		}	
	  }
	}
		
	void normalize()
	{
	  T tot;
	  sum(tot);
          tot /= size();
	  int x, y, z;
	  for (x = 0; x < _width; x++)
	  {
		for (y = 0; y < _height; y++)
		{
		  for (z = 0; z < _depth; z++)
		  {
			(*this)(x,y,z) /=tot; 
		  }
		}	
	  }
	}
	void write( const char *fname, bool isvector=false) const
	{
	  int x, y, z;
	  std::ofstream os(fname);
          unsigned int typ = isvector?1:0;			 
 
	  const char *teetype[3] = {"Scalar", "Vector", "" };
	  
	  os << "#Inventor V2.1 ascii" << "\n";
	  os << "So" << teetype[typ] << "Field {";
	  os << "\n";
	  os << "parameters" << "\n";
	  os << "grid_size " << _width << " " << _height << " " << _depth << "\n"; 
	  os << "global_transform 1 0 0 0 0 1 0 0 0 0 1 0 0 0 0 1" << "\n";
	  os << "data" << "\n";
	  for (x = 0; x < _width; x++)
	  {
		for (y = 0; y < _height; y++)
		{
		  for (z = 0; z < _depth; z++)
		  {
			os << (*this)(x,y,z);
			os  << "\n"; 
		  }
		}	
	  }
	  os << "}" << "\n";
	   
		os.close();
	}	
	const unsigned int offset(unsigned int x, unsigned int y, unsigned int z) const
	{
		return  (_width * _height * z) + (_width * y) + x; 
	}
  	
	const T operator() (unsigned int o) const 
	{
		return *(array + o); 
	}

	T &operator() (unsigned int o) 
	{
		return *(array + o); 
	}

	const T operator() (unsigned int x, unsigned int y, unsigned int z) const 
	{
		return *(array + (_width * _height * z) + (_width * y) + x); 
	}

	T &operator() (unsigned int x, unsigned int y, unsigned int z) 
	{
		return *(array + (_width * _height * z) + (_width * y) + x); 
	}
	void subsample( g3d::vector &pos, const float val)
	{
		
		float fx = pos.x();
		float fy = pos.y();
		float fz = pos.z();

	  int x = (int)fx;
	  int y = (int)fy; 
	  int z = (int)fz;

	  if(x < 1 || x > (_width-2) || y < 1 || y > (_height-2)  ||  z < 1  || z > (_depth-2))
	  {
		return;
	  } 	
	  float rx = fx - x; 
	  float ry = fy - y; 
	  float rz = fz - z;
	 
		T ret; 
	  float weights[8];
	  vector r(rx,ry,rz);
	  float weight_acc(0.f);	
	  int i,j,k, f = 0;
		float weight;

//	(*this)(x,y,z) +=  val;
//	return;
		int is,ie,js,je,ks,ke;
		is = 0;
		ie = 2;	
		js = 0;
		je = 2;	
		ks = 0;
		ke = 2;
		for (i = is; i < ie; i++)
		{	
			for (j = js; j < je; j++)
			{	
				for (k = ks; k < ke; k++)
				{
					vector n((float)i,(float)j,(float)k);
					float  weight = 1.f/(distance(r,n)+0.0000001f);
					weights[f] = weight;		
					weight_acc += weight;			
					f++;	
				}
			}
		}
	
		for (i = 0;i<8;i++)
		{
			weights[i] /= weight_acc;
		}
		f = 0;
	
		for(i = is; i < ie; i++)
		{	
			for (j = js; j < je; j++)
			{	
				for (k = ks; k < ke; k++)
				{
					(*this)(x+i,y+j,z+k)  +=  weights[f] * val;
					f++; 
				}
			}
		}
	}
	const T continuous(const vector &pos) const
	{
		float fx = pos.x();
		float fy = pos.y();
		float fz = pos.z();

	  int x = (int)fx;
	  int y = (int)fy; 
	  int z = (int)fz;

	  if(x < 0 || x > (_width-2) || y < 0 || y > (_height-2)  ||  z < 0  || z > (_depth-2))
	  {
		return vector(500.f,500.f,500.f);
	  } 	
	  float rx = fx - x; 
	  float ry = fy - y; 
	  float rz = fz - z;
	 
		T ret; 
	  float weights[8];
	  vector r(rx,ry,rz);
	  float weight_acc(0.f);	
	  int i,j,k, f = 0;
		float weight;

//	(*this)(x,y,z) +=  val;
//	return;
		int is,ie,js,je,ks,ke;
		is = 0;
		ie = 2;	
		js = 0;
		je = 2;	
		ks = 0;
		ke = 2;
		for (i = is; i < ie; i++)
		{	
			for (j = js; j < je; j++)
			{	
				for (k = ks; k < ke; k++)
				{
					vector n((float)i,(float)j,(float)k);
					float  weight = 1.f/(distance(r,n)+0.0000001f);
					weights[f] = weight;		
					weight_acc += weight;			
					f++;	
				}
			}
		}
	
		for (i = 0;i<8;i++)
		{
			weights[i] /= weight_acc;
		}
		f = 0;
	
		for(i = is; i < ie; i++)
		{	
			for (j = js; j < je; j++)
			{	
				for (k = ks; k < ke; k++)
				{
					ret += (*this)(x+i,y+j,z+k) * weights[f];
					f++; 
				}
			}
		}
		return ret;

	  }
	const T* data() const { return array;}
 
	private:
	unsigned int _width;
	unsigned int _height;
	unsigned int _depth;
	T *array;
	
};

} // namespace g3d
#endif
