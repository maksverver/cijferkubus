#include "Solver.h"

#include <assert.h>  // debug
#include <stdio.h>  // debug

#include <algorithm>
#include <unordered_set>
#include <set>
#include <utility>
#include <queue>

using std::pair;
using std::make_pair;

const int precompute_dist   =    5;  // distance from solution to precompute
const int dist_left_weight  = 1000;  // weight of dist_left factor

std::unordered_map<Cube, CubeRef::cube_id_t> CubeRef::mCubeToId;
std::vector<const Cube*> CubeRef::mIdToCube;

size_t CubeRef::cubeToId(const Cube &c)
{
    pair<std::unordered_map<Cube, cube_id_t>::iterator, bool>
        ins_res = mCubeToId.insert(make_pair(c, 0));

    if (ins_res.second == false)
    {
        // Entry already exists
        return ins_res.first->second;
    }
    else
    {
        // Newly inserted entry; create next id
        size_t id = mIdToCube.size();
        ins_res.first->second = id;
        mIdToCube.push_back(&ins_res.first->first);
        return id;
    }
}


// Estimates the distance to the solved cube
int heuristic(const Cube &c)
{
    int res = 0;
    for (int n = 0; n < Cube::num_faces; ++n)
    {
        res += (c.face(n) != gSolvedCube.face(n));
    }
    return res;
}

Solver::Solver(const Cube &c)
    : mInitialCube(c)
{
}

Solver::~Solver()
{
}

struct QueueState
{
    unsigned short base_dist, dist_left;
    Move last_move;
    CubeRef cube;
};

bool operator< (const QueueState &a, const QueueState &b)
{
    return a.base_dist + dist_left_weight*a.dist_left
         > b.base_dist + dist_left_weight*b.dist_left;
}

bool Solver::solve(std::vector<Move> &solution)
{
    solution.clear();
    if (mInitialCube == gSolvedCube) return true;

    const Move null_move = { 0, 0 };

    std::unordered_map<CubeRef, Move> solvedCubes; // cube -> inverse_move
    {
        std::vector<CubeRef> queue, next_queue;
        queue.push_back(gSolvedCube);
        solvedCubes[queue[0]] = null_move;
        for (int n = 0; n < precompute_dist; ++n)
        {
            next_queue.clear();
            for ( std::vector<CubeRef>::const_iterator it = queue.begin();
                  it != queue.end(); ++it )
            {
                for (Move move = { 0, 0 }; move.face < 6; ++move.face)
                {
                    Cube new_cube = *(*it);
                    for (move.turn = 1; move.turn < 4; ++move.turn)
                    {
                        new_cube.move(move.face, 1);
                        CubeRef new_cube_ref(new_cube);
                        if (solvedCubes.find(new_cube_ref) == solvedCubes.end())
                        {
                            Move inv_move = { move.face, 4 - move.turn };
                            solvedCubes[new_cube_ref] = inv_move;
                            next_queue.push_back(new_cube_ref);
                        }
                    }
                }
            }
            queue.swap(next_queue);
        }
    }

    std::unordered_map<CubeRef, Move> visited;  // cube -> last move
    std::priority_queue<QueueState> queue;
    QueueState initial = { 0, heuristic(mInitialCube), null_move, mInitialCube };
    queue.push(initial);

    while (!queue.empty())
    {
        QueueState state = queue.top();
        queue.pop();

        if (!visited.insert(make_pair(state.cube, state.last_move)).second)
        {
            continue;
        }

        for (Move move = { 0, 0 }; move.face < 6; ++move.face)
        {
            Cube new_cube = *state.cube;
            for (move.turn = 1; move.turn < 4; ++move.turn)
            {
                new_cube.move(move.face, 1);
                CubeRef new_cube_ref(new_cube);
                if (visited.find(new_cube_ref) == visited.end())
                {
                    if (solvedCubes.find(new_cube_ref) != solvedCubes.end())
                    {
                        // Find trace of moves from start to current state
                        Move last_move = state.last_move;
                        Cube cube = *state.cube;
                        assert(visited.find(cube) != visited.end());
                        while (last_move != null_move)
                        {
                            solution.push_back(last_move);
                            cube.move(last_move.face, 4 - last_move.turn);
                            assert(visited.find(cube) != visited.end());
                            last_move = visited[cube];
                        }
                        std::reverse(solution.begin(), solution.end());

                        // Add current move
                        solution.push_back(move);

                        // Add moves to final solved cube
                        {
                            Cube cube = *new_cube_ref;
                            while (cube != gSolvedCube)
                            {
                                Move next_move = solvedCubes[cube];
                                solution.push_back(next_move);
                                cube.move(next_move.face, next_move.turn);
                            }
                        }

                        return true;
                    }

                    QueueState new_state = { state.base_dist + 1,
                        heuristic(new_cube), move, new_cube_ref };
                    queue.push(new_state);
                }
            }
        }
    }
    return false;
}
