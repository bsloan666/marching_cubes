//        mcb_Mesh.cp
//        Blake G. Sloan

//        methods for manipulating meshes of
//        verteces and polygons

#include "mesh.h"
#include <stdio.h>
#include <vector>
#include "lat3d.h"
 
/***************************/


void mcb::mesh::write_obj(const char * fname, float scale){
    FILE *fp = fopen( fname,"w");
    int i;
    fprintf(fp,"g 1\n"); 
    for(i = 0; i < vertices.size() ; i++){

        fprintf(fp,"v"); 
        fprintf(fp," %3.6f",vertices[i]->coords.x() * scale); 
                fprintf(fp," %3.6f",vertices[i]->coords.y() * scale);  
                fprintf(fp," %3.6f\n",vertices[i]->coords.z() * scale);
    }
    for(i = 0; i < vertices.size(); i++){

        fprintf(fp,"vn"); 
        fprintf(fp," %3.6f",vertices[i]->normal.x()); 
                fprintf(fp," %3.6f",vertices[i]->normal.y());  
                fprintf(fp," %3.6f\n",vertices[i]->normal.z()); 
    }
    for(i=0;i<polygons.size();i++){
        fprintf(fp,"f");
        fprintf(fp," %d/%d/%d %d/%d/%d %d/%d/%d\n",
            polygons[i]->vertices[0]->ordinal+1, 
            polygons[i]->vertices[0]->ordinal+1, 
            polygons[i]->vertices[0]->ordinal+1,
            polygons[i]->vertices[1]->ordinal+1,  
            polygons[i]->vertices[1]->ordinal+1,  
            polygons[i]->vertices[1]->ordinal+1,  
            polygons[i]->vertices[2]->ordinal+1, 
            polygons[i]->vertices[2]->ordinal+1,
            polygons[i]->vertices[2]->ordinal+1
        );  
    }     
    fclose(fp); 
    fprintf(stderr, "%d verts, %d polys\n", vertices.size(), polygons.size()); 
    fprintf(stderr, "%d buckets\n", vertex_map.nbuckets()); 
}

static unsigned int found = 0;
static unsigned int created = 0;

mcb::vertex * mcb::mesh::add_vertex( vertex* a, polygon *currPoly)
{
    const unsigned int *index_ptr = vertex_map.find(a->coords);
    unsigned int index;
    if(index_ptr == nullptr){
        index = vertex_map.size();
        vertex_map.associate(a->coords, index);
        a->set_ordinal(index);
        a->neighbors.push_back(currPoly);
        vertices.push_back(a);
        return a;
    } else {
        index = *index_ptr;
        vertices[index]->neighbors.push_back(currPoly);
        return vertices[index];
    }
}

void mcb::mesh::add_polygon(mcb::vertex* a,mcb::vertex* b,mcb::vertex* c)
{
    mcb::polygon *pPlg = new mcb::polygon;
    pPlg->vertices[0] = add_vertex(c, pPlg);                        
    pPlg->vertices[1] = add_vertex(b, pPlg);                        
    pPlg->vertices[2] = add_vertex(a, pPlg);                    
    polygons.push_back(pPlg);    
}
//////////////////////////////////////////////////////////////////////////

/***************************************/
//        GenerateVertexNormals
//        Adding a step: do a reparameterization to 
//        get rid of skinny polys...
//        Calculates Vertex normals as a normalized
//        sum of a vertex's surrounding polygons
/***************************************/
mcb::mesh *mcb::mesh::generate_normals(void)
{
    int i,j;
    g3d::vector a;

    int x1,x2;

    for(i=0;i<polygons.size();i++)
    {
        polygons[i]->generate_normal();
    }

    int ii = 0;    
    g3d::vector zero(0,0,0);
    
    for(i=0;i<vertices.size();i++)
    {
        a.x() =  0;
        a.y() =  0;
        a.z() =  0;
        for(j=0;j<vertices[i]->neighbors.size();j++)
        {
            a +=  vertices[i]->neighbors[j]->normal;
        }
        ii+=j;
        a.normalize();
        vertices[i]->normal = a * -1.f;
            g3d::vector rad = zero - vertices[i]->coords;
    } 
// DEBUG!!!
    fprintf(stderr, "mean neighbor count = %f\n", 
        (float)ii/vertices.size() );
    fprintf(stderr, "Found %d repeat vertices. Created %d vertices\n", 
        found , created);
    fprintf(stderr, "%d vertices in vertex_map,  %d vertices in list.\n", 
        vertex_map.size() , vertices.size());
    return this;
}
