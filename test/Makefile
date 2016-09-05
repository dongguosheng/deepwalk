# Some Macros
# ---------------
# Compiler Name
CC		=	g++
# Compile Flags
CXXFLAGS=	-g -Wall -std=c++11 -fopenmp
# Linker Flags
LDFLAGS	=
# Include 
INCLUDES=	
# Libraries
LIBS	=
# Object Files
OBJS	=	main.o 
# Name of Executable
TARGET	=	test_walk
# ---------------

all:	$(TARGET)

test_walk: main.cpp Makefile
	$(CC) $(CXXFLAGS) $(INCLUDES) $< -o $@ -L.. $(LDFLAGS)

clean:
	-rm -f *.o core.* test_walk
