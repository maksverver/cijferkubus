OBJS=Cube.o MainWindow.o Timing.o main.o
BIN=cijferkubus
FLTK_CONFIG=fltk-config --use-gl --use-images

CXXFLAGS=`$(FLTK_CONFIG) --cxxflags` -Wall -Wextra -g -O3
LDFLAGS=`$(FLTK_CONFIG) --ldflags`

$(BIN):	$(OBJS)
	g++ $(LDFLAGS) -o $@ $^

all: $(BIN)

clean:
	rm -f $(OBJS)

distclean: clean
	rm -f $(BIN)

.PHONY: all clean distclean
