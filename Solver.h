#ifndef SOLVER_H_INCLUDED
#define SOLVER_H_INCLUDED

#include "Cube.h"

#include <vector>
#include <map>

class Solver
{
public:
    Solver(const Cube &c);
    ~Solver();

    bool solve(std::vector<int> &solution);

private:
    size_t cubeToId(const Cube &c);
    const Cube &idToCube(size_t n) { return *mIdToCube.at(n); }

private:
    const Cube mInitialCube;
    std::map<Cube, size_t> mCubeToId;
    std::vector<const Cube*> mIdToCube;
};

#endif /* ndef SOLVER_H_INCLUDED */
