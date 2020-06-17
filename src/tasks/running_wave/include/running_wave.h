#pragma once
#include <cmath>
#include <omp.h>
#include <sstream>
#include "physical_constants.h"
#include "grid_params.h"
#include "file_writer.h"
#include "command_line_parser.h"
#include "mask.h"
#include "filter.h"
#include "parallel_scheme.h"

struct RunningWave {

    // physical parameters
    double lambda;
    double angle;

    RunningWave(double lambda = 1.0, double angle = 0.0) :
        lambda(lambda),
        angle(angle)
    {}

    double f(double x, double z, double t) const {
        double x_ = x*cos(angle) + z*sin(angle);
        return sin(2*constants::pi/lambda*(x_ - constants::c*t));
    }

    GridParams::FieldFunc getfE() {
        return [this](vec3<> coord, double t) -> vec3<> {
            return vec3<>(0, f(coord.x, coord.z, t), 0);
        };
    }

    GridParams::FieldFunc getfB() {
        return [this](vec3<> coord, double t) -> vec3<> {
            return vec3<double>(-sin(angle)*f(coord.x, coord.z, t), 0,
                cos(angle)*f(coord.x, coord.x, t));
        };
    }

    GridParams::FieldFunc getfJ() {
        return [](vec3<> coord, double t) -> vec3<> {
            return vec3<>(0.0);
        };
    }
};

struct RunningWaveTask {

    std::unique_ptr<FieldSolver> fieldSolver;
    int nIter;
    RunningWave rw;
    GridParams gridParams;
    int dimensionOfOutputData;
    FileWriter fileWriter;

    RunningWaveTask() :
        fieldSolver(new FieldSolverPSATD()),
        nIter(1),
        dimensionOfOutputData(1)
    {
        gridParams.setMainParams({0.0, 0.0, 0.0}, {1.0, 1.0, 1.0},
            {64, 16, 64}, 0.25 / constants::c, 0.0);
        rw.lambda = gridParams.b().x - gridParams.a.x;
        gridParams.setShifts(fieldSolver->getSpatialShift(), fieldSolver->getTimeShift());
        gridParams.setFieldFuncs(rw.getfE(), rw.getfB(), rw.getfJ());
        createFileWriter(std::string(ROOT_DIR) + "/", State::off);
    }

    Section getSection() {
        if (dimensionOfOutputData == 2)
            return Section(Section::XOZ, Section::center);
        return Section(Section::XOY, Section::center,
            Section::XOZ, Section::center);
    }

    void createFileWriter(const std::string& dir, State state) {
        fileWriter = FileWriter(dir, getSection(), state);
        fileWriter.pushField(Field::E, Coordinate::y);
    }

};

struct RunningWaveParallelTask : public RunningWaveTask {

    vec3<int> np;
    vec3<> guard;
    std::unique_ptr<ParallelScheme> scheme;
    int nParIter, nIterLocal;

    std::unique_ptr<Mask> mask;
    std::unique_ptr<Filter> filter;

    RunningWaveParallelTask() : RunningWaveTask(),
        np(1, 1, 1),
        guard(0, 0, 0),
        nParIter(0),
        nIterLocal(1)
    {
        scheme.reset(new ParallelSchemeCopy());
        mask.reset(new SimpleMask());
        filter.reset(new LowFreqFilter({ 4, 4, 4 }, { 2, 2, 2 }));
    }

};

struct RunningWaveTaskParser : public CommandLineParser {

    RunningWaveTaskParser() : CommandLineParser({
        {"lambda", "the wavelength"},
        {"angle", "the angle of rotation"},
        {"ax", "the x coordinate of computational area start"},
        {"ay", "the y coordinate of computational area start"},
        {"az", "the z coordinate of computational area start"},
        {"dx", "the spatial step along x axis"},
        {"dy", "the spatial step along y axis"},
        {"dz", "the spatial step along z axis"},
        {"dt", "the time step"},
        {"nx", "the grid size along x axis"},
        {"ny", "the grid size along y axis"},
        {"nz", "the grid size along z axis"},
        {"ni", "the number of iterations"},
        {"solver", "the field solver (FDTD, PSTD, or PSATD)"},
        {"nthreads", "the number of threads"},
        {"dump", "the dump state (on or off)"},
        {"dir", "the dump directory"},
        {"dim", "the dimension of output data (1 or 2)"}
        })
    {}

    Stat parseArgs(int& argc, char**& argv, RunningWaveTask& task) {
        Stat stat = CommandLineParser::parseArgs(argc, argv);
        if (CommandLineParser::parseArgs(argc, argv) != Stat::OK)
            return stat;

        CommandLineParser::getArgVecDouble("ax", "ay", "az", task.gridParams.a);
        CommandLineParser::getArgVecDouble("dx", "dy", "dz", task.gridParams.d);
        CommandLineParser::getArgVecInt("nx", "ny", "nz", task.gridParams.n);
        CommandLineParser::getArgDouble("dt", task.gridParams.dt);
        CommandLineParser::getArgInt("ni", task.nIter);

        CommandLineParser::getArgFieldSolver("solver", task.fieldSolver);
        task.gridParams.setShifts(task.fieldSolver->getSpatialShift(), task.fieldSolver->getTimeShift());

        CommandLineParser::getArgDouble("lambda", task.rw.lambda);
        CommandLineParser::getArgDouble("angle", task.rw.angle);
        task.gridParams.setFieldFuncs(task.rw.getfE(), task.rw.getfB(), task.rw.getfJ());

        int nThreads = 1;
        CommandLineParser::getArgInt("nthreads", nThreads);
        omp_set_num_threads(nThreads);

        std::string dir = task.fileWriter.getDirectory();
        State dump = off;
        CommandLineParser::getArgStr("dir", dir);
        CommandLineParser::getArgInt("dim", task.dimensionOfOutputData);
        CommandLineParser::getArgState("dump", dump);
        task.createFileWriter(dir, dump);

        return Stat::OK;
    }

    void print(RunningWaveTask& task, std::ostream& ost = std::cout) const {
        std::stringstream str;
        str <<
            "lambda: " << task.rw.lambda << "\n" <<
            "angle: " << task.rw.angle << "\n" <<
            "ax: " << task.gridParams.a.x << "\n" <<
            "ay: " << task.gridParams.a.y << "\n" <<
            "az: " << task.gridParams.a.z << "\n" <<
            "dx: " << task.gridParams.d.x << "\n" <<
            "dy: " << task.gridParams.d.y << "\n" <<
            "dz: " << task.gridParams.d.z << "\n" <<
            "dt: " << task.gridParams.dt << "\n" <<
            "nx: " << task.gridParams.n.x << "\n" <<
            "ny: " << task.gridParams.n.y << "\n" <<
            "nz: " << task.gridParams.n.z << "\n" <<
            "ni: " << task.nIter << "\n" <<
            "solver: " << task.fieldSolver->to_string() << "\n" <<
            "nthreads: " << omp_get_max_threads() << "\n" <<
            "dump: " << to_string(task.fileWriter.getState()) << "\n" <<
            "dir: " << task.fileWriter.getDirectory() << "\n" <<
            "dim: " << task.dimensionOfOutputData << "\n";
        ost << str.str();
    }

};

struct RunningWaveParallelTaskParser : public RunningWaveTaskParser {

    std::map<std::string, std::string> parallelParams;

    RunningWaveParallelTaskParser() : RunningWaveTaskParser(), parallelParams({
        {"npx", "the number of processes along x axis"},
        {"npy", "the number of processes along y axis"},
        {"npz", "the number of processes along z axis"},
        {"gx", "the number of guard cells along x axis"},
        {"gy", "the number of guard cells along y axis"},
        {"gz", "the number of guard cells along z axis"},
        {"scheme", "the parallel scheme (sum or copy)"},
        {"npari", "the number of parallel iterations"},
        {"ndomi", "the number of iterations between exchanges"},
        {"mask", "the mask (simple or smooth)"},
        {"mwx", "the smooth mask width along x axis"},
        {"mwy", "the smooth mask width along y axis"},
        {"mwz", "the smooth mask width along z axis"},
        {"filter", "the filter state (on or off)"},
        {"fwx", "the filter width along x axis"},
        {"fwy", "the filter width along y axis"},
        {"fwz", "the filter width along z axis"},
        {"fnzx", "the number of zero frequences in filter along x axis"},
        {"fnzy", "the number of zero frequences in filter along y axis"},
        {"fnzz", "the number of zero frequences in filter along z axis"}
        }) {
        for (auto it = parallelParams.begin(); it != parallelParams.end(); it++) {
            CommandLineParser::params.insert(*it);
        }
    }

    Stat parseArgs(int& argc, char**& argv, RunningWaveParallelTask& task) {
        RunningWaveTaskParser::parseArgs(argc, argv, task);
        CommandLineParser::getArgVecInt("npx", "npy", "npz", task.np);
        CommandLineParser::getArgVecDouble("gx", "gy", "gz", task.guard);
        CommandLineParser::getArgParallelScheme("scheme", task.scheme);
        CommandLineParser::getArgInt("npari", task.nParIter);
        CommandLineParser::getArgInt("ndomi", task.nIterLocal);

        vec3<int> maskWidth = task.mask->getMaskWidth();
        CommandLineParser::getArgMask("mask", task.mask);
        CommandLineParser::getArgVecInt("mwx", "mwy", "mwz", maskWidth);
        task.mask->setMaskWidth(maskWidth);

        vec3<int> filterWidth = task.filter->getWidth();
        vec3<int> nZeroFreq = task.filter->getNumZeroFreq();
        State state = task.filter->getState();
        CommandLineParser::getArgState("filter", state);
        CommandLineParser::getArgVecInt("fwx", "fwy", "fwz", filterWidth);
        CommandLineParser::getArgVecInt("fnzx", "fnzy", "fnzz", nZeroFreq);
        task.filter->setParams(filterWidth, nZeroFreq);
        task.filter->setState(state);
    }

    void print(RunningWaveParallelTask& task, std::ostream& ost = std::cout) const {
        std::stringstream str;
        RunningWaveTaskParser::print(task, str);
        str <<
            "npx: " << task.np.x << "\n" <<
            "npy: " << task.np.y << "\n" <<
            "npz: " << task.np.z << "\n" <<
            "gx: " << task.guard.x << "\n" <<
            "gy: " << task.guard.y << "\n" <<
            "gz: " << task.guard.z << "\n" <<
            "scheme: " << task.scheme->to_string() << "\n" <<
            "npari: " << task.nParIter << "\n" <<
            "ndomi: " << task.nIterLocal << "\n" <<
            "mask: " << task.mask->to_string() << "\n" <<
            "mwx: " << task.mask->getMaskWidth().x << "\n" <<
            "mwy: " << task.mask->getMaskWidth().y << "\n" <<
            "mwz: " << task.mask->getMaskWidth().z << "\n" <<
            "filter: " << task.filter->to_string() << "\n" <<
            "fwx: " << task.filter->getWidth().x << "\n" <<
            "fwy: " << task.filter->getWidth().y << "\n" <<
            "fwz: " << task.filter->getWidth().z << "\n" <<
            "fnzx: " << task.filter->getNumZeroFreq().x << "\n" <<
            "fnzy: " << task.filter->getNumZeroFreq().y << "\n" <<
            "fnzz: " << task.filter->getNumZeroFreq().z << "\n";
        ost << str.str();
    }

};