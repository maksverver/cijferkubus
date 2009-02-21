#include "Move.h"


std::ostream &operator<< (std::ostream &os, const Move &move)
{
    static const char * const x = "UFRBLD";

    switch (move.turn&3)
    {
    case 0: return os;
    case 1: return os << x[move.face] << '\'';
    case 2: return os << x[move.face] << '2';
    case 3: return os << x[move.face];
    }

    return os;  // never reached
}

std::istream &operator>> (std::istream &is, Move &move)
{
    char ch;
    is >> ch;
    switch (ch)
    {
    case 'U': move.face = 0; break;
    case 'F': move.face = 1; break;
    case 'R': move.face = 2; break;
    case 'B': move.face = 3; break;
    case 'L': move.face = 4; break;
    case 'D': move.face = 5; break;
    default:
        is.setstate(std::istream::failbit);
        return is;
    }

    switch ((ch = is.peek()))
    {
    case '\'':
    case '1':
        is.get();
        move.turn = 1;
        break;
    case '2':
        is.get();
        move.turn = 2;
        break;
    case '3':
        is.get();
        move.turn = 3;
        break;
    default:
        // Default: move 1 turn clockwise
        move.turn = 3;
        break;
    }

    return is;
}
