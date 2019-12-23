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

struct ParametersForTightFocusing : public ParallelTaskParameters {

    // physical parameters
    double TW = 1e+12 * 1e+7;
    double GeV = 1.602e-3;

    double wavelength = 1e-4;
    double pulseLength = 2e-4;
    double phase = 0;
    double R0 = 16 * wavelength;
    double totalPower = 50 * TW;

    double F_number = 0.3;
    double edgeSmoothingAngle = 0.1;
    double openingAngle = atan(1.0 / (2.0*F_number));
    double timeFieldInit = -R0 / constants::c;

    double exclusionRadius = 1e-5;
    double amp = sqrt(totalPower*4.0 / (constants::c*(1.0 - cos(openingAngle))));

    // computing
    int factor = 1;
    vec3<int> n_start;
    bool ifStrip = false;
    
    FileWriter fileWriterEx, fileWriterEy, fileWriterEz;

    ParametersForTightFocusing(): n_start(320, 256, 256) {
        setDefaultValues();
    }

    void setDefaultValues() {
        ParallelTaskParameters::setDefaultValues();

        vec3<int> n = n_start * factor;
        vec3<> a(-20e-4, -20e-4, -20e-4), b(20e-4, 20e-4, 20e-4);
        // if (ifStrip)
        vec3<> d = (b - a) / (vec3<>)n;
        GridParams::FieldFunc fE, fB, fJ;
        setFieldFuncs(fE, fB, fJ);
        gridParams.initialize(a, d, n, fE, fB, fJ,
            fieldSolver->getSpatialShift(), fieldSolver->getTimeShift());

        dt = 0.2*wavelength /constants::c;
        nSeqSteps = 180;
        nParSteps = 0;

        fileWriterEx.initialize("./", E, x, Section(Section::XOY, Section::center));
        fileWriterEy.initialize("./", E, y, Section(Section::XOY, Section::center));
        fileWriterEz.initialize("./", E, z, Section(Section::XOY, Section::center));
    }

    double sign(double x) {
        return (x >= 0);
    }

    double block(double x, double xmin, double xmax) {
        return (sign(x - xmin) + sign(xmax - x))*0.5;
    }

    double longitudinalFieldVariation(double x_ct) {
        return sin(2 * constants::pi*x_ct / wavelength + phase) *
            pow(cos(constants::pi*x_ct / pulseLength), 2) *
            block(x_ct, -0.5*pulseLength, 0.5*pulseLength);
    }

    double transverseShape(double angle) {
        return block(angle, -1.0, openingAngle - edgeSmoothingAngle * 0.5) +
            pow(cos(0.5*constants::pi*(angle - openingAngle + edgeSmoothingAngle * 0.5) / edgeSmoothingAngle), 2) *
            block(angle, openingAngle - edgeSmoothingAngle * 0.5, openingAngle + edgeSmoothingAngle * 0.5);
    }

    vec3<> getPolarisation() {
        return vec3<>(0.0, 1.0, 0.0);
    }

    double mask(double x, double y, double z, double t = 0) {
        double R = sqrt(x*x + y * y + z * z);
        if (R > exclusionRadius) {
            double angle = asin(sqrt(y*y + z * z) / R);
            return (amp / R)*longitudinalFieldVariation(R + constants::c*(t + timeFieldInit))*transverseShape(angle)*(x < 0);
        }
        return 0;
    }

    void setFieldFuncs(GridParams::FieldFunc& fE, GridParams::FieldFunc& fB, GridParams::FieldFunc& fJ) {
        fE = [this](vec3<int> ind, int iter, const GridParams& gridParams) -> vec3<double> {
            double x = gridParams.getCoord(ind, E, Coordinate::x).x,
                y = gridParams.getCoord(ind, E, Coordinate::x).y,
                z = gridParams.getCoord(ind, E, Coordinate::x).z;
            vec3<> r(x, y, z);
            vec3<> s1 = vec3<>::cross(getPolarisation(), r);
            s1.normilize();
            vec3<> s0 = vec3<>::cross(r, s1);
            s0.normilize();
            return mask(x, y, z)*s0;
        };

        fB = [this](vec3<int> ind, int iter, const GridParams& gridParams) -> vec3<double> {
            double x = gridParams.getCoord(ind, B, Coordinate::x).x,
                y = gridParams.getCoord(ind, B, Coordinate::x).y,
                z = gridParams.getCoord(ind, B, Coordinate::x).z;
            vec3<> r(x, y, z);
            vec3<> s1 = vec3<>::cross(getPolarisation(), r);
            s1.normilize();
            return mask(x, y, z)*s1;
        };

        fJ = [](vec3<int> ind, int iter, const GridParams& gridParams) -> vec3<double> {
            return vec3<>(0);
        };
    }

    void print(std::ostream& ost = std::cout) const {
        ParallelTaskParameters::print(ost);
    }
};


class TightFocusing {
public:

    ParametersForTightFocusing params;

    Grid3d grid;

    TightFocusing(): params() {
        initialize();
    }

    void setParamsForTest(const ParametersForTightFocusing& p) {
        params = p;
        initialize();
    }

    void initialize() {
        grid.initialize(params.gridParams);
        params.fieldSolver->initialize(grid);
        grid.setShifts(params.fieldSolver->getSpatialShift(),
            params.fieldSolver->getTimeShift());
    }
};
