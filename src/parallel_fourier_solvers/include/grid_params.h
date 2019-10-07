#pragma once
#include "vector3d.h"

typedef vec3<double>(*FieldFunc)(vec3<int>, double);

struct GridParams {
    vec3<double> a;
    vec3<double> d;
    vec3<int> n;

    FieldFunc fE;
    FieldFunc fB;
    FieldFunc fJ;

    vec3<double> shiftT;  // [field]
    vec3<vec3<vec3<double>>> shiftSp;  // [field][coord] -> vec3

    GridParams() {}

    GridParams(vec3<double> a, vec3<double> d, vec3<int> n,
        FieldFunc fE, FieldFunc fB, FieldFunc fJ, const vec3<double>& shiftT,
        const vec3<vec3<vec3<double>>>& shiftSp)
    {
        initialize(a, d, n, fE, fB, fJ, shiftT, shiftSp);
    }

    GridParams(vec3<double> a, vec3<double> d, vec3<int> n,
        FieldFunc fE, FieldFunc fB, FieldFunc fJ)
    {
        initialize(a, d, n, fE, fB, fJ);
    }

    void initialize(vec3<> a, vec3<> d, vec3<int> n,
        FieldFunc fE, FieldFunc fB, FieldFunc fJ, const vec3<double>& shiftT,
        const vec3<vec3<vec3<double>>>& shiftSp)
    {
        initialize(a, d, n, fE, fB, fJ);
        setShifts(shiftT, shiftSp);
    }

    void initialize(vec3<> a, vec3<> d, vec3<int> n,
        FieldFunc fE, FieldFunc fB, FieldFunc fJ)
    {
        this->a = a;
        this->d = d;
        this->n = n;
        this->fE = fE;
        this->fB = fB;
        this->fJ = fJ;
    }

    void setShifts(const vec3<double>& shiftT,
        const vec3<vec3<vec3<double>>>& shiftSp) {
        this->shiftT = shiftT;
        this->shiftSp = shiftSp;
    }

    vec3<> b() const {
        return a + (vec3<>)n * d;
    }

    vec3<double> getCoord(vec3<double> node) const {
        return a + node * d;
    }

    vec3<int> getNode(vec3<double> coord) const {
        int x = (int)((coord.x - a.x) / d.x + 0.5);
        int y = (int)((coord.y - a.y) / d.y + 0.5);
        int z = (int)((coord.z - a.z) / d.z + 0.5);
        return vec3<int>(x, y, z);
    }

    vec3<int> getNode(vec3<> coord, Field field, Coordinate fieldCoord) const {
        vec3<> shift = shiftSp[field][fieldCoord];
        int x = (int)((coord.x - a.x - shift.x) / d.x + 0.5);
        int y = (int)((coord.y - a.y - shift.y) / d.y + 0.5);
        int z = (int)((coord.z - a.z - shift.z) / d.z + 0.5);
        return vec3<int>(x, y, z);
    }
};