SOLVER_OBJS=Cube.o Move.o Solver.o Timing.o main-solver.o
VIEWER_OBJS=Cube.o Move.o Timing.o MainWindow.o main-viewer.o
RANDOM_OBJS=Cube.o main-random.cpp
BINARIES=solver viewer random-cube

FLTK_CONFIG=fltk-config --use-gl --use-images
CXXFLAGS=`$(FLTK_CONFIG) --cxxflags` -Wall -Wextra -std=c++0x -O3 -g

all: $(BINARIES)

solver:	$(SOLVER_OBJS)
	g++ -o $@ $^

viewer:	$(VIEWER_OBJS)
	g++ `$(FLTK_CONFIG) --ldflags` -o $@ $^

random-cube: $(RANDOM_OBJS)
	g++ -o $@ $^

clean:
	rm -f $(SOLVER_OBJS) $(VIEWER_OBJS)

distclean: clean
	rm -f $(BINARIES)

.PHONY: all clean distclean
