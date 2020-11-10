//		mcb_Vect.cp
//		Blake G. Sloan 
 
//		many thanks to Rex E. Bradford for this and other
//		low-level Classes

#include "vertex.h"

bool mcb::vertex::set_ordinal(unsigned int o)
{
	ordinal = o;
	return true;
}


//
// Normal() computes a vector perpendicular to a plane, where that
// plane is defined by three points residing in the plane.  The
// direction of the normal vector is outwards from the convex angle
// defined by a->b->c, where the connection moves in a counter-clockwise
// direction.  The resulting normal vector is normalized before being
// returned.
//
//    a          = 1st vector, in counter-clockwise order
//    b          = 2nd vector, in counter-clockwise order
//    c          = 3rd vector, in counter-clockwise order
//
// Returns:  normal vector to the plane formed by a,b,c
// Note: Handle degeneracy. Somehow.

g3d::vector mcb::normal(  	const g3d::vector &a,   
			const g3d::vector &b,
                      	const g3d::vector &c)
{
   g3d::vector norm = g3d::vector::cross( b-a, b-c);
   norm.normalize();
   return norm;
}
//
//
