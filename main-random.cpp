#include "Cube.h"

#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>

int main()
{
    // Seed RNG
    srand((unsigned)getpid() ^ (unsigned)time_t(NULL) << 16);

    // Generate random cube
    Cube cube = gSolvedCube;
    for (int n = 0; n < 1000000; ++n)
    {
        cube.move(rand()%6, 1 + rand()%3);
    }

    writeCube(std::cout, cube);
}
