SOLVER_OBJS=Cube.o Move.o Solver.o Timing.o main-solver.o
VIEWER_OBJS=Cube.o Move.o Timing.o MainWindow.o main-viewer.o
BINARIES=solver viewer

FLTK_CONFIG=fltk-config --use-gl --use-images
CXXFLAGS=`$(FLTK_CONFIG) --cxxflags` -Wall -Wextra -g -O3

all: $(BINARIES)

solver:	$(SOLVER_OBJS)
	g++ -o $@ $^

viewer:	$(VIEWER_OBJS)
	g++ `$(FLTK_CONFIG) --ldflags` -o $@ $^


clean:
	rm -f $(SOLVER_OBJS) $(VIEWER_OBJS)

distclean: clean
	rm -f $(BINARIES)

.PHONY: all clean distclean
