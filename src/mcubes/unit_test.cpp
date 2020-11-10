//		Blake G. Sloan


#include <stdio.h>
#include "marching.h"
#include <vector>

float sphere_left(g3d::vector v)
{
	 v.x()-=50;
	 v.y()-=35;
	 return   sqrt(v.x() * v.x() + v.y() * v.y() + v.z() * v.z())-35;
}
float sphere_right(g3d::vector v)
{
	 v.x()+=50;
	 v.y()-=35;
	 return   sqrt(v.x() * v.x() + v.y() * v.y() + v.z() * v.z())-35;
}
float sphere_top(g3d::vector v)
{
	 v.y()+=30;
	 return   sqrt(v.x() * v.x() + v.y() * v.y() + v.z() * v.z())-70;
}

float quadratic(g3d::vector v){
    // return v.x() * v.x() * 0.5 - v.y() * v.y() * 0.33 +  v.z() * 0.2 * v.z() - 40.0;
    // return v.x() * v.x() * 0.5 - v.y() * v.y() * 0.33 * v.x()+  v.z() * 0.2 * v.z() - 40.0;
       return   sqrt( sin(v.x() * 0.01) * v.x()  * v.x()  + 
                      cos(v.y() * 0.01) * v.y() * v.y() * 2 + 
                      tan(v.z()* 0.01) * v.z() * v.z())-25;
}

float mixed_test(g3d::vector v)
{
	float spl = sphere_left(v);
	float spr = sphere_right(v);
	float spt = sphere_top(v);

	return spt * spr * spl;
}

int main(int argc, char ** argv){
 
    mcb::mesh *isoForm = new mcb::mesh;

 	int index = 1;
	
	const char *outfname =  argv[index++];
	float grain = atof( argv[index++]);
	int d1,d2;


printf("Marching Cubes\n");
        mcb::marching_cubes(quadratic, 256.f, (int)grain,  isoForm);
printf("Smoothing\n");


	mcb::mesh *smoo = isoForm->generate_normals();

printf("writing 3d file\n");
	smoo->write_obj(outfname, 1.f/256.f);

	printf("done\n");
	return 1;
       
}		

