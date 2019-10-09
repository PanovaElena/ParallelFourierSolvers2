#pragma once
#include "vector3d.h"
#include "my_complex.h"
#include "physical_constants.h"
#include "grid3d.h"
#include "simple_types.h"
#include "fftw3.h"

class FourierTransformOfGrid {
protected:
    fftw_plan plans[2][3][3];  // RtoC/CtoR, field, coordinate
    vec3<int> globalSize;

public:
    FourierTransformOfGrid() {}
    FourierTransformOfGrid(Grid3d& grid) {
        initialize(grid);
    }
    void initialize(Grid3d& grid) {
        this->globalSize = grid.sizeReal();
        createPlans(grid);
        grid.setFields();
    }

    void fourierTransform(Grid3d& grid, Direction dir);
    void fourierTransform(Grid3d& grid, Field _field, Coordinate _coord, Direction dir);

    virtual FourierTransformOfGrid* clone() const {
        return new FourierTransformOfGrid(*this);
    }

protected:
    virtual void createPlans(Grid3d& grid);
    void destroyPlans();
    void makeFFT(Array3d<double>& arr1, Array3d<MyComplex>& arr2, vec3<int> N,
        Direction dir, fftw_plan& plan);
};


class FourierMpiTransformOfGrid : public FourierTransformOfGrid {
public:
    FourierMpiTransformOfGrid() {}
    FourierMpiTransformOfGrid(Grid3d& grid, const vec3<int> globalSize) {
        initialize(grid, globalSize);
    }
    void initialize(Grid3d& grid, const vec3<int> globalSize) {
        this->globalSize = globalSize;
        createPlans(grid);
        grid.setFields();
    }

    FourierTransformOfGrid* clone() const override {
        return new FourierMpiTransformOfGrid(*this);
    }

protected:
    void createPlans(Grid3d& grid) override;

private:
    void initialize(Grid3d& grid);
};