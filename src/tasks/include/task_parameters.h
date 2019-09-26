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
#include "start_conditions.h"

struct TaskParameters {
    // type of field solver
    FieldSolver fieldSolver;

    // grid
    vec3<int> n;
    vec3<double> d;  // step of grid
    vec3<double> a, b;  // begin and end of grid

    // time step
    double dt;

    // number of time steps
    int nSeqSteps;

    // start conditions: E(0), B(0) and J(t)
    std::unique_ptr<StartConditions> startCond;

    TaskParameters(): fieldSolver(PSATD), n(32), a(0),
        d(constants::c), dt(0.1), nSeqSteps(0) {
        b = a + d * (vec3<>)n;
        startCond.reset(new StartConditions(a, d, dt, fieldSolver));
    }

    TaskParameters(const TaskParameters& t) {
        fieldSolver = t.fieldSolver;
        n = t.n; a = t.a; d = t.d; dt = t.dt;
        nSeqSteps = t.nSeqSteps;
        b = a + d * (vec3<>)n;
        startCond.reset(new StartConditions(a, d, dt, fieldSolver));
    }

    TaskParameters& operator=(const TaskParameters& t) {
        fieldSolver = t.fieldSolver;
        n = t.n; a = t.a; d = t.d; dt = t.dt;
        nSeqSteps = t.nSeqSteps;
        b = a + d * (vec3<>)n;
        startCond.reset(new StartConditions(a, d, dt, fieldSolver));
        return *this;
    }

};

struct ParallelTaskParameters : public TaskParameters {

    vec3<int> guard;

    // mask
    Mask mask;

    // low frequency filter
    Filter filter;

    // number of steps
    int nParSteps;  // parallel
    int nDomainSteps;  // between exchanges

    // output
    FileWriter fileWriter;

    ParallelTaskParameters() : guard(4), mask(SimpleMask),
        filter(vec3<int>(4), vec3<int>(2)),
        nParSteps(0), nDomainSteps(1) {
        mask.setMaskWidth(vec3<int>(4));
    }

    int getNSteps() const {
        return nSeqSteps + nParSteps;
    }

    void print(std::ostream& ost = std::cout) const {

        int numExchanges = nParSteps / nDomainSteps;
        int numIterBeforeLastExchange = nParSteps % nDomainSteps;

        ost <<
            "field solver = " << fieldSolver.to_string() << "\n" <<
            "dt = " << dt << "\n" <<
            "a = " << a << "\n" <<
            "b = " << (a + d * (const vec3<double>)n) << "\n" <<
            "n = " << n << "\n" <<
            "d = " << d << "\n" <<
            "guard = " << guard << "\n" <<
            "mask = " << mask.to_string() << "\n" <<
            "mask width = " << mask.getMaskWidth() << "\n" <<
            "filter = " << filter.to_string() << "\n" <<
            "filter width = " << filter.getWidth() << "\n" <<
            "number of zero frequences for filter = " << filter.getNumZeroFreq() << "\n" <<
            "number of steps = " << getNSteps() << "\n" <<
            "number of sequential steps = " << nSeqSteps << "\n" <<
            "number of parallel steps = " << nParSteps << "\n" <<
            "number of steps between exchanges = " << nDomainSteps << "\n" <<
            "number of exchanges = " << numExchanges + 1 << "\n" <<
            "number of iterations before last exchange = " << numIterBeforeLastExchange << "\n";
    }
};