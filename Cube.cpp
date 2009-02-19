#include "Cube.h"
#include <stdlib.h>

#define FACE_SOLVED { 0, 0 }, { 1, 0 }, { 2, 0 }, \
                    { 3, 0 }, { 4, 0 }, { 5, 0 }, \
                    { 6, 0 }, { 7, 0 }, { 8, 0 }

extern const Cube gSolvedCube = { { FACE_SOLVED, FACE_SOLVED, FACE_SOLVED,
                                    FACE_SOLVED, FACE_SOLVED, FACE_SOLVED } };

void Cube::verify()
{
    int count[9] = { };
    for (int n = 0; n < num_faces; ++n)
    {
        if (faces[n].sym >= 9) abort();
        if (faces[n].rot >= 4) abort();
        count[faces[n].sym] += 1;
    }
    for (int n = 0; n < 9; ++n) if (count[n] != 6) abort();
}
