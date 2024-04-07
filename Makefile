## This is a simple Makefile 

# Define what compiler to use and the flags.
CC=cc
CXX=g++
CCFLAGS= -g -std=c++11 -Wall -Werror
LDLIBS= -lm

# All source files
SRCS = proj.cpp proj_func.cpp

# Compile all .cpp files into .o files
# % matches all (like * in a command)
# $< is the source file (.cpp file)
%.o : %.cpp
	$(CXX) -c $(CCFLAGS) $<

# Targets
all: proj 

proj: proj.o proj_func.o
	$(CXX) -o proj proj.o proj_func.o $(CCFLAGS) $(LDLIBS)

clean:
	rm -f core *.o proj

