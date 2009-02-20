#include "MainWindow.h"
#include "Timing.h"

#include <FL/Fl.h>
#include <FL/Fl_PNG_Image.H>

#include <stdio.h>
#include <assert.h>

#define SYM1 0
#define SYM2 1
#define SYM3 2
#define SYM4 3
#define SYM5 4
#define SYM6 5
#define SYM7 6
#define SYM8 7
#define SYM9 8

Cube gTestCube = { {
    // TOP:
    { SYM1, 0 }, { SYM2, 0 }, { SYM3, 0 },
    { SYM2, 1 }, { SYM5, 0 }, { SYM6, 0 },
    { SYM7, 0 }, { SYM8, 0 }, { SYM9, 0 },
    // FRONT:
    { SYM1, 0 }, { SYM8, 2 }, { SYM3, 0 },
    { SYM6, 2 }, { SYM5, 2 }, { SYM4, 2 },
    { SYM7, 0 }, { SYM4, 1 }, { SYM9, 0 },
    // RIGHT:
    { SYM1, 0 }, { SYM4, 3 }, { SYM3, 0 },
    { SYM6, 2 }, { SYM5, 2 }, { SYM8, 1 },
    { SYM7, 0 }, { SYM4, 1 }, { SYM7, 1 },
    // BACK:
    { SYM1, 0 }, { SYM8, 2 }, { SYM3, 0 },
    { SYM4, 0 }, { SYM5, 2 }, { SYM8, 1 },
    { SYM7, 0 }, { SYM6, 3 }, { SYM3, 3 },
    // LEFT:
    { SYM1, 0 }, { SYM2, 0 }, { SYM3, 0 },
    { SYM2, 1 }, { SYM5, 3 }, { SYM2, 3 },
    { SYM9, 3 }, { SYM4, 1 }, { SYM9, 0 },
    // BOTTOM:
    { SYM1, 0 }, { SYM2, 0 }, { SYM9, 1 },
    { SYM6, 2 }, { SYM5, 1 }, { SYM6, 0 },
    { SYM7, 0 }, { SYM8, 0 }, { SYM9, 0 } } };

void benchmark()
{
    Cube cube = gTestCube;
    double dt = time_now();
    const int num_moves = 1000000;
    for (int n = 0; n < num_moves; ++n) cube.move(n%6);
    dt = time_now() - dt;
    printf( "%d moves in %.3f seconds; %.3f million moves per second\n",
            num_moves, dt, num_moves/dt*1e-6  );
}

int main()
{
    assert(gSolvedCube.seemsValid());
    assert(gTestCube.seemsValid());
    assert(gSolvedCube != gTestCube);

    benchmark();

    Fl_RGB_Image *img = new Fl_PNG_Image("labels.png");

    MainWindow *win = new MainWindow(100, 100, 500, 500, "Cijferkubus", img);

    win->setCube(gTestCube);
    win->end();
    win->show();
    return Fl::run();
}
