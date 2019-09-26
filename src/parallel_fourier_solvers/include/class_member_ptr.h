#pragma once
#include "vector3d.h"
#include "grid3d.h"
#include "simple_types.h"
#include "array3d.h"

typedef FieldForGrid<double> Grid3d::* FieldOfGrid;
typedef double vec3<double>::* MemberOfVec;
typedef Array3d<double> FieldForGrid<double>::* MemberOfField;

template<class T>
inline FieldForGrid<T> Grid3d::* getMemberPtrField(Field f) {
    switch (f) {
    case E:
        return &Grid3d::E;
    case B:
        return &Grid3d::B;
    case J:
        return &Grid3d::J;
    default:
        return &Grid3d::E;
    }
}

template<>
inline FieldForGrid<MyComplex> Grid3d::* getMemberPtrField(Field f) {
    switch (f) {
    case E:
        return &Grid3d::EF;
    case B:
        return &Grid3d::BF;
    case J:
        return &Grid3d::JF;
    default:
        return &Grid3d::EF;
    }
}

template<class T>
inline T vec3<T>::* getMemberPtrCoord(Coordinate coord) {
    switch (coord) {
    case x:
        return &vec3<T>::x;
    case y:
        return &vec3<T>::y;
    case z:
        return &vec3<T>::z;
    default:
        return &vec3<T>::x;
    }
}

template<class T>
inline Array3d<T> FieldForGrid<T>::* getMemberPtrFieldCoord(Coordinate coord) {
    return getMemberPtrCoord<Array3d<T>>(coord);
}