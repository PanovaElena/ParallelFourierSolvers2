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
    FieldSolver() {
        setShifts();
    }
    FieldSolver(Grid3d* grid) {
        initialize(grid);
    }
    FieldSolver(const FieldSolver& fs) {
        this->grid = fs.grid;
        this->shiftSp = fs.shiftSp;
        this->shiftT = fs.shiftT;
    }

    virtual FieldSolver* clone() const = 0;

    virtual void initialize(Grid3d* grid) {
        this->grid = grid;
        setShifts();
    }

    void run(double dt) {
        if (grid == 0) {
            std::cout << "try to use field solver without grid" << std::endl;
            return;
        }
        this->operator()(dt);
    }

    virtual void doFourierTransform(Direction dir) {}

	virtual bool getIfMpiFFT() {
		return false;
	}

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

    virtual void setShifts() {
        shiftT[E] = 0.0;
        shiftT[B] = 0.0;
        shiftT[J] = 0.0;
        shiftSp[E] = vec3<vec3<>>(vec3<>(0.0), vec3<>(0.0), vec3<>(0.0));
        shiftSp[B] = vec3<vec3<>>(vec3<>(0.0), vec3<>(0.0), vec3<>(0.0));
        shiftSp[J] = vec3<vec3<>>(vec3<>(0.0), vec3<>(0.0), vec3<>(0.0));
    };

    virtual std::string to_string() = 0;

    vec3<> shiftT;
    vec3<vec3<vec3<>>> shiftSp;
};

class RealFieldSolver : public FieldSolver {
public:
    RealFieldSolver() {}
    RealFieldSolver(Grid3d* grid) {
        initialize(grid);
    }

    void initialize(Grid3d* grid) override {
        FieldSolver::initialize(grid);
    }
};

class FieldSolverFDTD: public RealFieldSolver {
public:
    FieldSolverFDTD() {
        setShifts();
    }
    FieldSolverFDTD(Grid3d* grid){
        initialize(grid);
    }
    FieldSolverFDTD(const FieldSolverFDTD& fs) : RealFieldSolver(fs) {
        setShifts();
    }

    FieldSolver* clone() const override {
        return new FieldSolverFDTD(*this);
    }

    void setShifts() override {
        shiftT[E] = 0.5;
        shiftT[B] = -0.5;
        shiftT[J] = 0.0;
        shiftSp[E] = vec3<vec3<>>(vec3<>(0.5, 0, 0), vec3<>(0, 0.5, 0), vec3<>(0, 0, 0.5));
        shiftSp[B] = vec3<vec3<>>(vec3<>(0, 0.5, 0.5), vec3<>(0.5, 0, 0.5), vec3<>(0.5, 0.5, 0));
        shiftSp[J] = vec3<vec3<>>(vec3<>(0.5, 0, 0), vec3<>(0, 0.5, 0), vec3<>(0, 0, 0.5));
    }

    void initialize(Grid3d* grid) override {
        RealFieldSolver::initialize(grid);
        setShifts();
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

protected:

    std::unique_ptr<FourierTransformOfGrid> fourierTransform;
    bool ifMpi = false;
    vec3<int> globalSize;
	int localStartX;

public:
    FourierFieldSolver() {
        this->fourierTransform.reset(new FourierTransformOfGrid());
    }
    FourierFieldSolver(Grid3d* grid) {
		globalSize = grid->sizeReal();
        initialize(grid);
    }
    FourierFieldSolver(const FourierFieldSolver& fs) {
        this->grid = fs.grid;
        this->fourierTransform.reset(fs.fourierTransform->clone());
		this->globalSize = fs.globalSize;
		this->ifMpi = fs.ifMpi;
		this->localStartX = fs.localStartX;
    }

	bool getIfMpiFFT() override {
		return ifMpi;
	}

    void initialize(Grid3d* grid) override {
        FieldSolver::initialize(grid);
        if (!ifMpi) {
			globalSize = grid->sizeReal();
			fourierTransform.reset(new FourierTransformOfGrid(grid));
		}
		else {
			fourierTransform.reset(new FourierMpiTransformOfGrid(grid, globalSize, localStartX));
		}
    }

    void setGlobalFourierTransform(vec3<int> globalSize, int localStartX) {
        ifMpi = true;
        this->globalSize = globalSize;
		this->localStartX = localStartX;
    }

    static __forceinline vec3<> getFreqVector(vec3<int> ind, vec3<int> n, vec3<double> d) {
        double v1 = (2 * constants::pi*((ind.x <= n.x / 2) ? ind.x : ind.x - n.x)) / (n.x * d.x);
        double v2 = (2 * constants::pi*((ind.y <= n.y / 2) ? ind.y : ind.y - n.y)) / (n.y * d.y);
        double v3 = (2 * constants::pi*((ind.z <= n.z / 2) ? ind.z : ind.z - n.z)) / (n.z * d.z);
        return vec3<>(v1, v2, v3);
    }

    void doFourierTransform(Direction dir) override {
        fourierTransform->fourierTransform(grid, dir);
    }
};

class FieldSolverPSATD : public FourierFieldSolver {
public:
    FieldSolverPSATD() {
        setShifts();
    }
    FieldSolverPSATD(Grid3d* grid) {
        initialize(grid);
    }
    FieldSolverPSATD(const FieldSolverPSATD& fs) : FourierFieldSolver(fs) {
        setShifts();
    }

    FieldSolver* clone() const override {
        return new FieldSolverPSATD(*this);
    }

    void setShifts() override {
        shiftT[E] = 0.0;
        shiftT[B] = 0.0;
        shiftT[J] = 0.5;
        shiftSp[E] = vec3<vec3<>>(vec3<>(0.0), vec3<>(0.0), vec3<>(0.0));
        shiftSp[B] = vec3<vec3<>>(vec3<>(0.0), vec3<>(0.0), vec3<>(0.0));
        shiftSp[J] = vec3<vec3<>>(vec3<>(0.0), vec3<>(0.0), vec3<>(0.0));
    }

    void initialize(Grid3d* grid) override {
        FourierFieldSolver::initialize(grid);
        setShifts();
    }

    std::string to_string() override {
        return "PSATD";
    }

protected:
    void operator() (double dt) override;
};

class FieldSolverPSTD : public FourierFieldSolver {
public:
    FieldSolverPSTD() {
        setShifts();
    }
    FieldSolverPSTD(Grid3d* grid) {
        initialize(grid);
    }
    FieldSolverPSTD(const FieldSolverPSTD& fs) : FourierFieldSolver(fs) {
        setShifts();
    }

    FieldSolver* clone() const override {
        return new FieldSolverPSTD(*this);
    }

    void setShifts() override {
        shiftT[E] = 0.0;
        shiftT[B] = 0.5;
        shiftT[J] = 0.5;
        shiftSp[E] = vec3<vec3<>>(vec3<>(0.0), vec3<>(0.0), vec3<>(0.0));
        shiftSp[B] = vec3<vec3<>>(vec3<>(0.0), vec3<>(0.0), vec3<>(0.0));
        shiftSp[J] = vec3<vec3<>>(vec3<>(0.0), vec3<>(0.0), vec3<>(0.0));
    }

    void initialize(Grid3d* grid) override {
        FourierFieldSolver::initialize(grid);
        setShifts();
    }

    std::string to_string() override {
        return "PSTD";
    }

protected:
    void operator() (double dt) override;
    void refreshE(double dt);
    void refreshB(double dt);
};
