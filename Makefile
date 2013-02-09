# MAKEFILE

#-------------------------------------------------------------------------------
# USER SETTINGS
#-------------------------------------------------------------------------------

# Variable that contains the name of the program
PROGRAM := cgRender

OBJ_LIST := cgRender.o
#-------------------------------------------------------------------------------
# END USER SETTINGS

COMPILER := g++

CXXFLAGS := -g -pedantic -std=c++0x -Wall -Wextra -Werror=return-type -Wno-reorder
CFLAGS  = -I/usr/X11R6/include -I. -c
LDFLAGS = -L/usr/X11R6/lib -lglut -lGLU -lGL -lXi -lXmu -lXt -lXext -lX11 -lSM -lICE -lm


#-------------------------------------------------------------------------------
# ACTUAL TARGETS
#-------------------------------------------------------------------------------
PROGRAM: $(OBJ_LIST)
	$(COMPILER) $(OBJ_LIST) -o $(PROGRAM) $(CXXFLAGS) $(LDFLAGS)

cgRender.o: cgRender.h cgRender.cpp
	$(COMPILER) $(CXXFLAGS) $(CFLAGS) cgRender.cpp -o cgRender.o

#-------------------------------------------------------------------------------
# PHONY TARGETS
#-------------------------------------------------------------------------------
.PHONY : clean again all
all: $(PROGRAM)

clean:
	rm -rf $(OBJ_LIST) $(PROGRAM)

again:
	make clean
	make