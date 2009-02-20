#include "Cube.h"
#include <stdio.h>  //debug
#include <assert.h>  //debug

#define FACE_SOLVED { 0, 0 }, { 1, 0 }, { 2, 0 }, \
                    { 3, 0 }, { 4, 0 }, { 5, 0 }, \
                    { 6, 0 }, { 7, 0 }, { 8, 0 }

const int DR[4] = { +1,  0, -1,  0 };
const int DC[4] = {  0, +1,  0, -1 };

/* For each face, for each of the four neighbouring faces, a 4-tuple:
   (face, row, column, direction) */

const int gFaceBorderRotation[6][4][4] = {
    // TOP (0)
    { { 1, 0, 0, 1 },
      { 2, 0, 0, 1 },
      { 3, 0, 0, 1 },
      { 4, 0, 0, 1 } },
    // FRONT (1)
    { { 0, 2, 2, 3 },
      { 4, 0, 2, 0 },
      { 5, 0, 0, 1 },
      { 2, 2, 0, 2 } },
    // RIGHT (2)
    { { 0, 0, 2, 0 },
      { 1, 0, 2, 0 },
      { 5, 0, 2, 0 },
      { 3, 2, 0, 2 } },
    // BACK (3)
    { { 0, 0, 0, 1 },
      { 2, 0, 2, 0 },
      { 5, 2, 2, 3 },
      { 4, 2, 0, 2 } },
    // LEFT (4)
    { { 0, 2, 0, 2 },
      { 3, 0, 2, 0 },
      { 5, 2, 0, 2 },
      { 1, 2, 0, 2 } },
    // BOTTOM (5)
    { { 1, 2, 2, 3 },
      { 4, 2, 2, 3 },
      { 3, 2, 2, 3 },
      { 2, 2, 2, 3 } } };

extern const Cube gSolvedCube = { { FACE_SOLVED, FACE_SOLVED, FACE_SOLVED,
                                    FACE_SOLVED, FACE_SOLVED, FACE_SOLVED } };

bool Cube::seemsValid() const
{
    int count[9] = { };
    for (int n = 0; n < 6; ++n)
    {
        for (int r = 0; r < 3; ++r)
        {
            for (int c = 0; c < 3; ++c)
            {
                const Face &f = face(n, r, c);
                if (f.sym >= 9) return false;
                if (f.rot >= 4) return false;
                if ( f.sym%2 != ((r == 1) ^ (c == 1)) ) return false;
                if ( (f.sym == 4) != (r == 1 && c == 1) ) return false;
                count[f.sym] += 1;
            }
        }
    }
    for (int n = 0; n < 9; ++n) if (count[n] != 6) return false;
    return true;
}

static void rot4(Face &f, Face &g, Face &h, Face &i)
{
    Face old_i = i;
    i = h;
    h = g;
    g = f;
    f = old_i;
}

void Cube::move(int f)
{
    // Rotate faces on top
    rot4(face(f, 0, 0), face(f, 2, 0), face(f, 2, 2), face(f, 0, 2));
    rot4(face(f, 0, 1), face(f, 1, 0), face(f, 2, 1), face(f, 1, 2));
    for (int n = 0; n < 9; ++n) face(f, n).rotate(1);

    // Determine how to rotate faces on sides
    int fid[4], row[4], col[4], dir[4];
    for (int n = 0; n < 4; ++n)
    {
        fid[n] = gFaceBorderRotation[f][n][0];
        row[n] = gFaceBorderRotation[f][n][1];
        col[n] = gFaceBorderRotation[f][n][2];
        dir[n] = gFaceBorderRotation[f][n][3];
    }

    // Rotate faces on sides
    for (int d = 0; d < 3; ++d)
    {
        // DEBUG:
        /*
        printf("step %d:", d);
        for (int n = 0; n < 4; ++n)
        {
            printf(" (%d,%d,%d)", fid[n], row[n], col[n]);
            assert(row[n] >= 0 && row[n] < 3);
            assert(col[n] >= 0 && col[n] < 3);
        }
        printf("\n");
        */

        Face &a = face(fid[0], row[0], col[0]);
        Face &b = face(fid[1], row[1], col[1]);
        Face &c = face(fid[2], row[2], col[2]);
        Face &d = face(fid[3], row[3], col[3]);
        rot4(a, b, c, d);
        a.rotate((dir[0] - dir[3] + 4)%4);
        b.rotate((dir[1] - dir[0] + 4)%4);
        c.rotate((dir[2] - dir[1] + 4)%4);
        d.rotate((dir[3] - dir[2] + 4)%4);
        for (int n = 0; n < 4; ++n)
        {
            row[n] += DR[dir[n]];
            col[n] += DC[dir[n]];
        }
    }
}

bool readCube(std::istream &is, Cube &cube)
{
    for (int n = 0; n < Cube::num_faces; ++n)
    {
        int i;
        if (!(is >> i)) return false;
        int d = i/10;  // digit
        int r = i%10;  // rotation (anticlockwise)
        if (d < 1 || d > 9 || r < 0 || r >= 4) return false;
        cube.face(n).sym = d - 1;
        cube.face(n).rot = r;
    }
    return true;
}

bool writeCube(std::ostream &os, Cube &cube)
{
    for (int n = 0; n < Cube::num_faces; ++n)
    {
        if (n > 0 && n%9 == 0) os << '\n';
        os << 10 + 10*cube.face(n).sym + cube.face(n).rot
           << ((n%3 == 2) ? '\n' : ' ' );
        if (!os) return false;
    }
    return true;
}
