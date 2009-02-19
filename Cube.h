#ifndef CUBE_H_INCLUDED
#define CUBE_H_INCLUDED

struct Face
{
    unsigned char sym;  /* 0..8 */
    unsigned char rot;  /* 0..3 */
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

    void verify();
};

extern const Cube gSolvedCube;

#endif /* ndef CUBE_H_INCLUDED */
