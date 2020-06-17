#pragma once
#include <functional>
#include "vector3d.h"


struct GridParams {
public:
    typedef std::function<vec3<double>(vec3<double>, double)> FieldFunc;

public:
    FieldFunc fE;
    FieldFunc fB;
    FieldFunc fJ;

    vec3<double> a;
    vec3<double> d;
    vec3<int> n;

    vec3<double> shiftT;  // [field]
    vec3<vec3<vec3<double>>> shiftSp;  // [field][coord] -> vec3

    double dt;
    double startTime;

    GridParams() {}

    void setMainParams(vec3<double> a, vec3<double> d, vec3<int> n,
        double dt, double startTime) {
        this->a = a;
        this->d = d;
        this->n = n;
        this->dt = dt;
        this->startTime = startTime;
    }

    void setFieldFuncs(const FieldFunc& fE, const FieldFunc& fB, const FieldFunc& fJ) {
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

    vec3<double> getCoordBase(vec3<double> node) const {
        return a + node * d;
    }

    vec3<double> getCoord(vec3<int> node, Field field, Coordinate fieldCoord) const {
        vec3<> shift = shiftSp[field][fieldCoord];
        return a + (vec3<>(node) + shift) * d;
    }

    vec3<int> getNodeBase(vec3<double> coord) const {
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

    double getTimeBase(int iter, double dt, double startTime = 0) const {
        return iter * dt + startTime;
    }

    double getTime(int iter, Field field, Coordinate fieldCoord) const {
        return (iter + shiftT[field])* dt + startTime;
    }
};