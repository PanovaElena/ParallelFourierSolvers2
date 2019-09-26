#pragma once
#include <memory>
#include "fourier_transform.h"
#include "simple_types.h"
#include "grid3d.h"

#define COURANT_CONDITION_PSTD(d) (sqrt(2)*(d)/(constants::c*constants::pi))
#define COURANT_CONDITION_FDTD(d) ((d)/(constants::c*sqrt(2)))

inline int mod(int a, int b) {
    return (a + b) % b;
}

class FieldSolver {
protected:
    std::unique_ptr<Grid3d> grid;
public:
    FieldSolver() {}
    FieldSolver(Grid3d * grid) {
        this->grid.reset(grid);
    }
    virtual void operator() (double dt) = 0;

    virtual void doFourierTransform(Direction dir) {}

    vec3<vec3<>> getCoordOffset(Field field) const {
        switch (field) {
        case E:
            return shiftE;
        case B:
            return shiftB;
        default:
            return shiftJ;
        }
    }

    double getTimeOffset(Field field) const {
        switch (field) {
        case E:
            return shiftEt;
        case B:
            return shiftBt;
        default:
            return shiftJt;
        }
    }

    double shiftEt = 0, shiftBt = 0, shiftJt = 0;
    vec3<vec3<>> shiftE, shiftB, shiftJ;
};

class RealFieldSolver : public FieldSolver {
public:
    RealFieldSolver() {}
    RealFieldSolver(Grid3d * grid) : FieldSolver(grid) {}
};

class FieldSolverFDTD: public RealFieldSolver {
public:
    FieldSolverFDTD() {}
    FieldSolverFDTD(Grid3d * grid) :
        RealFieldSolver(grid) {
        shiftBt = -0.5;
        shiftEt = 0.5;
        shiftE = vec3<vec3<>>(vec3<>(0.5, 0, 0), vec3<>(0, 0.5, 0), vec3<>(0, 0, 0.5));
        shiftB = vec3<vec3<>>(vec3<>(0, 0.5, 0.5), vec3<>(0.5, 0, 0.5), vec3<>(0.5, 0.5, 0));
        shiftJ = vec3<vec3<>>(vec3<>(0.5, 0, 0), vec3<>(0, 0.5, 0), vec3<>(0, 0, 0.5));
    }
    void operator() (double dt) override;
protected:
    void refreshE(double dt);
    void refreshB(double dt);
};

class FourierFieldSolver : public FieldSolver {
    std::unique_ptr<FourierTransformOfGrid> fourierTransform;
public:
    FourierFieldSolver() {}
    FourierFieldSolver(Grid3d * grid, bool ifMpi = false, const vec3<int>* globalSize = 0) {
        initialize(grid, ifMpi, globalSize);
    }
    void initialize(Grid3d * grid, bool ifMpi = false, const vec3<int>* globalSize = 0) {
        this->grid.reset(grid);
        if (ifMpi)
            fourierTransform.reset(new FourierMpiTransformOfGrid(*grid, *globalSize));
        else
            fourierTransform.reset(new FourierTransformOfGrid(*grid));
    }

    vec3<MyComplex> getFreqVector(vec3<int> ind, const Grid3d& gr) {
        MyComplex v1 = (2 * constants::pi*((ind.x <= gr.sizeReal().x / 2) ? ind.x : ind.x - gr.sizeReal().x)) /
            (gr.getEnd().x - gr.getStart().x);
        MyComplex v2 = (2 * constants::pi*((ind.y <= gr.sizeReal().y / 2) ? ind.y : ind.y - gr.sizeReal().y)) /
            (gr.getEnd().y - gr.getStart().y);
        MyComplex v3 = (2 * constants::pi*((ind.z <= gr.sizeReal().z / 2) ? ind.z : ind.z - gr.sizeReal().z)) /
            (gr.getEnd().z - gr.getStart().z);
        return vec3<MyComplex>(v1, v2, v3);
    }

    void doFourierTransform(Direction dir) override {
        fourierTransform->fourierTransform(*grid, dir);
    }
};

class FieldSolverPSATD : public FourierFieldSolver {
public:
    FieldSolverPSATD() {}
    FieldSolverPSATD(Grid3d * grid, bool ifMpi = false, const vec3<int>* globalSize = 0) :
        FourierFieldSolver(grid, ifMpi, globalSize) {
        shiftJt = 0.5;
    }
    void operator() (double dt) override;
};

class FieldSolverPSTD : public FourierFieldSolver {
public:
    FieldSolverPSTD() {}
    FieldSolverPSTD(Grid3d * grid, bool ifMpi = false, const vec3<int>* globalSize = 0) :
        FourierFieldSolver(grid, ifMpi, globalSize) {
        shiftBt = 0.5;
        shiftJt = 0.5;
    }
    void operator() (double dt) override;
protected:
    void refreshE(double dt);
    void refreshB(double dt);
};

//const std::map<std::string, FieldSolver> FieldSolverMap =
//{ { "PSTD",PSTD },{ "PSATD",PSATD },{ "FDTD",FDTD } };
//
//void parallelScheme(MPIWorker& worker, const FieldSolver& fieldSolver, int numIter, int maxNumIterBetweenExchanges,
//    double dt, FileWriter& fileWriter);

