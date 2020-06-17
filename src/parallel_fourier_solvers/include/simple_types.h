#pragma once
#include <iostream>

template <class T>
class vec3;

enum Side {
    left,
    right
};

template <class T>
struct Pair {
    T left;
    T right;

    Pair() {}
    Pair(T l, T r) {
        left = l;
        right = r;
    }

    T getElem(Side side) const {
        return (side == Side::left ? left : right);
    }

    T& getElem(Side side) {
        return (side == Side::left ? left : right);
    }
};

enum Type {
    Complex,
    Double
};

enum Direction {
    RtoC,
    CtoR
};

enum Operation {
    sum,
    copy
};

enum Dimension {
    d0,
    d1,
    d2,
    d3
};

enum State {
    on,
    off
};

enum Field {
    E,
    B,
    J
};

enum Coordinate {
    x,
    y,
    z
};

typedef Pair<vec3<int>> Boards;

inline std::string to_string(Field f)
{
    switch (f) {
    case E:
        return "E";
    case B:
        return "B";
    default:
        return "J";
    }
    return "";
}

inline std::string to_string(Coordinate c)
{
    switch (c) {
    case x:
        return "x";
    case y:
        return "y";
    default:
        return "z";
    }
    return "";
}

inline std::string to_string(State state) {
    switch (state) {
    case on:
        return "on";
    default:
        return "off";
    }
}