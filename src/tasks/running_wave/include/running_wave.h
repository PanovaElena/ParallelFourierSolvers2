#pragma once
#include <iostream>
#include <cmath>
#include "grid3d.h"
#include "physical_constants.h"
#include "simple_types.h"
#include "fourier_transform.h"
#include "field_solver.h"
#include "class_member_ptr.h"
#include "file_writer.h"
#include "mask.h"
#include "filter.h"
#include "task_parameters.h"
#include "start_conditions.h"

class StartConditionsRunningWave : public StartConditions {
public:

    double angle, lambda;

    StartConditionsRunningWave() {}
    StartConditionsRunningWave(vec3<> _a, vec3<> _d, double _dt, double _angle,
        double _lambda, FieldSolver& _fs) : StartConditions(_a, _d, _dt, _fs),
        angle(_angle), lambda(_lambda) {}

    void initialize(vec3<> _a, vec3<> _d, double _dt, double _angle,
        double _lambda, FieldSolver& _fs) {
        a = _a; d = _d; dt = _dt; angle = _angle; lambda = _lambda; fs = _fs;
    }

    double f(double x, double z, double t) const {
        double x2 = x * cos(angle) + z * sin(angle);
        return sin(2 * constants::pi / lambda * (x2 - constants::c*t));
    }

    vec3<double> fE(vec3<int> ind) const override {
        double xEy = getCoord(vec3<>(ind.x + fs.shiftE.y.x, 0, 0)).x,
            zEy = getCoord(vec3<>(0, 0, ind.z + fs.shiftE.y.z)).z,
            tE = fs.shiftEt * dt;
        return vec3<double>(0, f(xEy, zEy, tE), 0);
    }

    vec3<double> fB(vec3<int> ind) const override {
        double xBx = getCoord(vec3<>(ind.x + fs.shiftB.x.x, 0, 0)).x,
            zBx = getCoord(vec3<int>(0, 0, ind.z + fs.shiftB.x.z)).z,
            xBz = getCoord(vec3<int>(ind.x + fs.shiftB.z.x, 0, 0)).x,
            zBz = getCoord(vec3<int>(0, 0, ind.z + fs.shiftB.z.z)).z,
            tB = fs.shiftBt * dt;
        return vec3<double>(-sin(angle)*f(xBx, zBx, tB), 0, cos(angle)*f(xBz, zBz, tB));
    }
};

struct ParametersForRunningWave : public ParallelTaskParameters {

    // physical parameters
    double lambda;
    double angle;

    // output
    int dimensionOfOutputData;

    ParametersForRunningWave() {
        n.x = 256; n.y = 1; n.z = n.x;
        guard = vec3<int>(64);
        d = vec3<double>(1);
        a = vec3<double>(0); b = vec3<double>(n.x*d.x);
        b = a + (vec3<double>)n * d;
        dt = 1 / (constants::c * 4);  // COURANT_CONDITION_PSTD(fmin(fmin(d.x, d.y), d.z)) / 2;
        nSeqSteps = 200;
        nParSteps = 600;
        nDomainSteps = (int)(0.4*guard.x*d.x / constants::c / dt);
        lambda = 16 * d.x;
        angle = 0;
        dimensionOfOutputData = 1;
        startCond.reset(new StartConditionsRunningWave(a, d, dt, angle, lambda, fieldSolver));
        fileWriter.initialize("./", E, y, Section(Section::XOY, Section::center, Section::XOZ, Section::center));
    }

    void print(std::ostream& ost = std::cout) const {
        ParallelTaskParameters::print(ost);
        ost <<
            "lambda = " << lambda << "\n" <<
            "angle = " << angle << "\n" <<
            "dimension of output data = " << dimensionOfOutputData << "\n" <<
            std::endl;
    }
};


class RunningWave {
public:

    ParametersForRunningWave params;

    Grid3d gr;

    RunningWave() : params() {
        initialize();
    }

    RunningWave(bool notInitialize) : params() {}

    void setParamsForTest(const ParametersForRunningWave& p) {
        params = p;
        if (params.dimensionOfOutputData == 2)
            params.fileWriter.setSection(Section(Section::XOZ, Section::center));
        else if (params.dimensionOfOutputData == 1)
            params.fileWriter.setSection(Section(Section::XOY, Section::center,
                Section::XOZ, Section::center));
        params.startCond.reset(new StartConditionsRunningWave (params.a, params.d,
            params.dt, params.angle, params.lambda, params.fieldSolver));
        initialize();
    }

    void initialize() {
        gr = Grid3d(params.n, params.a, params.a + (vec3<double>)params.n*params.d);
        setEB();
    }

    virtual void setEB() {
        for (int i = 0; i < gr.sizeReal().x; i++)
            for (int j = 0; j < gr.sizeReal().y; j++)
                for (int k = 0; k < gr.sizeReal().z; k++) {
                    gr.E.write(i, j, k, params.startCond->fE({ i,j,k }));
                    gr.B.write(i, j, k, params.startCond->fB({ i,j,k }));
                }
    }
};
