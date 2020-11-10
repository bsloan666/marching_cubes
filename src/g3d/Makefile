INCLUDES = lat3d.h vec3d.h 

LIB = libg3d.a



all:	 objview $(LIB) 

objview : objview.cpp $(INCLUDES) $(LIB)
	 g++ -O3 $< -o $@ -lm -lGL -lGLU -lglut -L./ -lg3d

$(LIB): vec3d.cpp
	 g++ -O3 -c $< -o vec3d.o -fpermissive 
	 ar -r $(LIB) vec3d.o 
	

clean:
	rm -fv objview $(LIB) *.o 

