//        marching.cpp
//        Blake G. Sloan
/*
    Corners
    4 ------ 7
    |\       |\
    | \      | \
    |  5 ----|--6
    0 -|---- 3  |
     \ |      \ |
      \|       \|
       1 ------ 2

    Edges
     ----- 7 ----
    |\           |\
    | 4          | 6
    8  \         B  \
    |    ----- 5 -----
    |---|- 3 ----|   |
     \  |         \  |
      0 9          2 A
       \|           \|
        |----- 1 -----
*/
#include "marching.h"
void mcb::Cube::get_cube( float (*evaluate)(g3d::vector),   
                float x,float y,float z)
{
    float  dim = this->cube_resol;
    corners[0].coords = g3d::vector(x, y + dim, z); 
    corners[1].coords = g3d::vector(x, y + dim, z + dim); 
    corners[2].coords = g3d::vector(x + dim, y + dim, z + dim); 
    corners[3].coords = g3d::vector(x + dim, y + dim, z); 
    corners[4].coords = g3d::vector(x, y, z); 
    corners[5].coords = g3d::vector(x, y, z + dim); 
    corners[6].coords = g3d::vector(x + dim, y, z + dim); 
    corners[7].coords = g3d::vector(x + dim, y, z);

    for(size_t i = 0; i < 8; i++){
        corners[i].scalar = evaluate( corners[i].coords);
    }

    edge_centers[0].coords.x() = x;
    edge_centers[0].coords.y() = y + cube_resol;
    edge_centers[0].coords.z() = z + cube_resol * interp(0, 1);
    
    edge_centers[1].coords.x() = x + cube_resol * interp(1, 2);
    edge_centers[1].coords.y() = y + cube_resol;
    edge_centers[1].coords.z() = z + cube_resol;
    
    edge_centers[2].coords.x() = x + cube_resol;
    edge_centers[2].coords.y() = y + cube_resol;
    edge_centers[2].coords.z() = z + cube_resol * interp(3, 2);
    
    edge_centers[3].coords.x() = x + cube_resol * interp(0, 3);
    edge_centers[3].coords.y() = y + cube_resol;
    edge_centers[3].coords.z() = z;
    
    edge_centers[4].coords.x() = x;
    edge_centers[4].coords.y() = y;
    edge_centers[4].coords.z() = z + cube_resol * interp(4, 5);
    
    edge_centers[5].coords.x() = x + cube_resol * interp(5, 6);
    edge_centers[5].coords.y() = y;
    edge_centers[5].coords.z() = z + cube_resol;
    
    edge_centers[6].coords.x() = x + cube_resol;
    edge_centers[6].coords.y() = y;
    edge_centers[6].coords.z() = z + cube_resol * interp(7, 6);
    
    edge_centers[7].coords.x() = x + cube_resol * interp(4, 7);
    edge_centers[7].coords.y() = y;
    edge_centers[7].coords.z() = z;
    
    edge_centers[8].coords.x() = x;
    edge_centers[8].coords.y() = y + cube_resol * interp(4, 0);
    edge_centers[8].coords.z() = z;
    
    edge_centers[9].coords.x() = x;
    edge_centers[9].coords.y() = y + cube_resol * interp(5, 1);
    edge_centers[9].coords.z() = z + cube_resol;
    
    edge_centers[0xa].coords.x() = x + cube_resol;
    edge_centers[0xa].coords.y() = y + cube_resol * interp(6, 2);
    edge_centers[0xa].coords.z() = z + cube_resol;
    
    edge_centers[0xb].coords.x() = x + cube_resol;
    edge_centers[0xb].coords.y() = y + cube_resol * interp(7, 3);
    edge_centers[0xb].coords.z() = z;
}

float mcb::Cube::interp( int idx1, int idx2)
{
    float v1,v2;
    v1 = this->corners[idx1].scalar;
    v2 = this->corners[idx2].scalar;
    float norm = v2-v1;
    if(norm != 0.f) 
    {
        return -v1/norm;
    }
    else
    {
        return 0.0;
    } 
}

bool  mcb::Cube::test_cube()
{
    int i;
    corner_code = 0;
    for(i = 0;i<8;i++)
    {
        if(corners[i].scalar > 0.f)
        { 
            corner_code = corner_code + (1<<i);
        }
    }
    return corner_code != 0 && corner_code != 255;    
} 

void mcb::marching_cubes( float (*evaluate)(g3d::vector), 
    float dim, unsigned int resol,  mcb::mesh * aForm)
{
    int x,y,z;
    int hdim =(int)( dim/2.0);

    fprintf(stderr,  "resol = %d\n", resol);
    fprintf(stderr,  "dim = %4.4f\n", dim);

    printf( "Entering Marching Cubes\n");

    float cube_resol = dim/resol;

    int sc = 0;
    int slice_count = (int)( resol) ;

    int incr =(int) cube_resol;
    
    printf( "sampled volume is %g x %g x %g\n", dim,dim,dim);
    printf( "each cube is %g x %g x %g\n", cube_resol, cube_resol, cube_resol);
    printf( "Sampling %d grid slices...\n", resol);

    int count = 0;
    for(y=-hdim;y<hdim;y+=incr)
    {
        for(z=-hdim;z<hdim;z+=incr)
        {
            for(x=-hdim;x<hdim;x+=incr)
            {
                // printf("So far, so good %d\n", count++);
                mcb::Cube *pcube = new mcb::Cube();
                pcube->cube_resol = cube_resol;
                pcube->get_cube(evaluate, x,y,z);
                if(pcube->test_cube())
                {
                    switch(pcube->corner_code)
                    {

                        case 0x0: 
                        break;
                        
                        case 0xff: 
                        break;
                        
                        case 0x1:/**/ 
                        aForm->add_polygon(&pcube->edge_centers[0],&pcube->edge_centers[8],&pcube->edge_centers[3]);
                        break;
                        
                        case 0x2:/**/
                        aForm->add_polygon(&pcube->edge_centers[1],&pcube->edge_centers[9],&pcube->edge_centers[0]);
                        break;
                        
                        case 0x3:/**/
                        aForm->add_polygon(&pcube->edge_centers[1],&pcube->edge_centers[9],&pcube->edge_centers[8]);
                        aForm->add_polygon(&pcube->edge_centers[1],&pcube->edge_centers[8],&pcube->edge_centers[3]);
                        break;
                        
                        case 0x4:/**/
                        aForm->add_polygon(&pcube->edge_centers[0xa],&pcube->edge_centers[1],&pcube->edge_centers[2]);
                        break;
                        
                        //case 0x5: /**/
                        //aForm->add_polygon(&pcube->edge_centers[0xa],&pcube->edge_centers[1],&pcube->edge_centers[2]);
                        //aForm->add_polygon(&pcube->edge_centers[0],&pcube->edge_centers[8],&pcube->edge_centers[3]);
                        //break;
                        
                        case 0x5: /**/
                        aForm->add_polygon(&pcube->edge_centers[8],&pcube->edge_centers[0xa],&pcube->edge_centers[1]);
                        aForm->add_polygon(&pcube->edge_centers[8],&pcube->edge_centers[1],&pcube->edge_centers[0]);
                        aForm->add_polygon(&pcube->edge_centers[0xa],&pcube->edge_centers[8],&pcube->edge_centers[3]);
                        aForm->add_polygon(&pcube->edge_centers[0xa],&pcube->edge_centers[3],&pcube->edge_centers[2]);
                        break;
                        
                        case 0x6:/**/
                        aForm->add_polygon(&pcube->edge_centers[0xa],&pcube->edge_centers[9],&pcube->edge_centers[0]);
                        aForm->add_polygon(&pcube->edge_centers[0xa],&pcube->edge_centers[0],&pcube->edge_centers[2]);
                        break;
                        
                        case 0x7: /**/
                        aForm->add_polygon(&pcube->edge_centers[9],&pcube->edge_centers[8],&pcube->edge_centers[0xa]);
                        aForm->add_polygon(&pcube->edge_centers[2],&pcube->edge_centers[0xa],&pcube->edge_centers[8]);
                        aForm->add_polygon(&pcube->edge_centers[2],&pcube->edge_centers[8],&pcube->edge_centers[3]);
                        break;
                        
                        case 0x8:/**/
                        aForm->add_polygon(&pcube->edge_centers[0xb],&pcube->edge_centers[2],&pcube->edge_centers[3]);
                        break;
                        
                        case 0x9:/**/
                        aForm->add_polygon(&pcube->edge_centers[8],&pcube->edge_centers[0xb],&pcube->edge_centers[2]);
                        aForm->add_polygon(&pcube->edge_centers[8],&pcube->edge_centers[2],&pcube->edge_centers[0]);
                        break;
                        
                        case 0xa:/**/
                        aForm->add_polygon(&pcube->edge_centers[9],&pcube->edge_centers[0xb],&pcube->edge_centers[2]);
                        aForm->add_polygon(&pcube->edge_centers[9],&pcube->edge_centers[2],&pcube->edge_centers[1]);
                        aForm->add_polygon(&pcube->edge_centers[0xb],&pcube->edge_centers[9],&pcube->edge_centers[0]);
                        aForm->add_polygon(&pcube->edge_centers[0xb],&pcube->edge_centers[0],&pcube->edge_centers[3]);
                        break;
                        
                        //case 0xa:/**/
                        //aForm->add_polygon(&pcube->edge_centers[0xb],&pcube->edge_centers[2],&pcube->edge_centers[3]);
                        //aForm->add_polygon(&pcube->edge_centers[1],&pcube->edge_centers[9],&pcube->edge_centers[0);
                        //break;
                        
                        case 0xb: /**/
                        aForm->add_polygon(&pcube->edge_centers[9],&pcube->edge_centers[8],&pcube->edge_centers[0xb]);
                        aForm->add_polygon(&pcube->edge_centers[9],&pcube->edge_centers[0xb],&pcube->edge_centers[2]);
                        aForm->add_polygon(&pcube->edge_centers[9],&pcube->edge_centers[2],&pcube->edge_centers[1]);
                        break;
                        
                        case 0xc: /**/
                        aForm->add_polygon(&pcube->edge_centers[0xb],&pcube->edge_centers[0xa],&pcube->edge_centers[1]);
                        aForm->add_polygon(&pcube->edge_centers[0xb],&pcube->edge_centers[1],&pcube->edge_centers[3]);
                        break;
                        
                        case 0xd: /**/ 
                        aForm->add_polygon(&pcube->edge_centers[8],&pcube->edge_centers[0xb],&pcube->edge_centers[0xa]);
                        aForm->add_polygon(&pcube->edge_centers[0],&pcube->edge_centers[8],&pcube->edge_centers[0xa]);
                        aForm->add_polygon(&pcube->edge_centers[0],&pcube->edge_centers[0xa],&pcube->edge_centers[1]);
                        break;
                        
                        case 0xe: /**/
                        aForm->add_polygon(&pcube->edge_centers[0xb],&pcube->edge_centers[0xa],&pcube->edge_centers[9]);
                        aForm->add_polygon(&pcube->edge_centers[9],&pcube->edge_centers[0],&pcube->edge_centers[3]);
                        aForm->add_polygon(&pcube->edge_centers[9],&pcube->edge_centers[3],&pcube->edge_centers[0xb]);
                        break;
                        
                        case 0xf:/**/
                        aForm->add_polygon(&pcube->edge_centers[8],&pcube->edge_centers[0xb],&pcube->edge_centers[0xa]);
                        aForm->add_polygon(&pcube->edge_centers[8],&pcube->edge_centers[0xa],&pcube->edge_centers[9]);
                        break;
                        
                        ////////
                         
                        case 0x10:/**/
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[7],&pcube->edge_centers[8]);
                        break;
                        
                        case 0x11: /**/ 
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[7],&pcube->edge_centers[3]);
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[3],&pcube->edge_centers[0]);
                        break;
                        
                        //case 0x12:/**/
                        //aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[7],&pcube->edge_centers[8]);
                        //aForm->add_polygon(&pcube->edge_centers[1],&pcube->edge_centers[9],&pcube->edge_centers[0);
                        //break;
                        
                        case 0x12:/**/
                        aForm->add_polygon(&pcube->edge_centers[9],&pcube->edge_centers[4],&pcube->edge_centers[7]);
                        aForm->add_polygon(&pcube->edge_centers[9],&pcube->edge_centers[7],&pcube->edge_centers[1]);
                        aForm->add_polygon(&pcube->edge_centers[8],&pcube->edge_centers[0],&pcube->edge_centers[1]);
                        aForm->add_polygon(&pcube->edge_centers[8],&pcube->edge_centers[1],&pcube->edge_centers[7]);
                        break;
                        
                        case 0x13:/**/
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[3],&pcube->edge_centers[1]);
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[1],&pcube->edge_centers[9]);
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[9],&pcube->edge_centers[4]);
                        break;
                        
                        case 0x14:/**/
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[7],&pcube->edge_centers[8]);
                        aForm->add_polygon(&pcube->edge_centers[0xa],&pcube->edge_centers[1],&pcube->edge_centers[2]);
                        break;
                        
                        case 0x15:/**/
                        aForm->add_polygon(&pcube->edge_centers[2],&pcube->edge_centers[0xa],&pcube->edge_centers[1]);
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[7],&pcube->edge_centers[3]);
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[3],&pcube->edge_centers[0]);
                        break;
                        
                        case 0x16:/**/
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[7],&pcube->edge_centers[8]);
                        aForm->add_polygon(&pcube->edge_centers[0xa],&pcube->edge_centers[9],&pcube->edge_centers[0]);
                        aForm->add_polygon(&pcube->edge_centers[0xa],&pcube->edge_centers[0],&pcube->edge_centers[2]);
                        break;
                        
                        case 0x17:/**/
                        aForm->add_polygon(&pcube->edge_centers[0xa],&pcube->edge_centers[4],&pcube->edge_centers[3]);
                        aForm->add_polygon(&pcube->edge_centers[0xa],&pcube->edge_centers[3],&pcube->edge_centers[2]);
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[7],&pcube->edge_centers[3]);
                        aForm->add_polygon(&pcube->edge_centers[9],&pcube->edge_centers[4],&pcube->edge_centers[0xa]);
                        break;
                        
                        //case 0x18:/**/
                        //aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[7],&pcube->edge_centers[8]);
                        //aForm->add_polygon(&pcube->edge_centers[0xb],&pcube->edge_centers[2],&pcube->edge_centers[3]);
                        //break;
                        
                        case 0x18:/**/
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[0xb],&pcube->edge_centers[2]);
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[2],&pcube->edge_centers[4]);
                        aForm->add_polygon(&pcube->edge_centers[3],&pcube->edge_centers[8],&pcube->edge_centers[4]);
                        aForm->add_polygon(&pcube->edge_centers[3],&pcube->edge_centers[4],&pcube->edge_centers[2]);
                        break;
                        
                        case 0x19:/**/
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[2],&pcube->edge_centers[0]);
                        aForm->add_polygon(&pcube->edge_centers[2],&pcube->edge_centers[4],&pcube->edge_centers[7]);
                        aForm->add_polygon(&pcube->edge_centers[2],&pcube->edge_centers[7],&pcube->edge_centers[0xb]);
                        break;
                        
                        case 0x1a:/**/
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[7],&pcube->edge_centers[8]);
                        aForm->add_polygon(&pcube->edge_centers[0xb],&pcube->edge_centers[2],&pcube->edge_centers[3]);
                        aForm->add_polygon(&pcube->edge_centers[1],&pcube->edge_centers[9],&pcube->edge_centers[0]);
                        break;
                        
                        case 0x1b:/**/
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[7],&pcube->edge_centers[0xb]);
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[0xb],&pcube->edge_centers[9]);
                        aForm->add_polygon(&pcube->edge_centers[9],&pcube->edge_centers[0xb],&pcube->edge_centers[2]);
                        aForm->add_polygon(&pcube->edge_centers[9],&pcube->edge_centers[2],&pcube->edge_centers[1]);
                        break;
                        
                        case 0x1c:/**/
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[7],&pcube->edge_centers[8]);
                        aForm->add_polygon(&pcube->edge_centers[0xb],&pcube->edge_centers[0xa],&pcube->edge_centers[1]);
                        aForm->add_polygon(&pcube->edge_centers[0xb],&pcube->edge_centers[1],&pcube->edge_centers[3]);
                        break;
                        
                        case 0x1d:/**/
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[0xb],&pcube->edge_centers[1]);
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[7],&pcube->edge_centers[0xb]);
                        aForm->add_polygon(&pcube->edge_centers[0],&pcube->edge_centers[4],&pcube->edge_centers[1]);
                        aForm->add_polygon(&pcube->edge_centers[1],&pcube->edge_centers[0xb],&pcube->edge_centers[0xa]);
                        break;
                        
                        case 0x1e:/**/
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[7],&pcube->edge_centers[8]);
                        aForm->add_polygon(&pcube->edge_centers[0xb],&pcube->edge_centers[0xa],&pcube->edge_centers[9]);
                        aForm->add_polygon(&pcube->edge_centers[9],&pcube->edge_centers[0],&pcube->edge_centers[3]);
                        aForm->add_polygon(&pcube->edge_centers[9],&pcube->edge_centers[3],&pcube->edge_centers[0xb]);
                        break;
                        
                        case 0x1f:/**/
                        aForm->add_polygon(&pcube->edge_centers[9],&pcube->edge_centers[0xb],&pcube->edge_centers[0xa]);
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[7],&pcube->edge_centers[0xb]);
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[0xb],&pcube->edge_centers[9]);
                        break;
                        
                        
                        //////////////
                        
                        case 0x20:/**/
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[4],&pcube->edge_centers[9]);
                        break;
                        
                        //case 0x21:/**/
                        //aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[4],&pcube->edge_centers[9]);
                        //aForm->add_polygon(&pcube->edge_centers[0],&pcube->edge_centers[8],&pcube->edge_centers[3]);
                        //break;
                        
                        case 0x21:/**/
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[8],&pcube->edge_centers[3]);
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[3],&pcube->edge_centers[5]);
                        aForm->add_polygon(&pcube->edge_centers[0],&pcube->edge_centers[9],&pcube->edge_centers[5]);
                        aForm->add_polygon(&pcube->edge_centers[0],&pcube->edge_centers[5],&pcube->edge_centers[3]);
                        break;
                        
                        case 0x22:/**/
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[4],&pcube->edge_centers[0]);
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[0],&pcube->edge_centers[1]);
                        break;
                        
                        case 0x23:/**/
                        aForm->add_polygon(&pcube->edge_centers[1],&pcube->edge_centers[5],&pcube->edge_centers[3]);
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[4],&pcube->edge_centers[8]);
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[8],&pcube->edge_centers[3]);
                        break;
                        
                        //case 0x24:/**/
                        //aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[4],&pcube->edge_centers[9]);
                        //aForm->add_polygon(&pcube->edge_centers[0xa],&pcube->edge_centers[1],&pcube->edge_centers[2]);
                        //break;
                        
                        case 0x24:/**/
                        aForm->add_polygon(&pcube->edge_centers[9],&pcube->edge_centers[1],&pcube->edge_centers[2]);
                        aForm->add_polygon(&pcube->edge_centers[9],&pcube->edge_centers[2],&pcube->edge_centers[4]);
                        aForm->add_polygon(&pcube->edge_centers[0xa],&pcube->edge_centers[5],&pcube->edge_centers[4]);
                        aForm->add_polygon(&pcube->edge_centers[0xa],&pcube->edge_centers[4],&pcube->edge_centers[2]);
                        break;
                        
                        case 0x25:/**/
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[4],&pcube->edge_centers[9]);
                        aForm->add_polygon(&pcube->edge_centers[0],&pcube->edge_centers[8],&pcube->edge_centers[3]);
                        aForm->add_polygon(&pcube->edge_centers[0xa],&pcube->edge_centers[1],&pcube->edge_centers[2]); 
                        break;
                        
                        case 0x26:/**/
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[0],&pcube->edge_centers[2]);
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[4],&pcube->edge_centers[2]);
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[2],&pcube->edge_centers[0xa]);
                        break;
                        
                        case 0x27:/**/
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[4],&pcube->edge_centers[8]);
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[8],&pcube->edge_centers[0xa]);
                        aForm->add_polygon(&pcube->edge_centers[0xa],&pcube->edge_centers[8],&pcube->edge_centers[3]);
                        aForm->add_polygon(&pcube->edge_centers[0xa],&pcube->edge_centers[3],&pcube->edge_centers[2]);
                        break;
                        
                        case 0x28:/**/
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[4],&pcube->edge_centers[9]);
                        aForm->add_polygon(&pcube->edge_centers[0xb],&pcube->edge_centers[2],&pcube->edge_centers[3]);
                        break;
                        
                        case 0x29:/**/
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[4],&pcube->edge_centers[9]);
                        aForm->add_polygon(&pcube->edge_centers[8],&pcube->edge_centers[0xb],&pcube->edge_centers[2]);
                        aForm->add_polygon(&pcube->edge_centers[8],&pcube->edge_centers[2],&pcube->edge_centers[0]);
                        break;
                        
                        case 0x2a:/**/
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[4],&pcube->edge_centers[0]);
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[0],&pcube->edge_centers[1]);
                        aForm->add_polygon(&pcube->edge_centers[0xb],&pcube->edge_centers[2],&pcube->edge_centers[3]);
                        break;
                        
                        case 0x2b:/**/
                        aForm->add_polygon(&pcube->edge_centers[1],&pcube->edge_centers[4],&pcube->edge_centers[0xb]);
                        aForm->add_polygon(&pcube->edge_centers[1],&pcube->edge_centers[5],&pcube->edge_centers[4]);
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[8],&pcube->edge_centers[0xb]);
                        aForm->add_polygon(&pcube->edge_centers[1],&pcube->edge_centers[0xb],&pcube->edge_centers[2]);
                        break;
                        
                        case 0x2c:/**/
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[4],&pcube->edge_centers[9]);
                        aForm->add_polygon(&pcube->edge_centers[0xb],&pcube->edge_centers[0xa],&pcube->edge_centers[1]);
                        aForm->add_polygon(&pcube->edge_centers[0xb],&pcube->edge_centers[1],&pcube->edge_centers[3]);
                        break;
                        
                        case 0x2d:/**/
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[4],&pcube->edge_centers[9]);
                        aForm->add_polygon(&pcube->edge_centers[8],&pcube->edge_centers[0xb],&pcube->edge_centers[0xa]);
                        aForm->add_polygon(&pcube->edge_centers[0],&pcube->edge_centers[8],&pcube->edge_centers[0xa]);
                        aForm->add_polygon(&pcube->edge_centers[0],&pcube->edge_centers[0xa],&pcube->edge_centers[1]);
                        break;
                        
                        case 0x2e:/**/
                        aForm->add_polygon(&pcube->edge_centers[0xa],&pcube->edge_centers[4],&pcube->edge_centers[3]);
                        aForm->add_polygon(&pcube->edge_centers[0xa],&pcube->edge_centers[5],&pcube->edge_centers[4]);
                        aForm->add_polygon(&pcube->edge_centers[3],&pcube->edge_centers[4],&pcube->edge_centers[0]);
                        aForm->add_polygon(&pcube->edge_centers[0xa],&pcube->edge_centers[3],&pcube->edge_centers[0xb]);
                        break;
                        
                        case 0x2f:/**/
                        aForm->add_polygon(&pcube->edge_centers[0xb],&pcube->edge_centers[0xa],&pcube->edge_centers[8]);
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[4],&pcube->edge_centers[8]);
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[8],&pcube->edge_centers[0xa]);
                        break;
                        
                        /////////////
                        
                        case 0x30:/**/
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[7],&pcube->edge_centers[8]);
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[8],&pcube->edge_centers[9]);
                        break;
                        
                        case 0x31: /**/
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[7],&pcube->edge_centers[3]);
                        aForm->add_polygon(&pcube->edge_centers[9],&pcube->edge_centers[5],&pcube->edge_centers[3]);
                        aForm->add_polygon(&pcube->edge_centers[9],&pcube->edge_centers[3],&pcube->edge_centers[0]);
                        break;
                        
                        case 0x32:/**/
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[7],&pcube->edge_centers[1]);
                        aForm->add_polygon(&pcube->edge_centers[1],&pcube->edge_centers[7],&pcube->edge_centers[8]);
                        aForm->add_polygon(&pcube->edge_centers[1],&pcube->edge_centers[8],&pcube->edge_centers[0]);
                        break;
                        
                        case 0x33:/**/
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[7],&pcube->edge_centers[3]);
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[3],&pcube->edge_centers[1]);
                        break;
                        
                        case 0x34:/**/
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[7],&pcube->edge_centers[8]);
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[8],&pcube->edge_centers[9]);
                        aForm->add_polygon(&pcube->edge_centers[0xa],&pcube->edge_centers[1],&pcube->edge_centers[2]);
                        break;
                        
                        case 0x35:/**/
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[7],&pcube->edge_centers[3]);
                        aForm->add_polygon(&pcube->edge_centers[9],&pcube->edge_centers[5],&pcube->edge_centers[3]);
                        aForm->add_polygon(&pcube->edge_centers[9],&pcube->edge_centers[3],&pcube->edge_centers[0]);
                        aForm->add_polygon(&pcube->edge_centers[0xa],&pcube->edge_centers[1],&pcube->edge_centers[2]);
                        break;
                        
                        case 0x36: /**/
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[8],&pcube->edge_centers[0]);
                        aForm->add_polygon(&pcube->edge_centers[0xa],&pcube->edge_centers[0],&pcube->edge_centers[2]);
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[7],&pcube->edge_centers[0xa]);
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[0],&pcube->edge_centers[0xa]);
                        break;
                        
                        case 0x37:/**/
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[7],&pcube->edge_centers[3]);
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[3],&pcube->edge_centers[2]);
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[2],&pcube->edge_centers[0xa]);
                        break;
                        
                        case 0x38:/**/
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[7],&pcube->edge_centers[8]);
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[8],&pcube->edge_centers[9]);
                        aForm->add_polygon(&pcube->edge_centers[0xb],&pcube->edge_centers[2],&pcube->edge_centers[3]);
                        break;
                        
                        case 0x39:/**/
                        aForm->add_polygon(&pcube->edge_centers[0],&pcube->edge_centers[5],&pcube->edge_centers[0xb]);
                        aForm->add_polygon(&pcube->edge_centers[9],&pcube->edge_centers[5],&pcube->edge_centers[0]);
                        aForm->add_polygon(&pcube->edge_centers[0],&pcube->edge_centers[0xb],&pcube->edge_centers[2]);
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[0xb],&pcube->edge_centers[5]);
                        break;
                        
                        case 0x3a:/**/
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[7],&pcube->edge_centers[1]);
                        aForm->add_polygon(&pcube->edge_centers[1],&pcube->edge_centers[7],&pcube->edge_centers[8]);
                        aForm->add_polygon(&pcube->edge_centers[1],&pcube->edge_centers[8],&pcube->edge_centers[0]);
                        aForm->add_polygon(&pcube->edge_centers[0xb],&pcube->edge_centers[2],&pcube->edge_centers[3]);
                        break;
                        
                        case 0x3b:/**/
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[7],&pcube->edge_centers[1]);
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[0xb],&pcube->edge_centers[2]);
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[2],&pcube->edge_centers[1]);
                        break;
                        
                        case 0x3c:/**/
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[7],&pcube->edge_centers[1]);
                        aForm->add_polygon(&pcube->edge_centers[1],&pcube->edge_centers[7],&pcube->edge_centers[8]);
                        aForm->add_polygon(&pcube->edge_centers[1],&pcube->edge_centers[8],&pcube->edge_centers[0]);
                        aForm->add_polygon(&pcube->edge_centers[0xb],&pcube->edge_centers[2],&pcube->edge_centers[3]);
                        break;
                        
                        case 0x3d:/**/
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[7],&pcube->edge_centers[0xb]);
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[0xb],&pcube->edge_centers[0xa]);
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[5],&pcube->edge_centers[9]);
                        break;
                        
                        case 0x3e:/**/
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[7],&pcube->edge_centers[0xb]);
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[0xb],&pcube->edge_centers[0xa]);
                        aForm->add_polygon(&pcube->edge_centers[8],&pcube->edge_centers[0],&pcube->edge_centers[3]);
                        break;
                        
                        case 0x3f:/**/
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[7],&pcube->edge_centers[0xb]);
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[0xb],&pcube->edge_centers[0xa]);
                        break;
                        
                        //////////////
                        
                        case 0x40:/**/
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[5],&pcube->edge_centers[0xa]);
                        break;
                        
                        case 0x41:/**/
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[5],&pcube->edge_centers[0xa]);
                        aForm->add_polygon(&pcube->edge_centers[8],&pcube->edge_centers[3],&pcube->edge_centers[0]);
                        break;
                        
                        //case 0x42:/**/
                        //aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[5],&pcube->edge_centers[0xa]);
                        //aForm->add_polygon(&pcube->edge_centers[9],&pcube->edge_centers[0],&pcube->edge_centers[1]);
                        //break;
                        
                        case 0x42:/**/
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[9],&pcube->edge_centers[0]);
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[0],&pcube->edge_centers[6]);
                        aForm->add_polygon(&pcube->edge_centers[1],&pcube->edge_centers[0xa],&pcube->edge_centers[6]);
                        aForm->add_polygon(&pcube->edge_centers[1],&pcube->edge_centers[6],&pcube->edge_centers[0]);
                        break;
                        
                        case 0x43:/**/
                        aForm->add_polygon(&pcube->edge_centers[9],&pcube->edge_centers[8],&pcube->edge_centers[3]);
                        aForm->add_polygon(&pcube->edge_centers[9],&pcube->edge_centers[3],&pcube->edge_centers[1]);
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[5],&pcube->edge_centers[0xa]);
                        break;
                        
                        case 0x44:/**/
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[5],&pcube->edge_centers[1]);
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[1],&pcube->edge_centers[2]);
                        break;
                        
                        case 0x45:/**/
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[5],&pcube->edge_centers[1]);
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[1],&pcube->edge_centers[2]);
                        aForm->add_polygon(&pcube->edge_centers[8],&pcube->edge_centers[3],&pcube->edge_centers[0]);
                        break;
                        
                        case 0x46:/**/
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[0],&pcube->edge_centers[2]);
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[5],&pcube->edge_centers[9]);
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[9],&pcube->edge_centers[0]);
                        break;
                        
                        case 0x47:/**/
                        aForm->add_polygon(&pcube->edge_centers[2],&pcube->edge_centers[5],&pcube->edge_centers[8]);
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[5],&pcube->edge_centers[2]);
                        aForm->add_polygon(&pcube->edge_centers[8],&pcube->edge_centers[3],&pcube->edge_centers[2]);
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[9],&pcube->edge_centers[8]);
                        break;
                        
                        //case 0x48:/**/
                        //aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[5],&pcube->edge_centers[0xa]);
                        //aForm->add_polygon(&pcube->edge_centers[0xb],&pcube->edge_centers[2],&pcube->edge_centers[3]);
                        //break;
                        
                        case 0x48:/**/
                        aForm->add_polygon(&pcube->edge_centers[0xa],&pcube->edge_centers[2],&pcube->edge_centers[3]);
                        aForm->add_polygon(&pcube->edge_centers[0xa],&pcube->edge_centers[3],&pcube->edge_centers[5]);
                        aForm->add_polygon(&pcube->edge_centers[0xb],&pcube->edge_centers[6],&pcube->edge_centers[5]);
                        aForm->add_polygon(&pcube->edge_centers[0xb],&pcube->edge_centers[5],&pcube->edge_centers[3]);
                        break;
                        
                        case 0x49:/**/
                        aForm->add_polygon(&pcube->edge_centers[8],&pcube->edge_centers[0xb],&pcube->edge_centers[2]);
                        aForm->add_polygon(&pcube->edge_centers[8],&pcube->edge_centers[2],&pcube->edge_centers[0]);
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[5],&pcube->edge_centers[0xa]);
                        break;
                        
                        case 0x4a:/**/
                        aForm->add_polygon(&pcube->edge_centers[9],&pcube->edge_centers[0],&pcube->edge_centers[1]);
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[5],&pcube->edge_centers[0xa]);
                        aForm->add_polygon(&pcube->edge_centers[0xb],&pcube->edge_centers[2],&pcube->edge_centers[3]);
                        break;
                        
                        case 0x4b:/**/
                        aForm->add_polygon(&pcube->edge_centers[9],&pcube->edge_centers[8],&pcube->edge_centers[0xb]);
                        aForm->add_polygon(&pcube->edge_centers[9],&pcube->edge_centers[0xb],&pcube->edge_centers[2]);
                        aForm->add_polygon(&pcube->edge_centers[9],&pcube->edge_centers[2],&pcube->edge_centers[1]);
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[5],&pcube->edge_centers[0xa]);
                        break;
                        
                        case 0x4c:/**/
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[1],&pcube->edge_centers[3]);
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[3],&pcube->edge_centers[0xb]);
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[0xb],&pcube->edge_centers[6]);
                        break;
                        
                        case 0x4d:/**/
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[0],&pcube->edge_centers[0xb]);
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[1],&pcube->edge_centers[0]);
                        aForm->add_polygon(&pcube->edge_centers[0],&pcube->edge_centers[8],&pcube->edge_centers[0xb]);
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[5],&pcube->edge_centers[0xb]);
                        break;
                        
                        case 0x4e:/**/
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[5],&pcube->edge_centers[9]);
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[9],&pcube->edge_centers[0xb]);
                        aForm->add_polygon(&pcube->edge_centers[0xb],&pcube->edge_centers[9],&pcube->edge_centers[0]);
                        aForm->add_polygon(&pcube->edge_centers[0xb],&pcube->edge_centers[0],&pcube->edge_centers[3]);
                        break;
                        
                        case 0x4f:/**/
                        aForm->add_polygon(&pcube->edge_centers[0xb],&pcube->edge_centers[9],&pcube->edge_centers[8]);
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[5],&pcube->edge_centers[9]);
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[9],&pcube->edge_centers[0xb]);
                        break;
                        
                        /////////////
                        
                        //case 0x50:/**/
                        //aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[5],&pcube->edge_centers[0xa]);
                        //aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[7],&pcube->edge_centers[8]);
                        //break;
                        
                        case 0x50:/**/
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[5],&pcube->edge_centers[0xa]);
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[0xa],&pcube->edge_centers[8]);
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[7],&pcube->edge_centers[8]);
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[8],&pcube->edge_centers[0xa]);
                        break;
                        
                        case 0x51:/**/
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[7],&pcube->edge_centers[3]);
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[3],&pcube->edge_centers[0]);
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[5],&pcube->edge_centers[0xa]);
                        break;
                        
                        case 0x52:/**/
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[5],&pcube->edge_centers[0xa]);
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[7],&pcube->edge_centers[8]);
                        aForm->add_polygon(&pcube->edge_centers[9],&pcube->edge_centers[0],&pcube->edge_centers[1]);
                        break;
                        
                        case 0x53:/**/
                        aForm->add_polygon(&pcube->edge_centers[1],&pcube->edge_centers[7],&pcube->edge_centers[3]);
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[7],&pcube->edge_centers[1]);
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[1],&pcube->edge_centers[9]);
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[5],&pcube->edge_centers[0xa]);
                        break;
                        
                        case 0x54:/**/
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[5],&pcube->edge_centers[1]);
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[1],&pcube->edge_centers[2]);
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[7],&pcube->edge_centers[8]);
                        break;
                        
                        case 0x55:/**/
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[7],&pcube->edge_centers[3]);
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[3],&pcube->edge_centers[0]);
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[5],&pcube->edge_centers[1]);
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[1],&pcube->edge_centers[2]);
                        break;
                        
                        case 0x56:/**/
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[0],&pcube->edge_centers[2]);
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[5],&pcube->edge_centers[9]);
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[9],&pcube->edge_centers[0]);
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[7],&pcube->edge_centers[8]);
                        break;
                        
                        case 0x57:/**/
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[5],&pcube->edge_centers[9]);
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[7],&pcube->edge_centers[3]);
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[3],&pcube->edge_centers[2]);
                        break;
                        
                        case 0x58:/**/
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[5],&pcube->edge_centers[0xa]);
                        aForm->add_polygon(&pcube->edge_centers[0xb],&pcube->edge_centers[2],&pcube->edge_centers[3]);
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[7],&pcube->edge_centers[8]);
                        break;
                        
                        case 0x59:/**/
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[2],&pcube->edge_centers[0]);
                        aForm->add_polygon(&pcube->edge_centers[2],&pcube->edge_centers[4],&pcube->edge_centers[7]);
                        aForm->add_polygon(&pcube->edge_centers[2],&pcube->edge_centers[7],&pcube->edge_centers[0xb]);
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[5],&pcube->edge_centers[0xa]);
                        break;
                        
                        case 0x5a:/**/
                        aForm->add_polygon(&pcube->edge_centers[9],&pcube->edge_centers[0],&pcube->edge_centers[1]);
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[5],&pcube->edge_centers[0xa]);
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[7],&pcube->edge_centers[8]);
                        aForm->add_polygon(&pcube->edge_centers[0xb],&pcube->edge_centers[2],&pcube->edge_centers[3]);
                        break;
                        
                        case 0x5b:/**/
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[5],&pcube->edge_centers[9]);
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[7],&pcube->edge_centers[0xb]);
                        aForm->add_polygon(&pcube->edge_centers[0xa],&pcube->edge_centers[2],&pcube->edge_centers[1]);
                        break;
                        
                        case 0x5c:/**/
                        aForm->add_polygon(&pcube->edge_centers[1],&pcube->edge_centers[3],&pcube->edge_centers[5]);
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[3],&pcube->edge_centers[0xb]);
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[0xb],&pcube->edge_centers[6]);
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[7],&pcube->edge_centers[8]);
                        break;
                        
                        case 0x5d:/**/
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[5],&pcube->edge_centers[1]);
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[1],&pcube->edge_centers[0]);
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[7],&pcube->edge_centers[0xb]);
                        break;
                        
                        case 0x5e:/**/
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[5],&pcube->edge_centers[9]);
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[7],&pcube->edge_centers[0xb]);
                        aForm->add_polygon(&pcube->edge_centers[8],&pcube->edge_centers[0],&pcube->edge_centers[3]);
                        break;
                        
                        case 0x5f:/**/
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[5],&pcube->edge_centers[9]);
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[7],&pcube->edge_centers[0xb]);
                        break;
                        
                        //////////
                        
                        case 0x60:/**/
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[4],&pcube->edge_centers[9]);
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[9],&pcube->edge_centers[0xa]);
                        break;
                        
                        case 0x61:/**/
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[4],&pcube->edge_centers[9]);
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[9],&pcube->edge_centers[0xa]);
                        aForm->add_polygon(&pcube->edge_centers[8],&pcube->edge_centers[3],&pcube->edge_centers[0]);
                        break;
                        
                        case 0x62:/**/
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[4],&pcube->edge_centers[0]);
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[0],&pcube->edge_centers[1]);
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[1],&pcube->edge_centers[0xa]);
                        break;
                        
                        case 0x63:/**/
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[3],&pcube->edge_centers[0xa]);
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[4],&pcube->edge_centers[0xa]);
                        aForm->add_polygon(&pcube->edge_centers[0xa],&pcube->edge_centers[3],&pcube->edge_centers[1]);
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[8],&pcube->edge_centers[3]);
                        break;
                        
                        case 0x64:/**/
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[4],&pcube->edge_centers[2]);
                        aForm->add_polygon(&pcube->edge_centers[2],&pcube->edge_centers[4],&pcube->edge_centers[9]);
                        aForm->add_polygon(&pcube->edge_centers[2],&pcube->edge_centers[9],&pcube->edge_centers[1]);
                        break;
                        
                        case 0x65:/**/
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[4],&pcube->edge_centers[2]);
                        aForm->add_polygon(&pcube->edge_centers[2],&pcube->edge_centers[4],&pcube->edge_centers[9]);
                        aForm->add_polygon(&pcube->edge_centers[2],&pcube->edge_centers[9],&pcube->edge_centers[1]);
                        aForm->add_polygon(&pcube->edge_centers[8],&pcube->edge_centers[3],&pcube->edge_centers[0]);
                        break;
                        
                        case 0x66:/**/
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[4],&pcube->edge_centers[0]);
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[0],&pcube->edge_centers[2]);
                        break;
                        
                        case 0x67:/**/
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[4],&pcube->edge_centers[2]);
                        aForm->add_polygon(&pcube->edge_centers[2],&pcube->edge_centers[4],&pcube->edge_centers[8]);
                        aForm->add_polygon(&pcube->edge_centers[2],&pcube->edge_centers[8],&pcube->edge_centers[3]);
                        break;
                        
                        case 0x68:/**/
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[4],&pcube->edge_centers[9]);
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[9],&pcube->edge_centers[0xa]);
                        aForm->add_polygon(&pcube->edge_centers[0xb],&pcube->edge_centers[2],&pcube->edge_centers[3]);
                        break;
                         
                        case 0x69:/**/
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[4],&pcube->edge_centers[9]);
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[9],&pcube->edge_centers[0xa]);
                        aForm->add_polygon(&pcube->edge_centers[8],&pcube->edge_centers[0xb],&pcube->edge_centers[2]);
                        aForm->add_polygon(&pcube->edge_centers[8],&pcube->edge_centers[2],&pcube->edge_centers[0]);
                        break;
                        
                        case 0x6a:/**/
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[4],&pcube->edge_centers[0]);
                        aForm->add_polygon(&pcube->edge_centers[0],&pcube->edge_centers[6],&pcube->edge_centers[0xa]);
                        aForm->add_polygon(&pcube->edge_centers[0],&pcube->edge_centers[0xa],&pcube->edge_centers[1]);
                        aForm->add_polygon(&pcube->edge_centers[0xb],&pcube->edge_centers[2],&pcube->edge_centers[3]);
                        break;
                        
                        case 0x6b:/**/
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[4],&pcube->edge_centers[8]);
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[8],&pcube->edge_centers[0xb]);
                        aForm->add_polygon(&pcube->edge_centers[0xa],&pcube->edge_centers[2],&pcube->edge_centers[1]);
                        break;
                        
                        case 0x6c:/**/
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[9],&pcube->edge_centers[3]);
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[4],&pcube->edge_centers[9]);
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[3],&pcube->edge_centers[0xb]);
                        aForm->add_polygon(&pcube->edge_centers[9],&pcube->edge_centers[1],&pcube->edge_centers[3]);
                        break;
                        
                        case 0x6d:/**/
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[4],&pcube->edge_centers[8]);
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[8],&pcube->edge_centers[0xb]);
                        aForm->add_polygon(&pcube->edge_centers[9],&pcube->edge_centers[1],&pcube->edge_centers[0]);
                        break; 
                        
                        case 0x6e:/**/
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[4],&pcube->edge_centers[0]);
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[0],&pcube->edge_centers[3]);
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[3],&pcube->edge_centers[0xb]);
                        break;
                         
                        case 0x6f:/**/
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[4],&pcube->edge_centers[8]);
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[8],&pcube->edge_centers[0xb]);
                        break;
                        
                        //////////
                        
                        case 0x70:/**/
                        aForm->add_polygon(&pcube->edge_centers[9],&pcube->edge_centers[0xa],&pcube->edge_centers[8]);
                        aForm->add_polygon(&pcube->edge_centers[8],&pcube->edge_centers[0xa],&pcube->edge_centers[6]);
                        aForm->add_polygon(&pcube->edge_centers[8],&pcube->edge_centers[6],&pcube->edge_centers[7]);
                        break;
                        
                        case 0x71:/**/
                        aForm->add_polygon(&pcube->edge_centers[0xa],&pcube->edge_centers[7],&pcube->edge_centers[0]);
                        aForm->add_polygon(&pcube->edge_centers[0xa],&pcube->edge_centers[6],&pcube->edge_centers[7]);
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[3],&pcube->edge_centers[0]);
                        aForm->add_polygon(&pcube->edge_centers[9],&pcube->edge_centers[0xa],&pcube->edge_centers[0]);
                        break;
                        
                        case 0x72:/**/
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[7],&pcube->edge_centers[8]);
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[8],&pcube->edge_centers[0xa]);
                        aForm->add_polygon(&pcube->edge_centers[0xa],&pcube->edge_centers[8],&pcube->edge_centers[0]);
                        aForm->add_polygon(&pcube->edge_centers[0xa],&pcube->edge_centers[0],&pcube->edge_centers[1]);
                        break;
                        
                        case 0x73:/**/
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[3],&pcube->edge_centers[1]);
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[1],&pcube->edge_centers[0xa]);
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[0xa],&pcube->edge_centers[6]);
                        break;
                         
                        case 0x74:/**/
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[8],&pcube->edge_centers[1]);
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[7],&pcube->edge_centers[8]);
                        aForm->add_polygon(&pcube->edge_centers[8],&pcube->edge_centers[9],&pcube->edge_centers[1]);
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[1],&pcube->edge_centers[2]);
                        break;
                        
                        case 0x75:/**/
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[7],&pcube->edge_centers[3]);
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[3],&pcube->edge_centers[2]);
                        aForm->add_polygon(&pcube->edge_centers[9],&pcube->edge_centers[1],&pcube->edge_centers[0]);
                        break;
                        
                        case 0x76:/**/
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[0],&pcube->edge_centers[2]);
                        aForm->add_polygon(&pcube->edge_centers[0],&pcube->edge_centers[6],&pcube->edge_centers[7]);
                        aForm->add_polygon(&pcube->edge_centers[0],&pcube->edge_centers[7],&pcube->edge_centers[8]);
                        break; 
                        
                        case 0x77:/**/
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[7],&pcube->edge_centers[3]);
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[3],&pcube->edge_centers[2]);
                        break;
                        
                        case 0x78:/**/
                        aForm->add_polygon(&pcube->edge_centers[9],&pcube->edge_centers[0xa],&pcube->edge_centers[8]);
                        aForm->add_polygon(&pcube->edge_centers[8],&pcube->edge_centers[0xa],&pcube->edge_centers[6]);
                        aForm->add_polygon(&pcube->edge_centers[8],&pcube->edge_centers[6],&pcube->edge_centers[7]);
                        aForm->add_polygon(&pcube->edge_centers[0xb],&pcube->edge_centers[2],&pcube->edge_centers[3]);
                        break;
                        
                        case 0x79:/**/
                        aForm->add_polygon(&pcube->edge_centers[9],&pcube->edge_centers[0xa],&pcube->edge_centers[2]);
                        aForm->add_polygon(&pcube->edge_centers[9],&pcube->edge_centers[2],&pcube->edge_centers[0]);
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[7],&pcube->edge_centers[0xb]);
                        break;
                        
                        case 0x7a:/**/
                        aForm->add_polygon(&pcube->edge_centers[8],&pcube->edge_centers[0],&pcube->edge_centers[3]);
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[7],&pcube->edge_centers[0xb]);
                        aForm->add_polygon(&pcube->edge_centers[0xa],&pcube->edge_centers[2],&pcube->edge_centers[1]);
                        break; 
                        
                        case 0x7b:/**/
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[7],&pcube->edge_centers[0xb]);
                        aForm->add_polygon(&pcube->edge_centers[0xa],&pcube->edge_centers[2],&pcube->edge_centers[1]);
                        break;
                         
                        case 0x7c:/**/
                        aForm->add_polygon(&pcube->edge_centers[8],&pcube->edge_centers[9],&pcube->edge_centers[1]);
                        aForm->add_polygon(&pcube->edge_centers[8],&pcube->edge_centers[1],&pcube->edge_centers[3]);
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[7],&pcube->edge_centers[0xb]);
                        break;
                         
                        case 0x7d:/**/
                        aForm->add_polygon(&pcube->edge_centers[9],&pcube->edge_centers[0],&pcube->edge_centers[1]);
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[7],&pcube->edge_centers[0xb]);
                        break; 
                        
                        case 0x7e:/**/
                        aForm->add_polygon(&pcube->edge_centers[8],&pcube->edge_centers[0],&pcube->edge_centers[3]);
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[7],&pcube->edge_centers[0xb]);
                        break;
                         
                        case 0x7f:/**/
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[7],&pcube->edge_centers[0xb]);
                        break;
                        
                        ///////////
                        
                        case 0x80:/**/
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[6],&pcube->edge_centers[0xb]);
                        break;
                        
                        //case 0x81:/**/
                        //aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[6],&pcube->edge_centers[0xb]);
                        //aForm->add_polygon(&pcube->edge_centers[8],&pcube->edge_centers[3],&pcube->edge_centers[0);
                        //break;
                        
                        case 0x81:/**/
                        aForm->add_polygon(&pcube->edge_centers[0xb],&pcube->edge_centers[3],&pcube->edge_centers[0]);
                        aForm->add_polygon(&pcube->edge_centers[0xb],&pcube->edge_centers[0],&pcube->edge_centers[6]);
                        aForm->add_polygon(&pcube->edge_centers[8],&pcube->edge_centers[7],&pcube->edge_centers[6]);
                        aForm->add_polygon(&pcube->edge_centers[8],&pcube->edge_centers[6],&pcube->edge_centers[0]);
                        break;
                         
                        case 0x82:/**/
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[6],&pcube->edge_centers[0xb]);
                        aForm->add_polygon(&pcube->edge_centers[9],&pcube->edge_centers[0],&pcube->edge_centers[1]);
                        break;
                         
                        case 0x83:/**/
                        aForm->add_polygon(&pcube->edge_centers[9],&pcube->edge_centers[8],&pcube->edge_centers[3]);
                        aForm->add_polygon(&pcube->edge_centers[9],&pcube->edge_centers[3],&pcube->edge_centers[1]);
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[6],&pcube->edge_centers[0xb]);
                        break;
                         
                        //case 0x84:/**/
                        //aForm->add_polygon(&pcube->edge_centers[0xa],&pcube->edge_centers[1],&pcube->edge_centers[2]);
                        //aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[6],&pcube->edge_centers[0xb]);
                        //break;
                        
                        case 0x84:/**/
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[0xa],&pcube->edge_centers[1]);
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[1],&pcube->edge_centers[7]);
                        aForm->add_polygon(&pcube->edge_centers[2],&pcube->edge_centers[0xb],&pcube->edge_centers[7]);
                        aForm->add_polygon(&pcube->edge_centers[2],&pcube->edge_centers[7],&pcube->edge_centers[1]);
                        break;
                        
                        case 0x85:/**/
                        aForm->add_polygon(&pcube->edge_centers[0xa],&pcube->edge_centers[1],&pcube->edge_centers[2]);
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[6],&pcube->edge_centers[0xb]);
                        aForm->add_polygon(&pcube->edge_centers[8],&pcube->edge_centers[3],&pcube->edge_centers[0]);
                        break;
                        
                        case 0x86:/**/
                        aForm->add_polygon(&pcube->edge_centers[0xa],&pcube->edge_centers[9],&pcube->edge_centers[0]);
                        aForm->add_polygon(&pcube->edge_centers[0xa],&pcube->edge_centers[0],&pcube->edge_centers[2]);
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[6],&pcube->edge_centers[0xb]);
                        break;
                        
                        case 0x87:/**/
                        aForm->add_polygon(&pcube->edge_centers[9],&pcube->edge_centers[8],&pcube->edge_centers[0xa]);
                        aForm->add_polygon(&pcube->edge_centers[0xa],&pcube->edge_centers[8],&pcube->edge_centers[3]);
                        aForm->add_polygon(&pcube->edge_centers[0xa],&pcube->edge_centers[3],&pcube->edge_centers[2]);
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[6],&pcube->edge_centers[0xb]);
                        break;
                         
                        case 0x88:/**/
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[6],&pcube->edge_centers[2]);
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[2],&pcube->edge_centers[3]);
                        break;
                        
                        case 0x89:/**/
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[2],&pcube->edge_centers[0]);
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[0],&pcube->edge_centers[8]);
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[8],&pcube->edge_centers[7]);
                        break;
                        
                        case 0x8a:/**/
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[6],&pcube->edge_centers[2]);
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[2],&pcube->edge_centers[3]);
                        aForm->add_polygon(&pcube->edge_centers[9],&pcube->edge_centers[0],&pcube->edge_centers[1]);
                        break;
                        
                        case 0x8b:/**/
                        aForm->add_polygon(&pcube->edge_centers[8],&pcube->edge_centers[6],&pcube->edge_centers[1]);
                        aForm->add_polygon(&pcube->edge_centers[8],&pcube->edge_centers[7],&pcube->edge_centers[6]);
                        aForm->add_polygon(&pcube->edge_centers[9],&pcube->edge_centers[8],&pcube->edge_centers[1]);
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[2],&pcube->edge_centers[1]);
                        break;
                        
                        case 0x8c:/**/
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[1],&pcube->edge_centers[3]);
                        aForm->add_polygon(&pcube->edge_centers[1],&pcube->edge_centers[7],&pcube->edge_centers[6]);
                        aForm->add_polygon(&pcube->edge_centers[1],&pcube->edge_centers[6],&pcube->edge_centers[0xa]);
                        break;
                        
                        case 0x8d:/**/
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[6],&pcube->edge_centers[0xa]);
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[0xa],&pcube->edge_centers[8]);
                        aForm->add_polygon(&pcube->edge_centers[8],&pcube->edge_centers[0xa],&pcube->edge_centers[1]);
                        aForm->add_polygon(&pcube->edge_centers[8],&pcube->edge_centers[1],&pcube->edge_centers[0]);
                        break;
                        
                        case 0x8e:/**/
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[0xa],&pcube->edge_centers[0]);
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[6],&pcube->edge_centers[0xa]);
                        aForm->add_polygon(&pcube->edge_centers[0],&pcube->edge_centers[0xa],&pcube->edge_centers[9]);
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[0],&pcube->edge_centers[3]);
                        break;
                        
                        case 0x8f:/**/
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[6],&pcube->edge_centers[0xa]);
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[0xa],&pcube->edge_centers[8]);
                        aForm->add_polygon(&pcube->edge_centers[8],&pcube->edge_centers[0xa],&pcube->edge_centers[9]);
                        break;
                        
                        ///////////
                        
                        case 0x90:/**/
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[6],&pcube->edge_centers[0xb]);
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[0xb],&pcube->edge_centers[8]);
                        break;
                        
                        case 0x91:/**/
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[6],&pcube->edge_centers[0]);
                        aForm->add_polygon(&pcube->edge_centers[0],&pcube->edge_centers[6],&pcube->edge_centers[0xb]);
                        aForm->add_polygon(&pcube->edge_centers[0],&pcube->edge_centers[0xb],&pcube->edge_centers[3]);
                        break;
                        
                        case 0x92:/**/
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[6],&pcube->edge_centers[0xb]);
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[0xb],&pcube->edge_centers[8]);
                        aForm->add_polygon(&pcube->edge_centers[9],&pcube->edge_centers[0],&pcube->edge_centers[1]);
                        break;
                        
                        case 0x93:/**/
                        aForm->add_polygon(&pcube->edge_centers[9],&pcube->edge_centers[6],&pcube->edge_centers[3]);
                        aForm->add_polygon(&pcube->edge_centers[9],&pcube->edge_centers[3],&pcube->edge_centers[1]);
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[6],&pcube->edge_centers[9]);
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[0xb],&pcube->edge_centers[3]);
                        break;
                        
                        case 0x94:/**/
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[6],&pcube->edge_centers[0xb]);
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[0xb],&pcube->edge_centers[8]);
                        aForm->add_polygon(&pcube->edge_centers[0xa],&pcube->edge_centers[1],&pcube->edge_centers[2]);
                        break;
                        
                        case 0x95:/**/
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[6],&pcube->edge_centers[0]);
                        aForm->add_polygon(&pcube->edge_centers[0],&pcube->edge_centers[6],&pcube->edge_centers[0xb]);
                        aForm->add_polygon(&pcube->edge_centers[0],&pcube->edge_centers[0xb],&pcube->edge_centers[3]);
                        aForm->add_polygon(&pcube->edge_centers[0xa],&pcube->edge_centers[1],&pcube->edge_centers[2]);
                        break;
                        
                        case 0x96:/**/
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[6],&pcube->edge_centers[0xb]);
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[0xb],&pcube->edge_centers[8]);
                        aForm->add_polygon(&pcube->edge_centers[0xa],&pcube->edge_centers[9],&pcube->edge_centers[0]);
                        aForm->add_polygon(&pcube->edge_centers[0xa],&pcube->edge_centers[0],&pcube->edge_centers[2]);
                        break;
                        
                        case 0x97:/**/
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[6],&pcube->edge_centers[0xa]);
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[0xa],&pcube->edge_centers[9]);
                        aForm->add_polygon(&pcube->edge_centers[0xb],&pcube->edge_centers[3],&pcube->edge_centers[2]);
                        break;
                        
                        case 0x98:/**/
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[6],&pcube->edge_centers[2]);
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[2],&pcube->edge_centers[3]);
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[3],&pcube->edge_centers[8]);
                        break;
                        
                        case 0x99:/**/
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[6],&pcube->edge_centers[2]);
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[2],&pcube->edge_centers[0]);
                        break;
                        
                        case 0x9a:/**/
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[6],&pcube->edge_centers[2]);
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[2],&pcube->edge_centers[3]);
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[3],&pcube->edge_centers[8]);
                        aForm->add_polygon(&pcube->edge_centers[9],&pcube->edge_centers[0],&pcube->edge_centers[1]);
                        break;
                        
                        case 0x9b:/**/
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[6],&pcube->edge_centers[2]);
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[2],&pcube->edge_centers[1]);
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[1],&pcube->edge_centers[9]);
                        break;
                        
                        case 0x9c:/**/
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[0xa],&pcube->edge_centers[3]);
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[6],&pcube->edge_centers[0xa]);
                        aForm->add_polygon(&pcube->edge_centers[0xa],&pcube->edge_centers[1],&pcube->edge_centers[3]);
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[3],&pcube->edge_centers[8]);
                        break;
                        
                        case 0x9d:/**/
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[6],&pcube->edge_centers[0]);
                        aForm->add_polygon(&pcube->edge_centers[0],&pcube->edge_centers[6],&pcube->edge_centers[0xa]);
                        aForm->add_polygon(&pcube->edge_centers[0],&pcube->edge_centers[0xa],&pcube->edge_centers[1]);
                        break;
                        
                        case 0x9e:/**/
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[6],&pcube->edge_centers[0xa]);
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[0xa],&pcube->edge_centers[9]);
                        aForm->add_polygon(&pcube->edge_centers[8],&pcube->edge_centers[0],&pcube->edge_centers[3]);
                        break;
                        
                        case 0x9f:/**/
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[6],&pcube->edge_centers[0xa]);
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[0xa],&pcube->edge_centers[9]);
                        break;
                        
                        ////////////
                        
                        //case 0xa0:/**/
                        //aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[4],&pcube->edge_centers[9]);
                        //aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[6],&pcube->edge_centers[0xb]);
                        //break;
                        
                        case 0xa0:/**/
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[6],&pcube->edge_centers[0xb]);
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[0xb],&pcube->edge_centers[9]);
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[4],&pcube->edge_centers[9]);
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[9],&pcube->edge_centers[0xb]);
                        break;
                        
                        case 0xa1:/**/
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[4],&pcube->edge_centers[9]);
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[6],&pcube->edge_centers[0xb]);
                        aForm->add_polygon(&pcube->edge_centers[8],&pcube->edge_centers[3],&pcube->edge_centers[0]);
                        break;
                        
                        case 0xa2:/**/
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[4],&pcube->edge_centers[0]);
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[0],&pcube->edge_centers[1]);
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[6],&pcube->edge_centers[0xb]);
                        break;
                        
                        case 0xa3:/**/
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[3],&pcube->edge_centers[1]);
                        aForm->add_polygon(&pcube->edge_centers[3],&pcube->edge_centers[5],&pcube->edge_centers[4]);
                        aForm->add_polygon(&pcube->edge_centers[3],&pcube->edge_centers[4],&pcube->edge_centers[8]);
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[6],&pcube->edge_centers[0xb]);
                        break;
                        
                        case 0xa4:/**/
                        aForm->add_polygon(&pcube->edge_centers[0xa],&pcube->edge_centers[1],&pcube->edge_centers[2]);
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[4],&pcube->edge_centers[9]);
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[6],&pcube->edge_centers[0xb]);
                        break;
                        
                        case 0xa5:/**/
                        aForm->add_polygon(&pcube->edge_centers[0xa],&pcube->edge_centers[1],&pcube->edge_centers[2]);
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[4],&pcube->edge_centers[9]);
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[6],&pcube->edge_centers[0xb]);
                        aForm->add_polygon(&pcube->edge_centers[8],&pcube->edge_centers[3],&pcube->edge_centers[0]);
                        break;

                        case 0xa6:/**/
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[0],&pcube->edge_centers[2]);
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[2],&pcube->edge_centers[0xa]);
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[0xa],&pcube->edge_centers[5]);
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[6],&pcube->edge_centers[0xb]);
                        break;
                        
                        case 0xa7:/**/
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[6],&pcube->edge_centers[0xa]);
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[4],&pcube->edge_centers[8]);
                        aForm->add_polygon(&pcube->edge_centers[0xb],&pcube->edge_centers[3],&pcube->edge_centers[2]);
                        break;
                        
                        case 0xa8:/**/
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[6],&pcube->edge_centers[2]);
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[2],&pcube->edge_centers[3]);
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[4],&pcube->edge_centers[9]);
                        break;
                        
                        case 0xa9:/**/
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[2],&pcube->edge_centers[0]);
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[0],&pcube->edge_centers[8]);
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[8],&pcube->edge_centers[7]);
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[4],&pcube->edge_centers[9]);
                        break;
                        
                        case 0xaa:/**/
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[6],&pcube->edge_centers[2]);
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[2],&pcube->edge_centers[3]);
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[4],&pcube->edge_centers[0]);
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[0],&pcube->edge_centers[1]);
                        break;
                        
                        case 0xab:/**/
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[4],&pcube->edge_centers[8]);
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[6],&pcube->edge_centers[2]);
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[2],&pcube->edge_centers[1]);
                        break;
                        
                        case 0xac:/**/
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[1],&pcube->edge_centers[3]);
                        aForm->add_polygon(&pcube->edge_centers[1],&pcube->edge_centers[7],&pcube->edge_centers[6]);
                        aForm->add_polygon(&pcube->edge_centers[1],&pcube->edge_centers[6],&pcube->edge_centers[0xa]);
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[4],&pcube->edge_centers[9]);
                        break;
                        
                        case 0xad:/**/
                        aForm->add_polygon(&pcube->edge_centers[9],&pcube->edge_centers[1],&pcube->edge_centers[0]);
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[4],&pcube->edge_centers[8]);
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[6],&pcube->edge_centers[0xa]);
                        break;
                        
                        case 0xae:/**/
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[4],&pcube->edge_centers[0]);
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[0],&pcube->edge_centers[3]);
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[6],&pcube->edge_centers[0xa]);
                        break;
                        
                        case 0xaf:/**/
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[4],&pcube->edge_centers[8]);
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[6],&pcube->edge_centers[0xa]);
                        break;
                        
                        //////////
                        
                        case 0xb0:/**/
                        aForm->add_polygon(&pcube->edge_centers[9],&pcube->edge_centers[0xb],&pcube->edge_centers[8]);
                        aForm->add_polygon(&pcube->edge_centers[0xb],&pcube->edge_centers[9],&pcube->edge_centers[5]);
                        aForm->add_polygon(&pcube->edge_centers[0xb],&pcube->edge_centers[5],&pcube->edge_centers[6]);
                        break; 

                        case 0xb1:/**/
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[6],&pcube->edge_centers[0xb]);
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[0xb],&pcube->edge_centers[9]);
                        aForm->add_polygon(&pcube->edge_centers[9],&pcube->edge_centers[0xb],&pcube->edge_centers[3]);
                        aForm->add_polygon(&pcube->edge_centers[9],&pcube->edge_centers[3],&pcube->edge_centers[0]);
                        break;
                        
                        case 0xb2:/**/
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[0xb],&pcube->edge_centers[0]);
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[6],&pcube->edge_centers[0xb]);
                        aForm->add_polygon(&pcube->edge_centers[0],&pcube->edge_centers[0xb],&pcube->edge_centers[8]);
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[0],&pcube->edge_centers[1]);
                        break;
                        
                        case 0xb3:/**/
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[3],&pcube->edge_centers[1]);
                        aForm->add_polygon(&pcube->edge_centers[3],&pcube->edge_centers[5],&pcube->edge_centers[6]);
                        aForm->add_polygon(&pcube->edge_centers[3],&pcube->edge_centers[6],&pcube->edge_centers[0xb]);
                        break;
                        
                        case 0xb4:/**/
                        aForm->add_polygon(&pcube->edge_centers[8],&pcube->edge_centers[9],&pcube->edge_centers[0xb]);
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[6],&pcube->edge_centers[0xb]);
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[0xb],&pcube->edge_centers[9]);
                        aForm->add_polygon(&pcube->edge_centers[0xa],&pcube->edge_centers[1],&pcube->edge_centers[2]);
                        break;
                        
                        case 0xb5:/**/
                        aForm->add_polygon(&pcube->edge_centers[9],&pcube->edge_centers[1],&pcube->edge_centers[0]);
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[6],&pcube->edge_centers[0xa]);
                        aForm->add_polygon(&pcube->edge_centers[0xb],&pcube->edge_centers[3],&pcube->edge_centers[2]);
                        break;
                        
                        case 0xb6:/**/
                        aForm->add_polygon(&pcube->edge_centers[0xb],&pcube->edge_centers[8],&pcube->edge_centers[2]);
                        aForm->add_polygon(&pcube->edge_centers[8],&pcube->edge_centers[0],&pcube->edge_centers[2]);
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[6],&pcube->edge_centers[0xa]);
                        break;
                        
                        case 0xb7:/**/
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[6],&pcube->edge_centers[0xa]);
                        aForm->add_polygon(&pcube->edge_centers[0xb],&pcube->edge_centers[3],&pcube->edge_centers[2]);
                        break;
                        
                        case 0xb8:/**/
                        aForm->add_polygon(&pcube->edge_centers[9],&pcube->edge_centers[5],&pcube->edge_centers[8]);
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[2],&pcube->edge_centers[8]);
                        aForm->add_polygon(&pcube->edge_centers[8],&pcube->edge_centers[2],&pcube->edge_centers[3]);
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[6],&pcube->edge_centers[2]);
                        break;
                        
                        case 0xb9:/**/
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[2],&pcube->edge_centers[0]);
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[0],&pcube->edge_centers[9]);
                        aForm->add_polygon(&pcube->edge_centers[6],&pcube->edge_centers[9],&pcube->edge_centers[5]);
                        break;
                        
                        case 0xba:/**/
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[6],&pcube->edge_centers[2]);
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[2],&pcube->edge_centers[1]);
                        aForm->add_polygon(&pcube->edge_centers[8],&pcube->edge_centers[0],&pcube->edge_centers[3]);
                        break; 

                        case 0xbb:/**/
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[6],&pcube->edge_centers[2]);
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[2],&pcube->edge_centers[1]);
                        break;
                        
                        case 0xbc:/**/
                        aForm->add_polygon(&pcube->edge_centers[8],&pcube->edge_centers[9],&pcube->edge_centers[1]);
                        aForm->add_polygon(&pcube->edge_centers[8],&pcube->edge_centers[1],&pcube->edge_centers[3]);
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[6],&pcube->edge_centers[0xa]);
                        break;
                        
                        case 0xbd:/**/
                        aForm->add_polygon(&pcube->edge_centers[9],&pcube->edge_centers[1],&pcube->edge_centers[0]);
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[6],&pcube->edge_centers[0xa]);
                        break;
                        
                        case 0xbe:/**/
                        aForm->add_polygon(&pcube->edge_centers[8],&pcube->edge_centers[0],&pcube->edge_centers[3]);
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[6],&pcube->edge_centers[0xa]);
                        break;   

                        case 0xbf:/**/
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[6],&pcube->edge_centers[0xa]);
                        break; 
                        
                        ////////////
                        
                        case 0xc0:/**/
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[5],&pcube->edge_centers[0xa]);
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[0xa],&pcube->edge_centers[0xb]);
                        break;
                        
                        case 0xc1:/**/
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[5],&pcube->edge_centers[0xa]);
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[0xa],&pcube->edge_centers[0xb]);
                        aForm->add_polygon(&pcube->edge_centers[8],&pcube->edge_centers[3],&pcube->edge_centers[0]);
                        break;
                        
                        case 0xc2:/**/
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[5],&pcube->edge_centers[0xa]);
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[0xa],&pcube->edge_centers[0xb]);
                        aForm->add_polygon(&pcube->edge_centers[9],&pcube->edge_centers[0],&pcube->edge_centers[1]);
                        break;
                        
                        case 0xc3:/**/
                        aForm->add_polygon(&pcube->edge_centers[9],&pcube->edge_centers[8],&pcube->edge_centers[3]);
                        aForm->add_polygon(&pcube->edge_centers[9],&pcube->edge_centers[3],&pcube->edge_centers[1]);
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[5],&pcube->edge_centers[0xa]);
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[0xa],&pcube->edge_centers[0xb]);
                        break;

                        case 0xc4:/**/
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[5],&pcube->edge_centers[1]);
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[1],&pcube->edge_centers[2]);
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[2],&pcube->edge_centers[0xb]);
                        break;
                        
                        case 0xc5:/**/
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[5],&pcube->edge_centers[1]);
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[1],&pcube->edge_centers[2]);
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[2],&pcube->edge_centers[0xb]);
                        aForm->add_polygon(&pcube->edge_centers[8],&pcube->edge_centers[3],&pcube->edge_centers[0]);
                        break;
                        
                        case 0xc6:/**/
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[0],&pcube->edge_centers[0xb]);
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[9],&pcube->edge_centers[0]);
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[5],&pcube->edge_centers[0xb]);
                        aForm->add_polygon(&pcube->edge_centers[0xb],&pcube->edge_centers[0],&pcube->edge_centers[2]);
                        break;
                        
                        case 0xc7:/**/
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[5],&pcube->edge_centers[9]);
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[9],&pcube->edge_centers[8]);
                        aForm->add_polygon(&pcube->edge_centers[0xb],&pcube->edge_centers[3],&pcube->edge_centers[2]);
                        break;

                        case 0xc8:/**/
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[5],&pcube->edge_centers[3]);
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[0xa],&pcube->edge_centers[2]);
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[2],&pcube->edge_centers[3]);
                        break;
                        
                        case 0xc9:/**/
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[0xa],&pcube->edge_centers[0]);
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[5],&pcube->edge_centers[0xa]);
                        aForm->add_polygon(&pcube->edge_centers[0xa],&pcube->edge_centers[2],&pcube->edge_centers[0]);
                        aForm->add_polygon(&pcube->edge_centers[8],&pcube->edge_centers[7],&pcube->edge_centers[0]);
                        break;
                        
                        case 0xca:/**/
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[5],&pcube->edge_centers[3]);
                        aForm->add_polygon(&pcube->edge_centers[3],&pcube->edge_centers[5],&pcube->edge_centers[0xa]);
                        aForm->add_polygon(&pcube->edge_centers[3],&pcube->edge_centers[0xa],&pcube->edge_centers[1]);
                        aForm->add_polygon(&pcube->edge_centers[9],&pcube->edge_centers[0],&pcube->edge_centers[1]);
                        break;
                        
                        case 0xcb:/**/
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[5],&pcube->edge_centers[8]);
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[9],&pcube->edge_centers[8]);
                        aForm->add_polygon(&pcube->edge_centers[0xa],&pcube->edge_centers[2],&pcube->edge_centers[1]);
                        break;
                        
                        
                        case 0xcc:/**/
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[5],&pcube->edge_centers[1]);
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[1],&pcube->edge_centers[3]);
                        break;
                        
                        case 0xcd:/**/
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[5],&pcube->edge_centers[1]);
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[1],&pcube->edge_centers[0]);
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[0],&pcube->edge_centers[8]);
                        break;
                        
                        case 0xce:/**/
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[5],&pcube->edge_centers[3]);
                        aForm->add_polygon(&pcube->edge_centers[3],&pcube->edge_centers[5],&pcube->edge_centers[9]);
                        aForm->add_polygon(&pcube->edge_centers[3],&pcube->edge_centers[9],&pcube->edge_centers[0]);
                        break;
                        
                        case 0xcf:/**/
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[5],&pcube->edge_centers[9]);
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[9],&pcube->edge_centers[8]);
                        break;
                        
                        /////////
                        
                        case 0xd0:/**/
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[5],&pcube->edge_centers[0xa]);
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[0xa],&pcube->edge_centers[8]);
                        aForm->add_polygon(&pcube->edge_centers[8],&pcube->edge_centers[0xa],&pcube->edge_centers[0xb]);
                        break;
                        
                        case 0xd1:/**/
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[0xb],&pcube->edge_centers[0]);
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[5],&pcube->edge_centers[0]);
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[0xa],&pcube->edge_centers[0xb]);
                        aForm->add_polygon(&pcube->edge_centers[0],&pcube->edge_centers[0xb],&pcube->edge_centers[3]);
                        break;
                        
                        case 0xd2:/**/
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[5],&pcube->edge_centers[0xa]);
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[0xa],&pcube->edge_centers[8]);
                        aForm->add_polygon(&pcube->edge_centers[8],&pcube->edge_centers[0xa],&pcube->edge_centers[0xb]);
                        aForm->add_polygon(&pcube->edge_centers[9],&pcube->edge_centers[0],&pcube->edge_centers[1]);
                        break;
                        
                        case 0xd3:/**/
                        aForm->add_polygon(&pcube->edge_centers[0xa],&pcube->edge_centers[0xb],&pcube->edge_centers[3]);
                        aForm->add_polygon(&pcube->edge_centers[0xa],&pcube->edge_centers[3],&pcube->edge_centers[1]);
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[5],&pcube->edge_centers[9]);
                        break;
                        
                        case 0xd4:/**/
                        aForm->add_polygon(&pcube->edge_centers[0xb],&pcube->edge_centers[4],&pcube->edge_centers[1]);
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[5],&pcube->edge_centers[1]);
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[0xb],&pcube->edge_centers[8]);
                        aForm->add_polygon(&pcube->edge_centers[2],&pcube->edge_centers[0xb],&pcube->edge_centers[1]);
                        break;
                        
                        case 0xd5:/**/
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[5],&pcube->edge_centers[1]);
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[1],&pcube->edge_centers[0]);
                        aForm->add_polygon(&pcube->edge_centers[0xb],&pcube->edge_centers[3],&pcube->edge_centers[2]);
                        break;
                        
                        case 0xd6:/**/
                        aForm->add_polygon(&pcube->edge_centers[0xb],&pcube->edge_centers[8],&pcube->edge_centers[0]);
                        aForm->add_polygon(&pcube->edge_centers[0xb],&pcube->edge_centers[0],&pcube->edge_centers[2]);
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[5],&pcube->edge_centers[9]);
                        break;
                        
                        case 0xd7:/**/
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[5],&pcube->edge_centers[9]);
                        aForm->add_polygon(&pcube->edge_centers[0xb],&pcube->edge_centers[3],&pcube->edge_centers[2]);
                        break;
                        
                        case 0xd8:/**/
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[5],&pcube->edge_centers[0xa]);
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[0xa],&pcube->edge_centers[8]);
                        aForm->add_polygon(&pcube->edge_centers[8],&pcube->edge_centers[0xa],&pcube->edge_centers[2]);
                        aForm->add_polygon(&pcube->edge_centers[8],&pcube->edge_centers[2],&pcube->edge_centers[3]);
                        break;
                        
                        case 0xd9:/**/
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[2],&pcube->edge_centers[0]);
                        aForm->add_polygon(&pcube->edge_centers[2],&pcube->edge_centers[4],&pcube->edge_centers[5]);
                        aForm->add_polygon(&pcube->edge_centers[2],&pcube->edge_centers[5],&pcube->edge_centers[0xa]);
                        break;
                        
                        case 0xda:/**/
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[5],&pcube->edge_centers[9]);
                        aForm->add_polygon(&pcube->edge_centers[8],&pcube->edge_centers[0],&pcube->edge_centers[3]);
                        aForm->add_polygon(&pcube->edge_centers[0xa],&pcube->edge_centers[2],&pcube->edge_centers[1]);
                        break;
                        
                        case 0xdb:/**/
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[5],&pcube->edge_centers[9]);
                        aForm->add_polygon(&pcube->edge_centers[0xa],&pcube->edge_centers[2],&pcube->edge_centers[1]);
                        break;

                        case 0xdc:/**/
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[1],&pcube->edge_centers[3]);
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[3],&pcube->edge_centers[8]);
                        aForm->add_polygon(&pcube->edge_centers[5],&pcube->edge_centers[8],&pcube->edge_centers[4]);
                        break;
                        
                        case 0xdd:/**/
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[5],&pcube->edge_centers[1]);
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[1],&pcube->edge_centers[0]);
                        break;
                        
                        case 0xde:/**/
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[5],&pcube->edge_centers[9]);
                        aForm->add_polygon(&pcube->edge_centers[8],&pcube->edge_centers[0],&pcube->edge_centers[3]);
                        break;

                        case 0xdf:/**/
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[5],&pcube->edge_centers[9]);
                        break;
                        
                        ///////////
                        
                        case 0xe0:/**/
                        aForm->add_polygon(&pcube->edge_centers[0xb],&pcube->edge_centers[9],&pcube->edge_centers[0xa]);
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[4],&pcube->edge_centers[9]);
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[9],&pcube->edge_centers[0xb]);
                        break;
                        
                        case 0xe1:/**/
                        aForm->add_polygon(&pcube->edge_centers[8],&pcube->edge_centers[3],&pcube->edge_centers[0]);
                        aForm->add_polygon(&pcube->edge_centers[0xb],&pcube->edge_centers[9],&pcube->edge_centers[0xa]);
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[4],&pcube->edge_centers[9]);
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[9],&pcube->edge_centers[0xb]);
                        break;
                        
                        case 0xe2:/**/
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[0],&pcube->edge_centers[0xa]);
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[4],&pcube->edge_centers[0]);
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[0xa],&pcube->edge_centers[0xb]);
                        aForm->add_polygon(&pcube->edge_centers[0xa],&pcube->edge_centers[0],&pcube->edge_centers[1]);
                        break;
                        
                        case 0xe3:/**/
                        aForm->add_polygon(&pcube->edge_centers[0xa],&pcube->edge_centers[0xb],&pcube->edge_centers[3]);
                        aForm->add_polygon(&pcube->edge_centers[0xa],&pcube->edge_centers[3],&pcube->edge_centers[1]);
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[4],&pcube->edge_centers[8]);
                        break;

                        case 0xe4:/**/
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[4],&pcube->edge_centers[9]);
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[9],&pcube->edge_centers[0xb]);
                        aForm->add_polygon(&pcube->edge_centers[0xb],&pcube->edge_centers[9],&pcube->edge_centers[1]);
                        aForm->add_polygon(&pcube->edge_centers[0xb],&pcube->edge_centers[1],&pcube->edge_centers[2]);
                        break;
                        
                        case 0xe5:/**/
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[4],&pcube->edge_centers[8]);
                        aForm->add_polygon(&pcube->edge_centers[9],&pcube->edge_centers[1],&pcube->edge_centers[0]);
                        aForm->add_polygon(&pcube->edge_centers[0xb],&pcube->edge_centers[3],&pcube->edge_centers[2]);
                        break;

                        case 0xe6:/**/
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[0],&pcube->edge_centers[2]);
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[2],&pcube->edge_centers[0xb]);
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[0xb],&pcube->edge_centers[7]);
                        break;
                        
                        case 0xe7:/**/
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[4],&pcube->edge_centers[8]);
                        aForm->add_polygon(&pcube->edge_centers[0xb],&pcube->edge_centers[3],&pcube->edge_centers[2]);
                        break;
                        
                        case 0xe8:/**/
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[0xa],&pcube->edge_centers[3]);
                        aForm->add_polygon(&pcube->edge_centers[4],&pcube->edge_centers[9],&pcube->edge_centers[0xa]);
                        aForm->add_polygon(&pcube->edge_centers[0xa],&pcube->edge_centers[2],&pcube->edge_centers[3]);
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[4],&pcube->edge_centers[3]);
                        break;
                        
                        case 0xe9:/**/
                        aForm->add_polygon(&pcube->edge_centers[9],&pcube->edge_centers[0xa],&pcube->edge_centers[2]);
                        aForm->add_polygon(&pcube->edge_centers[9],&pcube->edge_centers[2],&pcube->edge_centers[0]);
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[4],&pcube->edge_centers[8]);
                        break;
                        
                        case 0xea:/**/
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[4],&pcube->edge_centers[0]);
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[0],&pcube->edge_centers[3]);
                        aForm->add_polygon(&pcube->edge_centers[0xa],&pcube->edge_centers[2],&pcube->edge_centers[1]);
                        break;
                        
                        case 0xeb:/**/
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[4],&pcube->edge_centers[8]);
                        aForm->add_polygon(&pcube->edge_centers[0xa],&pcube->edge_centers[2],&pcube->edge_centers[1]);
                        break;

                        case 0xec:/**/
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[1],&pcube->edge_centers[3]);
                        aForm->add_polygon(&pcube->edge_centers[1],&pcube->edge_centers[7],&pcube->edge_centers[4]);
                        aForm->add_polygon(&pcube->edge_centers[1],&pcube->edge_centers[4],&pcube->edge_centers[9]);
                        break;
                        
                        case 0xed:/**/
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[4],&pcube->edge_centers[8]);
                        aForm->add_polygon(&pcube->edge_centers[9],&pcube->edge_centers[1],&pcube->edge_centers[0]);
                        break;
                        
                        case 0xee:/**/
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[4],&pcube->edge_centers[0]);
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[0],&pcube->edge_centers[3]);
                        break;
                        
                        case 0xef:/**/
                        aForm->add_polygon(&pcube->edge_centers[7],&pcube->edge_centers[4],&pcube->edge_centers[8]);
                        break; 
                        
                        ///////////
                        
                        case 0xf0:/**/
                        aForm->add_polygon(&pcube->edge_centers[9],&pcube->edge_centers[0xa],&pcube->edge_centers[0xb]);
                        aForm->add_polygon(&pcube->edge_centers[9],&pcube->edge_centers[0xb],&pcube->edge_centers[8]);
                        break;
                        
                        case 0xf1:/**/
                        aForm->add_polygon(&pcube->edge_centers[0xb],&pcube->edge_centers[9],&pcube->edge_centers[0xa]);
                        aForm->add_polygon(&pcube->edge_centers[9],&pcube->edge_centers[0xb],&pcube->edge_centers[3]);
                        aForm->add_polygon(&pcube->edge_centers[9],&pcube->edge_centers[3],&pcube->edge_centers[0]);
                        break;
                        
                        case 0xf2:/**/
                        aForm->add_polygon(&pcube->edge_centers[8],&pcube->edge_centers[0xa],&pcube->edge_centers[0xb]);
                        aForm->add_polygon(&pcube->edge_centers[0xa],&pcube->edge_centers[8],&pcube->edge_centers[0]);
                        aForm->add_polygon(&pcube->edge_centers[0xa],&pcube->edge_centers[0],&pcube->edge_centers[1]);
                        break;
                        
                        case 0xf3:/**/
                        aForm->add_polygon(&pcube->edge_centers[0xa],&pcube->edge_centers[0xb],&pcube->edge_centers[3]);
                        aForm->add_polygon(&pcube->edge_centers[0xa],&pcube->edge_centers[3],&pcube->edge_centers[1]);
                        break;
                        
                        case 0xf4:/**/
                        aForm->add_polygon(&pcube->edge_centers[9],&pcube->edge_centers[0xb],&pcube->edge_centers[8]);
                        aForm->add_polygon(&pcube->edge_centers[0xb],&pcube->edge_centers[9],&pcube->edge_centers[1]);
                        aForm->add_polygon(&pcube->edge_centers[0xb],&pcube->edge_centers[1],&pcube->edge_centers[2]);
                        break;
                        
                        case 0xf5:/**/
                        aForm->add_polygon(&pcube->edge_centers[0],&pcube->edge_centers[9],&pcube->edge_centers[1]);
                        aForm->add_polygon(&pcube->edge_centers[2],&pcube->edge_centers[0xb],&pcube->edge_centers[3]);
                        break;
                        
                        case 0xf6:/**/
                        aForm->add_polygon(&pcube->edge_centers[0xb],&pcube->edge_centers[8],&pcube->edge_centers[0]);
                        aForm->add_polygon(&pcube->edge_centers[0xb],&pcube->edge_centers[0],&pcube->edge_centers[2]);
                        break;
                        
                        case 0xf7:/**/
                        aForm->add_polygon(&pcube->edge_centers[0xb],&pcube->edge_centers[3],&pcube->edge_centers[2]);
                        break;
                        
                        case 0xf8:/**/
                        aForm->add_polygon(&pcube->edge_centers[9],&pcube->edge_centers[0xa],&pcube->edge_centers[8]);
                        aForm->add_polygon(&pcube->edge_centers[8],&pcube->edge_centers[0xa],&pcube->edge_centers[2]);
                        aForm->add_polygon(&pcube->edge_centers[8],&pcube->edge_centers[2],&pcube->edge_centers[3]);
                        break;
                        
                        case 0xf9:/**/
                        aForm->add_polygon(&pcube->edge_centers[9],&pcube->edge_centers[0xa],&pcube->edge_centers[2]);
                        aForm->add_polygon(&pcube->edge_centers[9],&pcube->edge_centers[2],&pcube->edge_centers[0]);
                        break;
                        
                        case 0xfa:/**/
                        aForm->add_polygon(&pcube->edge_centers[1],&pcube->edge_centers[0xa],&pcube->edge_centers[2]);
                        aForm->add_polygon(&pcube->edge_centers[3],&pcube->edge_centers[8],&pcube->edge_centers[0]);
                        break;
                        
                        case 0xfb:/**/
                        aForm->add_polygon(&pcube->edge_centers[0xa],&pcube->edge_centers[2],&pcube->edge_centers[1]);
                        break;
                        
                        case 0xfc:/**/
                        aForm->add_polygon(&pcube->edge_centers[8],&pcube->edge_centers[9],&pcube->edge_centers[1]);
                        aForm->add_polygon(&pcube->edge_centers[8],&pcube->edge_centers[1],&pcube->edge_centers[3]);
                        break;
                         
                        case 0xfd:/**/
                        aForm->add_polygon(&pcube->edge_centers[0],&pcube->edge_centers[9],&pcube->edge_centers[1]);
                        break;
                        
                        case 0xfe:/**/
                        aForm->add_polygon(&pcube->edge_centers[3],&pcube->edge_centers[8],&pcube->edge_centers[0]);
                        break;
                        
                        default:
                            break;
                    }
                }
            }
        }
    }
}

