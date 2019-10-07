#pragma once
#include <memory>
#include "fourier_transform.h"
#include "simple_types.h"
#include "grid3d.h"

#define COURANT_CONDITION_PSTD(d) (sqrt(2)*(d)/(constants::c*constants::pi))
#define COURANT_CONDITION_FDTD(d) ((d)/(constants::c*sqrt(2)))

class FieldSolver {
protected:
    std::shared_ptr<Grid3d> grid;

public:
    FieldSolver() {}
    FieldSolver(Grid3d& grid) {
        initialize(grid);
    }

    virtual void initialize(Grid3d& grid) {
        this->grid.reset(&grid);
    }

    virtual void operator() (double dt) = 0;

    virtual void doFourierTransform(Direction dir) {}

    vec3<vec3<>> getSpatialShift(Field field) const {
        return shiftSp[field];
    }

    double getTimeShift(Field field) const {
        return shiftT[field];
    }

    vec3<vec3<vec3<>>> getSpatialShift() const {
        return shiftSp;
    }

    vec3<> getTimeShift() const {
        return shiftT;
    }

    vec3<> shiftT;
    vec3<vec3<vec3<>>> shiftSp;
};

class RealFieldSolver : public FieldSolver {
public:
    RealFieldSolver() {}
    RealFieldSolver(Grid3d& grid) {
        initialize(grid);
    }

    void initialize(Grid3d& grid) override {
        FieldSolver::initialize(grid);
    }
};

class FieldSolverFDTD: public RealFieldSolver {
public:
    FieldSolverFDTD() {}

    FieldSolverFDTD(Grid3d& grid){
        initialize(grid);
    }

    void initialize(Grid3d& grid) override {
        RealFieldSolver::initialize(grid);
        shiftT[B] = -0.5;
        shiftT[E] = 0.5;
        shiftSp[E] = vec3<vec3<>>(vec3<>(0.5, 0, 0), vec3<>(0, 0.5, 0), vec3<>(0, 0, 0.5));
        shiftSp[B] = vec3<vec3<>>(vec3<>(0, 0.5, 0.5), vec3<>(0.5, 0, 0.5), vec3<>(0.5, 0.5, 0));
        shiftSp[J] = vec3<vec3<>>(vec3<>(0.5, 0, 0), vec3<>(0, 0.5, 0), vec3<>(0, 0, 0.5));
    }

    void operator() (double dt) override;
protected:
    void refreshE(double dt);
    void refreshB(double dt);
};

class FourierFieldSolver : public FieldSolver {
    std::shared_ptr<FourierTransformOfGrid> fourierTransform;

public:
    FourierFieldSolver() {}
    FourierFieldSolver(Grid3d& grid) {
        initialize(grid);
    }

    void initialize(Grid3d& grid) override {
        FourierFieldSolver::initialize(grid);
        fourierTransform.reset(new FourierTransformOfGrid(grid));
    }

    void setGlobalFourierTransform(vec3<int> globalSize) {
        fourierTransform.reset(new FourierMpiTransformOfGrid(*grid, globalSize));
    }

    static vec3<MyComplex> getFreqVector(vec3<int> ind, const Grid3d& gr) {
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
    FieldSolverPSATD(Grid3d& grid, bool ifMpiF = false, const vec3<int>* globalSize = 0) {
        initialize(grid);
    }

    void initialize(Grid3d& grid) override {
        FourierFieldSolver::initialize(grid);
        shiftT[J] = 0.5;
    }

    void operator() (double dt) override;
};

class FieldSolverPSTD : public FourierFieldSolver {
public:
    FieldSolverPSTD() {}
    FieldSolverPSTD(Grid3d& grid, bool ifMpiF = false, const vec3<int>* globalSize = 0) {
        initialize(grid);
    }

    void initialize(Grid3d& grid) override {
        FourierFieldSolver::initialize(grid);
        shiftT[B] = 0.5;
        shiftT[J] = 0.5;
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

