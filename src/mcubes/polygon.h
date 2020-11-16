//        mcb_Poly.h
//        Blake G. Sloan 
#ifndef __MCB_POLYGON_H__
#define __MCB_POLYGON_H__

#include "vertex.h"

namespace mcb {

class polygon {
    public:
    
    vertex *vertices[3];
    g3d::vector normal;
    polygon() {}

    void generate_normal();
    g3d::vector centroid();    
    float area();    
    float extent();    
    int is_degenerate(int &ix1, int &ix2);
};
}
#endif                
            
