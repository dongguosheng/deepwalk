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
TARGET	=	walk
# ---------------

all:	$(TARGET)

walk: main.cpp Makefile
	$(CC) $(CXXFLAGS) $(INCLUDES) $< -o $@ -L. $(LDFLAGS)

clean:
	-rm -f *.o core.* walk
