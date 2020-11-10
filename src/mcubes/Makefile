SOURCE = 	mesh.cpp  \
		marching.cpp \
		polygon.cpp  \
		vertex.cpp \
		perlin_3d.cpp \
		unit_test.cpp

OBJS = 		mesh.o  \
		marching.o \
		polygon.o  \
		perlin_3d.o  \
		vertex.o  

INCLUDES = 	mesh.h  \
		marching.h \
		polygon.h  \
		perlin_3d.h  \
		vertex.h  

CXXFLAGS = -I ../g3d -I ./ -fpermissive --std=c++11 

XLDFLAGS = -L ../g3d -lg3d 

LIB =   libmcubes.a

EXECOBJ = unit_test.o 
	
EXEC = unit_test

all: $(EXEC) $(LIB)

.cpp.o:
	g++ -g $< -o $@ -c $(CXXFLAGS) 

$(LIB): $(OBJS) $(INCLUDES)
	ar -r $@ $(OBJS) 

$(EXEC): $(LIB) $(EXECOBJ) 
	g++ -o $@  $(EXECOBJ) $< -lm $(XLDFLAGS)
 
#$(EXEC): $(SOURCE) 
#	g++ -o $@ $(CXXFLAGS)  $(SOURCE) -lm 

clean:
	rm -fv  $(EXEC) $(OBJS) $(EXECOBJ) $(LIB)

test:
	./unit_test
