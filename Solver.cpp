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
    size_t cube_id;
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

    std::unordered_map<size_t, Move> solvedCubes; // cube_id -> inverse_move
    {
        std::vector<size_t> queue, next_queue;
        queue.push_back(cubeToId(gSolvedCube));
        solvedCubes[queue[0]] = null_move;
        for (int n = 0; n < precompute_dist; ++n)
        {
            next_queue.clear();
            for ( std::vector<size_t>::const_iterator it = queue.begin();
                  it != queue.end(); ++it )
            {
                size_t cube_id = *it;
                for (Move move = { 0, 0 }; move.face < 6; ++move.face)
                {
                    Cube new_cube = idToCube(cube_id);
                    for (move.turn = 1; move.turn < 4; ++move.turn)
                    {
                        new_cube.move(move.face, 1);
                        size_t new_cube_id = cubeToId(new_cube);
                        if (solvedCubes.find(new_cube_id) == solvedCubes.end())
                        {
                            Move inv_move = { move.face, 4 - move.turn };
                            solvedCubes[new_cube_id] = inv_move;
                            next_queue.push_back(new_cube_id);
                        }
                    }
                }
            }
            queue.swap(next_queue);
        }
    }

    std::unordered_map<size_t, Move> visited;  // cube id -> last move
    std::priority_queue<QueueState> queue;
    QueueState initial = { 0, heuristic(mInitialCube),
                           null_move, cubeToId(mInitialCube) };
    queue.push(initial);

    while (!queue.empty())
    {
        QueueState state = queue.top();
        queue.pop();

        if (!visited.insert(make_pair(state.cube_id, state.last_move)).second)
        {
            continue;
        }

        for (Move move = { 0, 0 }; move.face < 6; ++move.face)
        {
            Cube new_cube = idToCube(state.cube_id);
            for (move.turn = 1; move.turn < 4; ++move.turn)
            {
                new_cube.move(move.face, 1);
                size_t new_cube_id = cubeToId(new_cube);
                if (visited.find(new_cube_id) == visited.end())
                {
                    if (solvedCubes.find(new_cube_id) != solvedCubes.end())
                    {
                        // Find trace of moves from start to current state
                        Move last_move = state.last_move;
                        Cube cube = idToCube(state.cube_id);
                        assert(visited.find(state.cube_id) != visited.end());
                        while (last_move != null_move)
                        {
                            solution.push_back(last_move);
                            cube.move(last_move.face, 4 - last_move.turn);
                            size_t cube_id = cubeToId(cube);
                            assert(visited.find(cube_id) != visited.end());
                            last_move = visited[cube_id];
                        }
                        std::reverse(solution.begin(), solution.end());

                        // Add current move
                        solution.push_back(move);

                        // Add moves to final solved cube
                        {
                            size_t cube_id = new_cube_id;
                            Cube cube = idToCube(cube_id);
                            while (cube != gSolvedCube)
                            {
                                Move next_move = solvedCubes[cube_id];
                                solution.push_back(next_move);
                                cube.move(next_move.face, next_move.turn);
                                cube_id = cubeToId(cube);
                            }
                        }

                        return true;
                    }

                    QueueState new_state = { state.base_dist + 1,
                        heuristic(new_cube), move, new_cube_id };
                    queue.push(new_state);
                }
            }
        }
    }
    return false;
}

size_t Solver::cubeToId(const Cube &c)
{
    pair<std::unordered_map<Cube, size_t>::iterator, bool>
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
