## This is a simple Makefile 

# Define what compiler to use and the flags.
CC=cc
CXX=g++
CCFLAGS= -g -std=c++11 -Wall -Werror
LDLIBS= -lm
#all: test_list
all: proj

# Compile all .c files into .o files
# % matches all (like * in a command)
# $< is the source file (.c file)
%.o : %.cpp %.hpp
	$(CXX) -c $(CCFLAGS) $<



###################################
# BEGIN SOLUTION
proj: proj.o 
	$(CXX) -o proj proj.o  $(CCFLAGS) $(LDLIBS)


clean:
	rm -f core *.o proj


