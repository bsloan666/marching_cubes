//        mcb_Vect.h     
//           Blake G. Sloan
#ifndef __MCB_VERTEX_H__
#define __MCB_VERTEX_H__
#include <math.h>
#include <vector>
#include "vec3d.h"

namespace mcb 
{
class polygon;

class vertex{
    public:
    vertex () { scalar = 0.f; }    
    g3d::vector coords;
    g3d::vector normal;
    unsigned long ordinal;
    bool set_ordinal(unsigned int o);
    float scalar;
    std::vector<mcb::polygon*> neighbors; 
};
    
    g3d::vector normal(const g3d::vector &a,   
        const g3d::vector &b,
        const g3d::vector &c);
}
#endif
