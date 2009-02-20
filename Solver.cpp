#include "Solver.h"

#include <assert.h>  // debug
#include <stdio.h>  // debug

#include <algorithm>
#include <set>
#include <utility>
#include <queue>

using std::pair;
using std::make_pair;


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
    int base_dist, dist_left, last_move;
    size_t cube_id;
};

bool operator< (const QueueState &a, const QueueState &b)
{
    return a.base_dist + 1000*a.dist_left > b.base_dist + 1000*b.dist_left;
}

bool Solver::solve(std::vector<int> &solution)
{
    solution.clear();

    if (mInitialCube == gSolvedCube) return true;

    std::map<size_t, int> visited;  // cube id -> last move
    std::priority_queue<QueueState> queue;
    QueueState initial = { 0, heuristic(mInitialCube), -1,
                           cubeToId(mInitialCube) };
    queue.push(initial);

    int cnt = 0;
    while (!queue.empty())
    {
        QueueState state = queue.top();
        queue.pop();

        if (++cnt == 10000)
        {
            printf("%4d %4d\n", state.base_dist, state.dist_left);
            cnt = 0;
        }

        if (!visited.insert(make_pair(state.cube_id, state.last_move)).second)
        {
            continue;
        }

        if (state.dist_left == 0)
        {
            printf("Found path of length %d!\n", state.base_dist);
            int last_move = state.last_move;
            Cube cube = idToCube(state.cube_id);
            assert(visited.find(state.cube_id) != visited.end());
            while (last_move != -1)
            {
                int f = last_move/4, x = last_move%4;
                for (int n = 0; n < x; ++n) solution.push_back(f);
                for (int n = x; n < 4; ++n) cube.move(f);
                size_t cube_id = cubeToId(cube);
                assert(visited.find(cube_id) != visited.end());
                last_move = visited[cube_id];
            }
            std::reverse(solution.begin(), solution.end());
            return true;
        }

        for (int n = 0; n < 6; ++n)
        {
            Cube new_cube = idToCube(state.cube_id);
            for (int x = 1; x < 4; ++x)
            {
                new_cube.move(n);
                size_t new_cube_id = cubeToId(new_cube);
                if (visited.find(new_cube_id) == visited.end())
                {
                    QueueState new_state = { state.base_dist + 1,
                        heuristic(new_cube), 4*n + x, new_cube_id };
                    queue.push(new_state);
                }
            }
        }
    }
    return false;
}

size_t Solver::cubeToId(const Cube &c)
{
    pair<std::map<Cube, size_t>::iterator, bool>
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
