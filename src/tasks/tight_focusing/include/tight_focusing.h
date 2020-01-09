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

    double D = 2 * pulseLength;

    std::string dir = "./";

    // computing
    double factor = 1.0;
    vec3<int> n_start;
    vec3<int> n_start_strip;
    bool ifStrip = false;
    
    FileWriter fileWriterEx, fileWriterEy, fileWriterEz;

    ParametersForTightFocusing(): n_start(320, 256, 256), n_start_strip(32, 256, 256) {
        setDefaultValues();
    }

    void updateNotStrip() {
        vec3<int> n = (vec3<int>)((vec3<>)n_start * factor);
        vec3<> a(-20e-4, -20e-4, -20e-4), b(20e-4, 20e-4, 20e-4);
        vec3<> d = (b - a) / (vec3<>)n;
        GridParams::FieldFunc fE, fB, fJ;
        setFieldFuncsNotStrip(fE, fB, fJ);
        gridParams.initialize(a, d, n, fE, fB, fJ,
            fieldSolver->getSpatialShift(), fieldSolver->getTimeShift());
    }

    void updateStrip() {
        vec3<int> n = (vec3<int>)((vec3<>)n_start_strip * factor);
        vec3<> a(-19e-4, -20e-4, -20e-4), b(-15e-4, 20e-4, 20e-4);
        vec3<> d = (b - a) / (vec3<>)n;
        GridParams::FieldFunc fE, fB, fJ;
        setFieldFuncsStrip(fE, fB, fJ);
        gridParams.initialize(a, d, n, fE, fB, fJ,
            fieldSolver->getSpatialShift(), fieldSolver->getTimeShift());
    }


    void setDefaultValues() {
        ParallelTaskParameters::setDefaultValues();

        updateNotStrip();

        dt = 0.2*wavelength /constants::c;
        nSeqSteps = 10;//180;
        nParSteps = 0;

        fileWriterEx.initialize(dir, E, x, Section(Section::XOY, Section::center));
        fileWriterEy.initialize(dir, E, y, Section(Section::XOY, Section::center));
        fileWriterEz.initialize(dir, E, z, Section(Section::XOY, Section::center));
    }

    double sign(double x) {
        return (x >= 0? 1.0: -1.0);
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
        double R = sqrt(x * x + y * y + z * z);
        if (R > exclusionRadius) {
            double angle = asin(sqrt(y * y + z * z) / R);
            return (amp / R)*longitudinalFieldVariation(R + constants::c*(t + timeFieldInit))*transverseShape(angle)*(x < 0);
        }
        return 0;
    }


    void setFieldFuncsNotStrip(GridParams::FieldFunc& fE, GridParams::FieldFunc& fB, GridParams::FieldFunc& fJ) {

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


    void setFieldFuncsStrip(GridParams::FieldFunc& fE, GridParams::FieldFunc& fB, GridParams::FieldFunc& fJ) {
        GridParams::FieldFunc fE1, fB1;
        setFieldFuncsNotStrip(fE1, fB1, fJ);

        const int nPeriods = 5;

        fE = [this, nPeriods, fE1](vec3<int> ind, int iter, const GridParams& gridParams) -> vec3<double> {
            vec3<double> res(0, 0, 0);
            double x = gridParams.getCoord(ind, E, Coordinate::x).x,
                y = gridParams.getCoord(ind, E, Coordinate::x).y,
                z = gridParams.getCoord(ind, E, Coordinate::x).z;
            for (int i = 0; i < nPeriods; i++) {
                res = res + fE1(gridParams.getNode(vec3<>(x + i * D, y, z), E, Coordinate::x), iter, gridParams);
            }
            return res;
        };

        fB = [this, nPeriods, fB1](vec3<int> ind, int iter, const GridParams& gridParams) -> vec3<double> {
            vec3<double> res(0, 0, 0);
            double x = gridParams.getCoord(ind, B, Coordinate::x).x,
                y = gridParams.getCoord(ind, B, Coordinate::x).y,
                z = gridParams.getCoord(ind, B, Coordinate::x).z;
            for (int i = 0; i < nPeriods; i++) {
                res = res + fB1(gridParams.getNode(vec3<>(x + i * D, y, z), B, Coordinate::x), iter, gridParams);
            }
            return res;
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
    }

    void setParamsForTest(const ParametersForTightFocusing& p, int allocLocal = -1) {
        params = p;
        initialize(allocLocal);
    }

    void initialize(int allocLocal = -1) {
        grid.initialize(params.gridParams, params.fieldSolver->getIfMpiFFT(), allocLocal);
        params.fieldSolver->initialize(grid);
        grid.setShifts(params.fieldSolver->getSpatialShift(),
            params.fieldSolver->getTimeShift());
    }
};
