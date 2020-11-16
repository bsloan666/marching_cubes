//        mesh.h
//        Blake G. Sloan
//
#ifndef __MCB_MESH_H__
#define __MCB_MESH_H__

#include "polygon.h"
#include <vector>
#include <set>
#include "lat3d.h" 
#include "bgs_exp_hashmap.h"
#include "vec3d.h"

namespace mcb 
{
 
class mesh {
    
    public:
        
    ExpandableHashMap<g3d::vector, unsigned int> vertex_map;
    std::vector<vertex *> vertices;
    std::vector<polygon *> polygons;    
           
    mesh():vertex_map() {}
    void add_polygon( vertex* a, vertex* b, vertex* c );
    vertex *add_vertex( vertex* a, polygon* cp );

    mesh *generate_normals(void);

    void write_dxf(const char *fname);
    void write_obj(const char *fname, float scale);
};

}


#endif
