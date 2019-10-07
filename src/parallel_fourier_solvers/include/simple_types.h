#pragma once

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
