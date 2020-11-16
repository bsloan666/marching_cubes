//        marching.h
//        Blake G. Sloan
#ifndef __MCB_MARCHING_H__
#define __MCB_MARCHING_H__

#include "mesh.h"
#include "lat3d.h"
#include <vector>

namespace mcb 
{
class Cube {
    public:
    
    vertex corners[8];
    vertex edge_centers[12];
    float cube_resol;
    unsigned char corner_code;
    
    Cube() {};
    
    void get_cube(float (*evaluate)(g3d::vector),   
        float x,float y,float z);

    void assign_midpoints();
    bool test_cube();

    float interp( int idx1, int idx2);
};

void marching_cubes( float (*evaluate)(g3d::vector), float dim, unsigned int resol,  mesh *aForm);

}        
#endif
