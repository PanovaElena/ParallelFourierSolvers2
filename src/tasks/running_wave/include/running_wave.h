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
#include "grid_params.h"

struct ParametersForRunningWave : public ParallelTaskParameters {

    // physical parameters
    double lambda;
    double angle;

    // output
    int dimensionOfOutputData;

    ParametersForRunningWave() {
        setDefaultValues();
    }

    void setDefaultValues() {
        ParallelTaskParameters::setDefaultValues();
        vec3<int> n(64, 1, 64);
        vec3<> a(0), d(1);
        FieldFunc fE, fB, fJ;
        setFieldFuncs(fE, fB, fJ);
        gridParams.initialize(a, d, n, fE, fB, fJ,
            fieldSolver->getSpatialShift(), fieldSolver->getTimeShift());

        guard = vec3<int>(16, 0, 16);
        dt = 1.0 / (constants::c * 4);  // < COURANT_CONDITION_PSTD
        nSeqSteps = 200;
        nParSteps = 600;
        nDomainSteps = (int)(0.4*guard.x*d.x / constants::c / dt);
        lambda = 16 * d.x;
        angle = 0;
        dimensionOfOutputData = 1;

        fileWriter.initialize("./", E, y, Section(Section::XOY, Section::center, Section::XOZ, Section::center));
    }

    double f(double x, double z, double t) const {
        double x2 = x * cos(angle) + z * sin(angle);
        return sin(2 * constants::pi / lambda * (x2 - constants::c*t));
    }

    void setFieldFuncs(FieldFunc& fE, FieldFunc& fB, FieldFunc& fJ) {
        fE = [this](vec3<int> ind, int iter) {
            vec3<double> EyCoord = gridParams.getCoord(ind, E, y);
            double tE = gridParams.shiftT[E] * dt;
            return vec3<double>(0, f(EyCoord.x, EyCoord.z, tE), 0);
        };

        fB = [this](vec3<int> ind, int iter) {
            vec3<double> BxCoord = gridParams.getCoord(ind, B, x);
            vec3<double> BzCoord = gridParams.getCoord(ind, B, z);
            double tB = gridParams.shiftT[B] * dt;
            return vec3<double>(-sin(angle)*f(BxCoord.x, BxCoord.z, tB), 0,
                cos(angle)*f(BxCoord.x, BzCoord.x, tB));
        };

        fJ = [this](vec3<int> ind, int iter) {return vec3<>(0); };
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

    Grid3d grid;

    RunningWave(bool commonGrid = true): params() {
        if (commonGrid) initialize();
    }

    void setParamsForTest(const ParametersForRunningWave& p, bool commonGrid = true) {
        params = p;
        if (params.dimensionOfOutputData == 2)
            params.fileWriter.setSection(Section(Section::XOZ, Section::center));
        else if (params.dimensionOfOutputData == 1)
            params.fileWriter.setSection(Section(Section::XOY, Section::center,
                Section::XOZ, Section::center));
        if (commonGrid) initialize();
    }

    void initialize() {
        grid.initialize(params.gridParams);
        params.fieldSolver->initialize(grid);
    }
};
