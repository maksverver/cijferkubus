#ifndef CUBE_H_INCLUDED
#define CUBE_H_INCLUDED

#include <algorithm>
#include <iostream>
#include <cstring>

struct Face
{
    unsigned char sym : 4;  /* 0..8 */
    unsigned char rot : 4;  /* 0..3 */

    bool operator== (const Face &f) const { return sym == f.sym && rot == f.rot; }
    bool operator!= (const Face &f) const { return sym != f.sym || rot != f.rot; }

    void rotate(unsigned n) { rot = (rot + n)%4; }
};


struct Cube
{
    static const int num_faces = 54;

    Face faces[num_faces];

    Face &face(int n) { return faces[n]; }
    const Face &face(int n) const { return faces[n]; }
    Face &face(int f, int n) { return faces[9*f + n]; }
    const Face &face(int f, int n) const { return faces[9*f + n]; }
    Face &face(int f, int r, int c) { return faces[9*f + 3*r + c]; }
    const Face &face(int f, int r, int c) const { return faces[9*f + 3*r + c]; }

    int cmp(const Cube &c) const { return std::memcmp(&faces, &c.faces, sizeof(faces)); }
    bool operator== (const Cube &c) const { return cmp(c) == 0; }
    bool operator!= (const Cube &c) const { return cmp(c) != 0; }
    bool operator<  (const Cube &c) const { return cmp(c) <  0; }
    bool operator<= (const Cube &c) const { return cmp(c) <= 0; }
    bool operator>  (const Cube &c) const { return cmp(c) >  0; }
    bool operator>= (const Cube &c) const { return cmp(c) >= 0; }

    /* Verifies if the cube is tentatively valid. Checks if each digit occurs
       exactly six times, the fifth symbol occurs only on center faces, odd
       symbols occur only on corner faces and even symbols only on side faces.
    */
    bool seemsValid() const;

    /* Turn face `f` in counter-clockwise direction `t` times.
       (f is between 0 and 6, exclusive) */
    void move(int f, int t = 1);

    size_t hashCode() const;
};

bool readCube(std::istream &is, Cube &cube);
bool writeCube(std::ostream &os, const Cube &cube);

extern const Cube gSolvedCube;

#endif /* ndef CUBE_H_INCLUDED */
