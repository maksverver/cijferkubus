#include "Solver.h"
#include "Timing.h"

#include <fstream>
#include <stdio.h>
#include <assert.h>

void benchmark()
{
    Cube cube = gSolvedCube;
    double dt = time_now();
    const int num_moves = 1000000;
    for (int n = 0; n < num_moves; ++n) cube.move(n%6);
    dt = time_now() - dt;
    printf( "%d moves in %.3f seconds; %.3f million moves per second\n",
            num_moves, dt, num_moves/dt*1e-6  );
}

bool write_solution(std::ostream &os, const std::vector<Move> &solution)
{
    if (!(os << solution.size() << '\n')) return false;
    for ( std::vector<Move>::const_iterator it = solution.begin();
          it != solution.end(); ++it )
    {
        if (!(os << *it << '\n')) return false;
    }
    return true;
}

int main(int argc, const char *argv[])
{
    benchmark();

    if (argc != 3)
    {
        printf("Usage: solver <cube-in> <solution-out>\n");
        return 1;
    }

    Cube cube;
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
    Solver solver(cube);
    if (!solver.solve(solution))
    {
        printf("No solution found!\n");
    }
    else
    {
        std::ofstream ofs(argv[2]);
        if (!write_solution(ofs, solution))
        {
            printf("Couldn't write solution to %s!\n", argv[2]);
        }
    }
}
