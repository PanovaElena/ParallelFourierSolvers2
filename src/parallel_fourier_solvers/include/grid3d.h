#pragma once
#include "vector3d.h"
#include "my_complex.h"
#include "array3d.h"
#include "simple_types.h"
#include "grid_params.h"

template <class T>
struct FieldForGrid : public vec3<Array3d<T>> {

    vec3<T> operator() (int i, int j, int k) {
        return vec3<T>(this->x(i, j, k), this->y(i, j, k), this->z(i, j, k));
    }
    vec3<T> operator() (vec3<int> ind) const {
        int i = ind.x; int j = ind.y; int k = ind.z;
        return vec3<T>(this->x(i, j, k), this->y(i, j, k), this->z(i, j, k));
    }
    void write(int i, int j, int k, vec3<T> val) {
        this->x(i, j, k) = val.x;
        this->y(i, j, k) = val.y;
        this->z(i, j, k) = val.z;
    }
    void clear() {
        this->x.clear();
        this->y.clear();
        this->z.clear();
    }
    void initialize(vec3<int> _n) {
        this->x.initialize(_n);
        this->y.initialize(_n);
        this->z.initialize(_n);
    }
    friend bool operator==(const FieldForGrid& f1, const FieldForGrid& f2) {
        return (f1.x == f2.x && f1.y == f2.y && f1.z == f2.z);
    }
    friend bool operator!=(const FieldForGrid& f1, const FieldForGrid& f2) {
        return !(f1 == f2);
    }
};

class Grid3d
{
private:
    GridParams gridParams;

    void clearGrid() {
        E.clear();
        B.clear();
        J.clear();
        EF.clear();
        BF.clear();
        JF.clear();
    }

    Type state = Double;  // if grid complex or real

public:
    FieldForGrid<double> E;
    FieldForGrid<double> B;
    FieldForGrid<double> J;
    FieldForGrid<MyComplex> EF;
    FieldForGrid<MyComplex> BF;
    FieldForGrid<MyComplex> JF;

public:
    Grid3d() :E(), B(), J(), EF(), BF(), JF() {}
    Grid3d(const Grid3d& gr) {
        initialize(gr.gridParams);
    }
    Grid3d(const GridParams& params) {
        initialize(gridParams);
    }
    ~Grid3d() {
        clearGrid();
    }

    //сравнение только по вещественным полям
    int operator==(const Grid3d& grid2);
    int operator!=(const Grid3d& grid2) {
        return !(*this == grid2);
    }

    Grid3d& operator=(const Grid3d& grid2) {
        if (this != &grid2)
            initialize(grid2.gridParams);
        return *this;
    }

    void initialize(const GridParams& params);
    void setFields();
    void setJ(int iter);

    vec3<int> sizeReal() const {
        return gridParams.n;
    }
    vec3<int> sizeComplex() const {
        return { gridParams.n.x, gridParams.n.y,
            gridParams.n.z / 2 + 1 };
    }

    vec3<double> getStep() const {
        return gridParams.d;
    }
    vec3<double> getStart() const {
        return gridParams.a;
    }
    vec3<double> getEnd() const {
        return gridParams.b();
    }

    vec3<> getCoord(vec3<> node) {
        return gridParams.getCoord(node);
    }
    vec3<int> getNode(vec3<> coord) {
        return gridParams.getNode(coord);
    }
    vec3<int> getNode(vec3<> coord, Field field, Coordinate fieldCoord) {
        return gridParams.getNode(coord, field, fieldCoord);
    }

    GridParams getGridParams() const {
        return gridParams;
    }

    void setShifts(const vec3<vec3<vec3<>>>& shiftSp, const vec3<>& shiftT) {
        gridParams.setShifts(shiftSp, shiftT);
    }

    Direction getLastFourierTransformDirect() {
        switch (state) {
        case Complex:
            return RtoC;
        default:
            return CtoR;
        }
    }

    void setLastFourierTransformDirect(Direction dir) {
        switch (dir) {
        case RtoC:
            state = Complex;
            break;
        default:
            state = Double;
            break;
        }
    }

    Type getState() {
        return state;
    }

    void setState(Type state) {
        this->state = state;
    }

};