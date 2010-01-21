#ifndef MOVE_H_INCLUDED
#define MOVE_H_INCLUDED

#include <iostream>

extern const char * const move_str[6][4];

struct Move
{
    Move() : face(0), turn(0) { };
    Move(int face, int turn) : face(face), turn(turn) { };
    unsigned char face, turn;

    bool operator== (const Move &m) { return face == m.face && turn == m.turn; }
    bool operator!= (const Move &m) { return face != m.face || turn != m.turn; }
};

std::ostream &operator<< (std::ostream &os, const Move &move);
std::istream &operator>> (std::istream &is, Move &move);


#endif /* ndef MOVE_H_INCLUDED */
