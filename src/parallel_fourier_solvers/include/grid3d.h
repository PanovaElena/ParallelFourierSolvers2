#pragma once
#include "vector3d.h"
#include "my_complex.h"
#include "array3d.h"
#include "simple_types.h"
#include "grid_params.h"

template <class T>
struct VectorField: public vec3<Array3d<T>> {
    
    __forceinline vec3<T> operator() (int i, int j, int k) {
        return vec3<T>(this->x(i, j, k), this->y(i, j, k), this->z(i, j, k));
    }
    __forceinline vec3<T> operator() (vec3<int> ind) const {
        int i = ind.x; int j = ind.y; int k = ind.z;
        return vec3<T>(this->x(i, j, k), this->y(i, j, k), this->z(i, j, k));
    }
    __forceinline void write(int i, int j, int k, vec3<T> val) {
        this->x(i, j, k) = val.x;
        this->y(i, j, k) = val.y;
        this->z(i, j, k) = val.z;
    }
    __forceinline void clear() {
        this->x.clear();
        this->y.clear();
        this->z.clear();
    }
    __forceinline void initialize(vec3<int> n) {
        this->x.create(n);
        this->y.create(n);
        this->z.create(n);
    }
    __forceinline void initialize(vec3<int> n, int alloc) {
		this->x.create(n, alloc);
		this->y.create(n, alloc);
		this->z.create(n, alloc);
	}
    template <class U>
    __forceinline void initialize(VectorField<U>& field, vec3<int> n) {
        this->x.createShallowCopy(field.x, n);
        this->y.createShallowCopy(field.y, n);
        this->z.createShallowCopy(field.z, n);
    }
    __forceinline friend bool operator==(const VectorField& f1, const VectorField& f2) {
        return (f1.x == f2.x && f1.y == f2.y && f1.z == f2.z);
    }
    __forceinline friend bool operator!=(const VectorField& f1, const VectorField& f2) {
        return !(f1 == f2);
    }
};

class Grid3d
{
private:
    GridParams gridParams;
    size_t allocLocal;

    void clearGrid();

    Type state = Double;  // if grid complex or real

public:
    VectorField<double> E;
    VectorField<double> B;
    VectorField<double> J;
    VectorField<MyComplex> EF;
    VectorField<MyComplex> BF;
    VectorField<MyComplex> JF;

public:
    Grid3d();
    Grid3d(const Grid3d& gr);
    Grid3d(const GridParams& params);
    Grid3d(const GridParams& params, size_t allocLocal);
    ~Grid3d();

    //сравнение только по вещественным полям
    int operator==(const Grid3d& grid2);
    int operator!=(const Grid3d& grid2) {
        return !(*this == grid2);
    }

    Grid3d& operator=(const Grid3d& grid2);

    void create(const GridParams& params);
    void create(const GridParams& params, size_t allocLocal);
    void setFields();
    void setJ(int iter);

    vec3<int> sizeReal() const;
    vec3<int> sizeComplex() const;

    vec3<double> getStep() const;
    vec3<double> getStart() const;
    vec3<double> getEnd() const;

    vec3<> getCoord(vec3<> node, Field field, Coordinate fieldCoord) {
        return gridParams.getCoord(node, field, fieldCoord);
    }
    vec3<int> getNode(vec3<> coord, Field field, Coordinate fieldCoord) {
        return gridParams.getNode(coord, field, fieldCoord);
    }

    GridParams getGridParams() const {
        return gridParams;
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