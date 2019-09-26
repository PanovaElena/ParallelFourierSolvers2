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
#include "start_conditions.h"

class StartConditionsSphericalWave : public StartConditions {
public:

    Source* source;

    StartConditionsSphericalWave() {}
    StartConditionsSphericalWave(vec3<> _a, vec3<> _d, double _dt, const FieldSolver& _fs, Source* _source) :
        StartConditions(_a, _d, _dt, _fs),  source(_source) {}

    void initialize(vec3<> _a, vec3<> _d, double _dt, const FieldSolver& _fs, Source* _source) {
        a = _a; d = _d; dt = _dt; fs = _fs; source = _source;
    }

    virtual vec3<double> fJ(vec3<int> ind, int numIter) const override {
        vec3<vec3<double>> dcJ = fs.getCoordOffset(J);
        double dtJ = fs.getTimeOffset(J);
        vec3<double> coord(getCoord(vec3<>(ind.x + dcJ.z.x, ind.y + dcJ.z.y, ind.z + dcJ.z.z)));
        return vec3<>(0, 0, source->getJ(coord, (numIter + dtJ)*dt));
    }
};

struct ParametersForSphericalWave : public ParallelTaskParameters {

    Source source;

    ParametersForSphericalWave() {
        n.x = 128; n.y = n.x; n.z = 1;
        guard = vec3<int>(32);
        d = vec3<double>(constants::c);
        a = vec3<>(-1 * (vec3<>)(n / 2) * d);
        b = a + (vec3<>)n * d;
        dt = 0.1;
        nSeqSteps = 300;
        nParSteps = 100;
        nDomainSteps = (int)(0.4*guard.x*d.x / constants::c / dt);
        source.time = 16;
        source.omega = 2 * constants::pi / source.time;
        source.omegaEnv = source.omega;
        source.width = vec3<double>(d.x * 8, d.y * 8, d.z / 4);
        source.coord = vec3<double>(0, 0, 0);
        source.startTime = 0;
        startCond.reset(new StartConditionsSphericalWave(a, d, dt, fieldSolver, &source));
        fileWriter.initialize("./", E, z, Section(Section::XOY, Section::center));
    }

    void print() const {
        ParallelTaskParameters::print();
        std::cout <<
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

    Grid3d gr;

    SphericalWave() : params() {
        initialize();
    }

    void setParamsForTest(const ParametersForSphericalWave& p) {
        params = p;
        params.startCond.reset(new StartConditionsSphericalWave(params.a, params.d,
            params.dt, params.fieldSolver, &params.source));
        initialize();
    }

    void initialize() {
        gr = Grid3d(params.n, params.a, params.a + (vec3<double>)params.n*params.d);
        setEB();
    }

    void setJ(int iter) {
        for (int i = 0; i < gr.sizeReal().x; i++)
            for (int j = 0; j < gr.sizeReal().y; j++)
                for (int k = 0; k < gr.sizeReal().z; k++)
                    gr.J.write(i, j, k, params.startCond->fJ({ i,j,k }, iter));
    }

    virtual void setEB() {
        for (int i = 0; i < gr.sizeReal().x; i++)
            for (int j = 0; j < gr.sizeReal().y; j++)
                for (int k = 0; k < gr.sizeReal().z; k++) {
                    gr.E.write(i, j, k, params.startCond->fE({ i,j,k }));
                    gr.B.write(i, j, k, params.startCond->fB({ i,j,k }));
                    gr.J.write(i, j, k, params.startCond->fJ({ i,j,k }, 0));
                }
    }

};

