#ifndef CUBE_H_INCLUDED
#define CUBE_H_INCLUDED

#include <algorithm>
#include <cstring>

struct Face
{
    unsigned char sym;  /* 0..8 */
    unsigned char rot;  /* 0..3 */

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


    int cmp(const Cube &c) const {
        return std::memcmp(&faces, &c.faces, sizeof(faces)); }
    bool operator== (const Cube &c) const { return cmp(c) == 0; }
    bool operator!= (const Cube &c) const { return cmp(c) != 0; }
    bool operator<  (const Cube &c) const { return cmp(c) <  0; }
    bool operator<= (const Cube &c) const { return cmp(c) <= 0; }
    bool operator>  (const Cube &c) const { return cmp(c) >  0; }
    bool operator>= (const Cube &c) const { return cmp(c) >= 0; }

    bool seemsValid() const;

    void move(int f);
};

extern const Cube gSolvedCube;

#endif /* ndef CUBE_H_INCLUDED */
