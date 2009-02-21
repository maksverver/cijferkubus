#ifndef SOLVER_H_INCLUDED
#define SOLVER_H_INCLUDED

#include "Cube.h"
#include "Move.h"

#include <unordered_map>
#include <vector>
#include <map>

struct CubeHasher
{
    size_t operator() (const Cube &cube) const { return cube.hashCode(); }
};

class Solver
{
public:
    Solver(const Cube &c);
    ~Solver();

    bool solve(std::vector<Move> &solution);

private:
    size_t cubeToId(const Cube &c);
    const Cube &idToCube(size_t n) { return *mIdToCube.at(n); }

private:
    const Cube mInitialCube;
    std::unordered_map<Cube, size_t, CubeHasher> mCubeToId;
    std::vector<const Cube*> mIdToCube;
};

#endif /* ndef SOLVER_H_INCLUDED */
