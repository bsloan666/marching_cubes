INCLUDES = lat3d.h vec3d.h 

LIB = libg3d.a

CXXFLAGS = -I /opt/X11/include
LDFLAGS = -L /opt/X11/lib

all:	 objview $(LIB) 

objview : objview.cpp $(INCLUDES) $(LIB)
	 g++ -O3 $(CXXFLAGS) $< -o $@ -lm -lGL -lGLU -lglut -L./ $(LDFLAGS) -lg3d

$(LIB): vec3d.cpp
	 g++ -O3 -c $< -o vec3d.o -fpermissive 
	 ar -r $(LIB) vec3d.o 
	

clean:
	rm -fv objview $(LIB) *.o 

