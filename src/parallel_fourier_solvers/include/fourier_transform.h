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
    FourierTransformOfGrid(Grid3d* grid) {
        initialize(grid, grid->sizeReal());
    }

	void initialize(Grid3d* grid, const vec3<int> globalSize, int localStartX = 0) {
		this->globalSize = globalSize;
		this->localStartX = localStartX;
		createPlans(grid);
		grid->setFields();
	}

    void fourierTransform(Grid3d* grid, Direction dir);
    void fourierTransform(Grid3d* grid, Field _field, Coordinate _coord, Direction dir);

    virtual FourierTransformOfGrid* clone() const {
        return new FourierTransformOfGrid(*this);
    }

	vec3<int> getGlobalIndex(const vec3<int>& ind) {
		return vec3<int>(ind.x + localStartX, ind.y, ind.z);
	}

protected:
    virtual void createPlans(Grid3d* grid);
    void destroyPlans();
    void makeFFT(Array3d<double>& arr1, Array3d<MyComplex>& arr2, vec3<int> N,
        Direction dir, fftw_plan& plan);

	int localStartX = 0;
};


class FourierMpiTransformOfGrid : public FourierTransformOfGrid {
public:
    FourierMpiTransformOfGrid() {}

    FourierMpiTransformOfGrid(Grid3d* grid,
        const vec3<int>& globalSize, int localStartX):
        FourierTransformOfGrid() {
        this->initialize(grid, globalSize, localStartX);
    }

    FourierTransformOfGrid* clone() const override {
        return new FourierMpiTransformOfGrid(*this);
    }

protected:
    void createPlans(Grid3d* grid) override;
};