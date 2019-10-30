#pragma once
#include <string>
#include <memory>
#include <iostream>
#include "fourier_transform.h"
#include "simple_types.h"
#include "grid3d.h"


#define COURANT_CONDITION_PSTD(d) (sqrt(2)*(d)/(constants::c*constants::pi))
#define COURANT_CONDITION_FDTD(d) ((d)/(constants::c*sqrt(2)))

class FieldSolver {
protected:
    Grid3d* grid = 0;

    virtual void operator() (double dt) = 0;

public:
    FieldSolver() {}
    FieldSolver(Grid3d& grid) {
        initialize(grid);
    }

    virtual FieldSolver* clone() const = 0;

    virtual void initialize(Grid3d& grid) {
        this->grid = &grid;
    }

    void run(double dt) {
        if (grid == 0) {
            std::cout << "try to use field solver without grid" << std::endl;
            return;
        }
        this->operator()(dt);
    }

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

    virtual std::string to_string() = 0;

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

    FieldSolver* clone() const override {
        return new FieldSolverFDTD(*this);
    }

    void initialize(Grid3d& grid) override {
        shiftT[E] = 0.5;
        shiftT[B] = -0.5;
        shiftT[J] = 0.0;
        shiftSp[E] = vec3<vec3<>>(vec3<>(0.5, 0, 0), vec3<>(0, 0.5, 0), vec3<>(0, 0, 0.5));
        shiftSp[B] = vec3<vec3<>>(vec3<>(0, 0.5, 0.5), vec3<>(0.5, 0, 0.5), vec3<>(0.5, 0.5, 0));
        shiftSp[J] = vec3<vec3<>>(vec3<>(0.5, 0, 0), vec3<>(0, 0.5, 0), vec3<>(0, 0, 0.5));
        RealFieldSolver::initialize(grid);
    }

    void operator() (double dt) override;

    std::string to_string() override {
        return "FDTD";
    }

protected:
    void refreshE(double dt);
    void refreshB(double dt);
};

class FourierFieldSolver : public FieldSolver {
    std::unique_ptr<FourierTransformOfGrid> fourierTransform;
    bool ifMpi = false;
    vec3<int> globalSize;

public:
    FourierFieldSolver() {
        this->fourierTransform.reset(new FourierTransformOfGrid());
    }
    FourierFieldSolver(Grid3d& grid) {
        initialize(grid);
    }
    FourierFieldSolver(const FourierFieldSolver& fs) {
        this->grid = fs.grid;
        this->fourierTransform.reset(fs.fourierTransform->clone());
    }

    void initialize(Grid3d& grid) override {
        FieldSolver::initialize(grid);
        if (!ifMpi)
             fourierTransform.reset(new FourierTransformOfGrid(grid));
        else fourierTransform.reset(new FourierMpiTransformOfGrid(grid, globalSize));
    }

    void setGlobalFourierTransform(vec3<int> globalSize) {
        ifMpi = true;
        this->globalSize = globalSize;
    }

    static vec3<MyComplex> getFreqVector(vec3<int> ind, const Grid3d& gr) {
        MyComplex v1 = 2 * constants::pi / (gr.getEnd().x - gr.getStart().x) *
            ((ind.x <= gr.sizeReal().x / 2) ? ind.x : ind.x - gr.sizeReal().x);
        MyComplex v2 = 2 * constants::pi / (gr.getEnd().y - gr.getStart().y) *
            ((ind.y <= gr.sizeReal().y / 2) ? ind.y : ind.y - gr.sizeReal().y);
        MyComplex v3 = 2 * constants::pi / (gr.getEnd().z - gr.getStart().z) *
            ((ind.z <= gr.sizeReal().z / 2) ? ind.z : ind.z - gr.sizeReal().z);
        return vec3<MyComplex>(v1, v2, v3);
    }

    // simple
    static inline double getFreq(int i, int size, double a, double b) {
        return 2 * constants::pi / (b - a) * ((i <= size / 2) ? i : i - size);
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
    FieldSolverPSATD(const FieldSolverPSATD& fs) : FourierFieldSolver(fs) {}

    FieldSolver* clone() const override {
        return new FieldSolverPSATD(*this);
    }

    void initialize(Grid3d& grid) override {
        shiftT[E] = 0.0;
        shiftT[B] = 0.0;
        shiftT[J] = 0.5;
        shiftSp[E] = vec3<vec3<>>(vec3<>(0.0), vec3<>(0.0), vec3<>(0.0));
        shiftSp[B] = vec3<vec3<>>(vec3<>(0.0), vec3<>(0.0), vec3<>(0.0));
        shiftSp[J] = vec3<vec3<>>(vec3<>(0.0), vec3<>(0.0), vec3<>(0.0));
        FourierFieldSolver::initialize(grid);
    }

    std::string to_string() override {
        return "PSATD";
    }

protected:
    void operator() (double dt) override;
};

class FieldSolverPSTD : public FourierFieldSolver {
public:
    FieldSolverPSTD() {}
    FieldSolverPSTD(Grid3d& grid, bool ifMpiF = false, const vec3<int>* globalSize = 0) {
        initialize(grid);
    }
    FieldSolverPSTD(const FieldSolverPSTD& fs) : FourierFieldSolver(fs) {}

    FieldSolver* clone() const override {
        return new FieldSolverPSTD(*this);
    }

    void initialize(Grid3d& grid) override {
        shiftT[E] = 0.0;
        shiftT[B] = 0.5;
        shiftT[J] = 0.5;
        shiftSp[E] = vec3<vec3<>>(vec3<>(0.0), vec3<>(0.0), vec3<>(0.0));
        shiftSp[B] = vec3<vec3<>>(vec3<>(0.0), vec3<>(0.0), vec3<>(0.0));
        shiftSp[J] = vec3<vec3<>>(vec3<>(0.0), vec3<>(0.0), vec3<>(0.0));
        FourierFieldSolver::initialize(grid);
    }

    std::string to_string() override {
        return "PSTD";
    }

protected:
    void operator() (double dt) override;
    void refreshE(double dt);
    void refreshB(double dt);
};
