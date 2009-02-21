#include "Cube.h"
#include "MainWindow.h"
#include "Solver.h"
#include "Move.h"
#include "Timing.h"

#include <FL/Fl.h>
#include <FL/Fl_PNG_Image.H>

#include <stdio.h>
#include <assert.h>

#include <fstream>

bool readSolution(std::istream &is, std::vector<Move> &solution)
{
    solution.clear();
    int size;
    if (!(is >> size) || size < 0) return false;
    for (int n = 0; n < size; ++n)
    {
        Move m;
        if (!(is >> m))return false;
        solution.push_back(m);
    }
    return true;
}

int main(int argc, const char *argv[])
{
    Cube cube = gSolvedCube;
    if (argc > 1)
    {
        std::ifstream ifs(argv[1]);
        if (!readCube(ifs, cube))
        {
            printf("Couldn't read cube from %s!\n", argv[1]); 
            return 1;
        }
        if (!cube.seemsValid())
        {
            printf("Initial cube is invalid!\n");
            return 1;
        }
    }

    std::vector<Move> solution;
    if (argc > 2)
    {
        std::ifstream ifs(argv[2]);
        if (!readSolution(ifs, solution))
        {
            printf("Couldn't read solution from %s!\n", argv[2]);
            return 1;
        }
    }

    Fl_RGB_Image *img = new Fl_PNG_Image("labels.png");
    MainWindow *win = new MainWindow(100, 100, 500, 500, "Cijferkubus", img);
    win->size_range(200, 200, 0, 0, 0, 0, 0);
    win->setCube(cube);
    win->addMoves(solution);
    win->end();
    win->show();
    return Fl::run();
}
