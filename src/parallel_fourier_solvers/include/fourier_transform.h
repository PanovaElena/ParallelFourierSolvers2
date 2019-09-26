#pragma once
#include "vector3d.h"
#include "my_complex.h"
#include "physical_constants.h"
#include "grid3d.h"
#include "simple_types.h"
#include "fftw.h"

enum Direction {
    None,
    RtoC,
    CtoR
};

class FourierTransformOfGrid {
protected:
    fftw_plan plans[2][3][3];  // RtoC/CtoR, field, coordinate
    vec3<int> globalSize;

public:
    FourierTransformOfGrid() {}
    FourierTransformOfGrid(Grid3d& grid) {
        this->globalSize = grid.sizeReal();
        initialize(grid);
    }
    void initialize(Grid3d& grid) {
        createPlans(grid);
    }

    void fourierTransform(Grid3d& grid, Direction dir);
    void fourierTransform(Grid3d& grid, Field _field, Coordinate _coord, Direction dir);

protected:
    virtual void createPlans(Grid3d& grid);
    void destroyPlans();
    void makeFFT(Array3d<double>& arr1, Array3d<MyComplex>& arr2, vec3<int> N,
        Direction dir, fftw_plan& plan);
};


#ifdef __USE_GLOBAL_FFT__

class FourierMpiTransformOfGrid : public FourierTransformOfGrid {
public:
    FourierMpiTransformOfGrid() {}
    FourierMpiTransformOfGrid(Grid3d& grid, vec3<int> globalSize) {
        this->globalSize = globalSize;
        initialize(grid, globalSize);
    }
    void initialize(Grid3d& grid, vec3<int> globalSize) {
        createPlans(grid);
    }
    void createPlans(Grid3d& grid) override;

private:
    void initialize(Grid3d& grid);
};

#endif __USE_GLOBAL_FFT__