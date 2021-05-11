# calls:
# NOTE THAT THIS USED TO USE GCC, but I needed c++ to use std::tuple
CC         = c++
CFLAGS     = -std=c++11 -c -O3 -stdlib=libc++
LDFLAGS    = 
EXECUTABLE = previz

SOURCES    = previz.cpp spaceship.cpp renderConfig.cpp skeleton.cpp motion.cpp displaySkeleton.cpp material.cpp texture.cpp shapes.cpp raytracer.cpp physicsWorld.cpp shader.cpp ray.cpp
OBJECTS    = $(SOURCES:.cpp=.o)

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f *.o previz
