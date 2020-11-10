//		mcb_Poly.cpp
//		Blake G. Sloan


#include "polygon.h"
#define true 1
#define false 0


g3d::vector mcb::polygon::centroid()
{
	
	g3d::vector accum = this->vertices[0]->coords + 
			   this->vertices[1]->coords + 
			   this->vertices[2]->coords;
	accum/=3;
	return accum;
}	

void mcb::polygon::generate_normal()
{ 
                this->normal = mcb::normal(  vertices[0]->coords,
                                        vertices[1]->coords,
                                        vertices[2]->coords);
}

float mcb::polygon::area()
{
	g3d::vector vlen1,vlen2,vlen3;
	
	vlen1 = this->vertices[0]->coords - this->vertices[1]->coords;
	vlen2 = this->vertices[1]->coords - this->vertices[2]->coords;
	vlen3 = this->vertices[2]->coords - this->vertices[0]->coords;
	
	float len1 = vlen1.magnitude();
	float len2 = vlen2.magnitude();
	float len3 = vlen3.magnitude();
	
}
float mcb::polygon::extent()
{
	g3d::vector vlen1,vlen2,vlen3;
	
	vlen1 = this->vertices[0]->coords - this->vertices[1]->coords;
	vlen2 = this->vertices[1]->coords - this->vertices[2]->coords;
	vlen3 = this->vertices[2]->coords - this->vertices[0]->coords;
	
	float len1 = vlen1.magnitude();
	float len2 = vlen2.magnitude();
	float len3 = vlen3.magnitude();

	float gr = len1>len2?len1:len2;
	return gr>len3?gr:len3;

}
/*****************************************/

int mcb::polygon::is_degenerate(int &indx1, int & indx2)
{
	g3d::vector vlen1,vlen2,vlen3;
	
	vlen1 = this->vertices[0]->coords - this->vertices[1]->coords;
	vlen2 = this->vertices[1]->coords - this->vertices[2]->coords;
	vlen3 = this->vertices[2]->coords - this->vertices[0]->coords;
	
	float a = vlen1.magnitude();
	float b = vlen2.magnitude();
	float c = vlen3.magnitude();

	float gr = a>b?a:b;
	float le = a<b?a:b;
	float extent =  gr>c?gr:c;
	float shorty =  le<c?le:c;
	float s = (a+b+c)/2;
	float area = sqrt (s * (s-a) * (s-b) * (s-c));

	if(shorty == a)
	{
		indx1=0;
		indx2=1;
	}
	else if(shorty == b)
	{
		indx1=1;
		indx2=2;
	}
	else
	{
		indx1=2;
		indx2=0;
	}
	return (area/extent) < 0.004; 
}
