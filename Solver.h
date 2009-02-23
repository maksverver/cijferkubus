#ifndef SOLVER_H_INCLUDED
#define SOLVER_H_INCLUDED

#include "Cube.h"
#include "Move.h"

#include <unordered_map>
#include <vector>
#include <map>

class CubeRef
{
private:
    typedef unsigned int cube_id_t;

    static std::unordered_map<Cube, cube_id_t> mCubeToId;
    static std::vector<const Cube*> mIdToCube;

public:
    CubeRef(const Cube &cube) : cube_id(cubeToId(cube)) { };
    CubeRef(const CubeRef &cr) : cube_id(cr.cube_id) { };

    CubeRef &operator= (const CubeRef &cr) { cube_id = cr.cube_id; return *this; }

    bool operator== (const CubeRef &cr) const { return cube_id == cr.cube_id; }
    bool operator!= (const CubeRef &cr) const { return cube_id != cr.cube_id; }
    bool operator<  (const CubeRef &cr) const { return cube_id <  cr.cube_id; }
    bool operator>  (const CubeRef &cr) const { return cube_id >  cr.cube_id; }
    bool operator<= (const CubeRef &cr) const { return cube_id <= cr.cube_id; }
    bool operator>= (const CubeRef &cr) const { return cube_id >= cr.cube_id; }

    const Cube &operator*()  const { return  idToCube(cube_id); }
    const Cube *operator->() const { return &idToCube(cube_id); }

    size_t hashCode() const { return std::hash<cube_id_t>()(cube_id); }

private:
    static size_t cubeToId(const Cube &c);
    static const Cube &idToCube(size_t n) { return *mIdToCube.at(n); }

private:
    cube_id_t cube_id;
};

namespace std {
    template<> struct hash<Cube> {
        size_t operator()(const Cube &c) const { return c.hashCode(); }
    };
    template<> struct hash<CubeRef> {
        size_t operator()(const CubeRef &cr) const { return cr.hashCode(); }
    };
}

class Solver
{
public:
    Solver(const Cube &c);
    ~Solver();

    bool solve(std::vector<Move> &solution);

private:
    const Cube mInitialCube;
};

#endif /* ndef SOLVER_H_INCLUDED */
