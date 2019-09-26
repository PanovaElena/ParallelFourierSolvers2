#pragma once
#include "vector3d.h"
#include "my_complex.h"
#include "array3d.h"
#include "simple_types.h"

template <class T>
struct FieldForGrid : public vec3<Array3d<T>> {

    vec3<T> operator() (int i, int j, int k) {
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
        this->x.initialize({ _n.x, _n.y, _n.z });
        this->y.initialize({ _n.x, _n.y, _n.z });
        this->z.initialize({ _n.x, _n.y, _n.z });
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
    vec3<int> n;

    vec3<double> a;  // start of working area
    vec3<double> b;  // end of working area

    vec3<double> d;  // step of grid

    void clearGrid();

    Type state = Double;  // if grid complex or real

public:
    FieldForGrid<double> E;
    FieldForGrid<double> B;
    FieldForGrid<double> J;
    FieldForGrid<MyComplex> EF;
    FieldForGrid<MyComplex> BF;
    FieldForGrid<MyComplex> JF;

public:
    Grid3d();
    Grid3d(const Grid3d& gr);
    Grid3d(vec3<int> n, vec3<double> a, vec3<double> b);
    ~Grid3d();
    //сравнение только по вещественным полям
    int operator==(const Grid3d& grid2);

    Grid3d& operator=(const Grid3d& grid2);

    void initialize(vec3<int> n, vec3<double> a, vec3<double> b);

    vec3<int> sizeReal() const;
    vec3<int> sizeComplex() const;

    vec3<double> getStep() const;
    vec3<double> getStart() const;
    vec3<double> getEnd() const;

    vec3<> getCoord(vec3<double> node) {
        return a + (vec3<>)node * d;
    }
    vec3<int> getNode(vec3<double> coord) {
        int x = (int)((coord.x - a.x) / d.x + 0.5);
        int y = (int)((coord.y - a.y) / d.y + 0.5);
        int z = (int)((coord.z - a.z) / d.z + 0.5);
        return vec3<int>(x, y, z);
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

    void setState(Type _state) {
        state = _state;
    }

};