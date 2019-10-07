#pragma once
#include <functional>
#include "vector3d.h"

typedef std::function<vec3<double>(vec3<int>, double)> FieldFunc;

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

    GridParams(vec3<double> a, vec3<double> d, vec3<int> n)
    {
        initialize(a, d, n);
    }

    GridParams(vec3<double> a, vec3<double> d, vec3<int> n,
        FieldFunc fE, FieldFunc fB, FieldFunc fJ)
    {
        initialize(a, d, n, fE, fB, fJ);
    }

    GridParams(vec3<double> a, vec3<double> d, vec3<int> n,
        FieldFunc fE, FieldFunc fB, FieldFunc fJ,
        const vec3<vec3<vec3<>>>& shiftSp, const vec3<>& shiftT)
    {
        initialize(a, d, n, fE, fB, fJ, shiftSp, shiftT);
    }

    void initialize(vec3<> a, vec3<> d, vec3<int> n)
    {
        this->a = a;
        this->d = d;
        this->n = n;
    }

    void initialize(vec3<> a, vec3<> d, vec3<int> n,
        FieldFunc fE, FieldFunc fB, FieldFunc fJ)
    {
        initialize(a, d, n);
        setFieldFuncs(fE, fB, fJ);
    }

    void initialize(vec3<> a, vec3<> d, vec3<int> n,
        FieldFunc fE, FieldFunc fB, FieldFunc fJ,
        const vec3<vec3<vec3<>>>& shiftSp, const vec3<>& shiftT)
    {
        initialize(a, d, n, fE, fB, fJ);
        setShifts(shiftSp, shiftT);
    }

    void setFieldFuncs(FieldFunc fE, FieldFunc fB, FieldFunc fJ) {
        this->fE = fE;
        this->fB = fB;
        this->fJ = fJ;
    }

    void setShifts(const vec3<vec3<vec3<>>>& shiftSp,
        const vec3<>& shiftT) {
        this->shiftSp = shiftSp;
        this->shiftT = shiftT;
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