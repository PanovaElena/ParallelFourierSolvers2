#pragma once
#include <functional>
#include "vector3d.h"


struct GridParams {
public:
    typedef std::function<vec3<double>(vec3<int>, int, const GridParams&)> FieldFunc;

protected:
    FieldFunc funcE;
    FieldFunc funcB;
    FieldFunc funcJ;

public:

    vec3<double> a;
    vec3<double> d;
    vec3<int> n;

    vec3<double> shiftT;  // [field]
    vec3<vec3<vec3<double>>> shiftSp;  // [field][coord] -> vec3

    GridParams() {}

    GridParams(vec3<double> a, vec3<double> d, vec3<int> n)
    {
        initialize(a, d, n);
    }

    GridParams(vec3<double> a, vec3<double> d, vec3<int> n,
        const FieldFunc& fE, const FieldFunc& fB, const FieldFunc& fJ)
    {
        initialize(a, d, n, fE, fB, fJ);
    }

    GridParams(vec3<double> a, vec3<double> d, vec3<int> n,
        const FieldFunc& fE, const FieldFunc& fB, const FieldFunc& fJ,
        const vec3<vec3<vec3<>>>& shiftSp, const vec3<>& shiftT)
    {
        initialize(a, d, n, fE, fB, fJ, shiftSp, shiftT);
    }

    void initialize(vec3<> a, vec3<> d, vec3<int> n)
    {
        this->a = a;
        this->d = d;
        this->n = n;
        auto lambda = [](vec3<int>, double, const GridParams&) {return vec3<>(0); };
        funcE = lambda;
        funcB = lambda;
        funcJ = lambda;
    }

    void initialize(vec3<> a, vec3<> d, vec3<int> n,
        const FieldFunc& fE, const FieldFunc& fB, const FieldFunc& fJ)
    {
        initialize(a, d, n);
        setFieldFuncs(fE, fB, fJ);
    }

    void initialize(vec3<> a, vec3<> d, vec3<int> n,
        const FieldFunc& fE, const FieldFunc& fB, const FieldFunc& fJ,
        const vec3<vec3<vec3<>>>& shiftSp, const vec3<>& shiftT)
    {
        initialize(a, d, n, fE, fB, fJ);
        setShifts(shiftSp, shiftT);
    }

    void setFieldFuncs(const FieldFunc& fE, const FieldFunc& fB, const FieldFunc& fJ) {
        this->funcE = fE;
        this->funcB = fB;
        this->funcJ = fJ;
    }

    void setShifts(const vec3<vec3<vec3<>>>& shiftSp,
        const vec3<>& shiftT) {
        this->shiftSp = shiftSp;
        this->shiftT = shiftT;
    }

    vec3<> b() const {
        return a + (vec3<>)n * d;
    }

    bool isFieldFuncsSetted() const {
        return funcE != 0 && funcB != 0 && funcJ != 0;
    }

    vec3<> fE(vec3<int> ind, int iter) const {
        return funcE(ind, iter, *this);
    }

    vec3<> fB(vec3<int> ind, int iter) const {
        return funcB(ind, iter, *this);
    }

    vec3<> fJ(vec3<int> ind, int iter) const {
        return funcJ(ind, iter, *this);
    }

    vec3<double> getCoord(vec3<double> node) const {
        return a + node * d;
    }

    vec3<double> getCoord(vec3<int> node, Field field, Coordinate fieldCoord) const {
        vec3<> shift = shiftSp[field][fieldCoord];
        return a + (vec3<>(node) + shift) * d;
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

    double getTime(int iter, double dt, double startTime = 0) const {
        return iter * dt + startTime;
    }

    double getTime(int iter, double dt, double startTime, Field field) const {
        return (iter + shiftT[field])* dt + startTime;
    }
};