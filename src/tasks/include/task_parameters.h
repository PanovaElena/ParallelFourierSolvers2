#pragma once
#include <iostream>
#include <string>
#include <cmath>
#include <memory>
#include "field_solver.h"
#include "filter.h"
#include "mask.h"
#include "vector3d.h"
#include "file_writer.h"
#include "physical_constants.h"
#include "grid_params.h"
#include "parallel_scheme.h"


class TaskParameters {
public:
    // type of field solver
    std::unique_ptr<FieldSolver> fieldSolver;

    // grid
    GridParams gridParams;

    // time step
    double dt;

    // number of time steps
    int nSeqSteps;

    TaskParameters() {
        setDefaultValues();
    }

    TaskParameters(const TaskParameters& t) {
        fieldSolver.reset(t.fieldSolver->clone());
        gridParams = t.gridParams;
        nSeqSteps = t.nSeqSteps;
    }

    TaskParameters& operator=(const TaskParameters& t) {
        fieldSolver.reset(t.fieldSolver->clone());
        gridParams = t.gridParams;
        nSeqSteps = t.nSeqSteps;
        return *this;
    }

    void setDefaultValues() {
        fieldSolver.reset(new FieldSolverPSATD());
        vec3<> a(0), d(constants::c);
        vec3<int> n(32);
        auto lambda = [](vec3<int>, double) {return vec3<>(0); };
        gridParams.initialize(a, d, n, lambda, lambda, lambda,
            fieldSolver->getSpatialShift(), fieldSolver->getTimeShift());
        dt = 0.1;
        nSeqSteps = 0;
    }

    void setFieldSolver(const FieldSolver& fs, Grid3d& grid) {
        fieldSolver.reset(fs.clone());
        grid.setShifts(fieldSolver->getSpatialShift(), fieldSolver->getTimeShift());
        fieldSolver->initialize(grid);
    }

    void print(std::ostream& ost = std::cout) const {
        ost <<
            "field solver = " << fieldSolver->to_string() << "\n" <<
            "dt = " << dt << "\n" <<
            "a = " << gridParams.a << "\n" <<
            "b = " << gridParams.b() << "\n" <<
            "n = " << gridParams.n << "\n" <<
            "d = " << gridParams.d << "\n" <<
            "number of sequential steps = " << nSeqSteps << "\n";
    }

};

class ParallelTaskParameters : public TaskParameters {

public:
    // mask
    std::unique_ptr<Mask> mask;

    // low frequency filter
    std::unique_ptr<Filter> filter;

    // sum or copy
    std::unique_ptr<ParallelScheme> scheme;

    vec3<int> guard;

    vec3<int> numOfProcesses;

    // number of steps
    int nParSteps;  // parallel
    int nDomainSteps;  // between exchanges

    // output
    FileWriter fileWriter;

    ParallelTaskParameters() {
        setDefaultValues();
    }

    ParallelTaskParameters(const ParallelTaskParameters& t) :
        TaskParameters(t) {
        mask.reset(t.mask->clone());
        filter.reset(t.filter->clone());
        scheme.reset(t.scheme->clone());
        numOfProcesses = t.numOfProcesses;
        guard = t.guard;
        nParSteps = t.nParSteps;
        nDomainSteps = t.nDomainSteps;
        fileWriter = t.fileWriter;
    }

    ParallelTaskParameters& operator=(const ParallelTaskParameters& t) {
        fieldSolver.reset(t.fieldSolver->clone());
        gridParams = t.gridParams;
        nSeqSteps = t.nSeqSteps;
        mask.reset(t.mask->clone());
        filter.reset(t.filter->clone());
        scheme.reset(t.scheme->clone());
        numOfProcesses = t.numOfProcesses;
        guard = t.guard;
        nParSteps = t.nParSteps;
        nDomainSteps = t.nDomainSteps;
        fileWriter = t.fileWriter;
        return *this;
    }

    int getNSteps() const {
        return nSeqSteps + nParSteps;
    }

    void setDefaultValues() {
        TaskParameters::setDefaultValues();
        guard = vec3<int>(8);
        mask.reset(new SimpleMask(vec3<int>(4)));
        filter.reset(new LowFreqFilter(vec3<int>(4), vec3<int>(2)));
        filter->turnOff();
        scheme.reset(new ParallelSchemeCopy());
        nParSteps = 0;
        nDomainSteps = 1;
        numOfProcesses = vec3<int>(2, 1, 1);
    }

    void print(std::ostream& ost = std::cout) const {

        int numExchanges = nParSteps / nDomainSteps;
        int numIterBeforeLastExchange = nParSteps % nDomainSteps;

        TaskParameters::print(ost);
        ost <<
            "number of parallel steps = " << nParSteps << "\n" <<
            "number of steps between exchanges = " << nDomainSteps << "\n" <<
            "number of exchanges = " << numExchanges + 1 << "\n" <<
            "number of iterations before last exchange = " << numIterBeforeLastExchange << "\n" <<
            "scheme = " << scheme->to_string() << "\n" <<
            "guard = " << guard << "\n" <<
            "mask = " << mask->to_string() << "\n" <<
            "mask width = " << mask->getMaskWidth() << "\n" <<
            "filter = " << filter->to_string() << "\n" <<
            "filter width = " << filter->getWidth() << "\n" <<
            "number of zero frequences for filter = " << filter->getNumZeroFreq() << "\n" <<
            "number of processes = " << numOfProcesses << "\n";
    }
};