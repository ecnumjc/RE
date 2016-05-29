CC = g++
GCC = gcc
CFLAGS = -lm -O2 -Wall -funroll-loops -ffast-math
#CFLAGS = -lm -O2 -Wall

all: rcnn

rcnn : ecnn-noada.cpp
	$(CC) $(CFLAGS) $(OPT_DEF) ecnn-noada.cpp -DLINUX -fopenmp -O2 -o rcnn

clean:
	rm -rf *.o rcnn
