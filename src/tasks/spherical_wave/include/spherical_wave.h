#pragma once
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include "grid3d.h"
#include "class_member_ptr.h"
#include "file_writer.h"
#include "mask.h"
#include "filter.h"
#include "physical_constants.h"
#include "field_solver.h"
#include "task_parameters.h"
#include "source.h"
#include "grid_params.h"

struct ParametersForSphericalWave : public ParallelTaskParameters {

    // physical parameters
    Source source;

    ParametersForSphericalWave() {
        setDefaultValues();
    }

    void setDefaultValues() {
        ParallelTaskParameters::setDefaultValues();
        vec3<int> n(64, 64, 1);
        vec3<>  d(constants::c), a(-1 * (vec3<>)(n / 2) * d);
        GridParams::FieldFunc fE, fB, fJ;
        setFieldFuncs(fE, fB, fJ);
        gridParams.initialize(a, d, n, fE, fB, fJ,
            fieldSolver->getSpatialShift(), fieldSolver->getTimeShift());

        guard = vec3<int>(16, 16, 0);
        dt = 0.1;  // < COURANT_CONDITION_PSTD
        nSeqSteps = 300;
        nParSteps = 100;
        nDomainSteps = (int)(0.4*guard.x*d.x / constants::c / dt);

        source.time = 16;
        source.omega = 2 * constants::pi / source.time;
        source.omegaEnv = source.omega;
        source.width = vec3<double>(d.x * 2, d.y * 2, d.z / 4);
        source.coord = vec3<double>(0, 0, 0);
        source.startTime = 0;

        fileWriter.initialize("./", E, z, Section(Section::XOY, Section::center));
    }

    void setFieldFuncs(GridParams::FieldFunc& fE, GridParams::FieldFunc& fB, GridParams::FieldFunc& fJ) {
        fE = [this](vec3<int> ind, int iter, const GridParams& gridParams) -> vec3<double> {
            return vec3<>(0);
        };

        fB = [this](vec3<int> ind, int iter, const GridParams& gridParams) -> vec3<double> {
            return vec3<>(0);
        };

        fJ = [this](vec3<int> ind, int iter, const GridParams& gridParams) -> vec3<double> {
            vec3<> coord = gridParams.getCoord(ind, J, z);
            double t = gridParams.getTime(iter, dt, source.startTime, J);
            return vec3<>(0, 0, source.getJ(coord, t));
        };
    }

    void print(std::ostream& ost = std::cout) const {
        ParallelTaskParameters::print(ost);
        ost <<
            "coordinate of source = " << source.coord << "\n" <<
            "omega = " << source.omega << "\n" <<
            "omega of envelope = " << source.omegaEnv << "\n" <<
            "time of working of sourse = " << source.time << "\n" <<
            "width of sourse = " << source.width << "\n" <<
            "start time of sourse = " << source.startTime << "\n" <<
            std::endl;
    }
};

class SphericalWave {
public:

    ParametersForSphericalWave params;

    Grid3d grid;

    SphericalWave(bool commonGrid = true) : params() {
        if (commonGrid) initialize();
    }

    void setParamsForTest(const ParametersForSphericalWave& p, bool commonGrid = true) {
        params = p;
        if (commonGrid) initialize();
    }

    void initialize() {
        grid.initialize(params.gridParams);
        params.fieldSolver->initialize(grid);
        grid.setShifts(params.fieldSolver->getSpatialShift(),
            params.fieldSolver->getTimeShift());
    }
};

