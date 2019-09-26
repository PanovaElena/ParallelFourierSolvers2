#pragma once
#include <omp.h>
#include <string>
#include "mpi_worker.h"
#include "mpi_wrapper_3d.h"
#include "test_parallel.h"
#include "running_wave.h"
#include "field_solver.h"
#include "file_writer.h"

class RunningWaveJustParallel : public RunningWave {
public:
    RunningWaveJustParallel() : RunningWave(true) {}

    void setParamsForTest(ParametersForRunningWave p) {
        params = p;
        if (params.dimensionOfOutputData == 2)
            params.fileWriter.setSection(Section(Section::XOZ, Section::center));
        params.startCond.reset(new StartConditionsRunningWave(params.a, params.d, params.dt,
            params.angle, params.lambda, params.fieldSolver));
    }
};

class TestRunningWaveJustParallel : public TestParallel {
    RunningWaveJustParallel runningWave;
    MPIWorker& worker;

public:
    void setParamsForTest(const ParametersForRunningWave& p) {
        runningWave.setParamsForTest(p);
    }

    TestRunningWaveJustParallel(MPIWorker& _mpiWorker) : runningWave(), worker(_mpiWorker) {
        setNameFiles();
    }

    Status doParallelPart() {

        FileWriter fw(runningWave.params.fileWriter.getDir(), runningWave.params.fileWriter.getField(),
            runningWave.params.fileWriter.getCoord(),
            Section(Section::XOZ, Section::center, Section::XOY, Section::start));

        vec3<int> guard(worker.getMPIWrapper().MPISize().x == 1 ? 0 : runningWave.params.guard.x,
            worker.getMPIWrapper().MPISize().y == 1 ? 0 : runningWave.params.guard.y,
            worker.getMPIWrapper().MPISize().z == 1 ? 0 : runningWave.params.guard.z);
        if (worker.initialize(runningWave.params.n, guard, runningWave.params.mask,
            worker.getMPIWrapper(), *runningWave.params.startCond) == Status::ERROR)
            return Status::ERROR;

        MPIWrapper::showMessage("start par: domain from " + to_string(worker.getMainDomainStart()) + " to " +
           to_string(worker.getMainDomainEnd()) + "; guard is " + to_string(worker.getGuardSize()));

        //MPIWrapper::showMessage("writing to file first domain");
        runningWave.params.fileWriter.write(worker.getGrid(), nameFileAfterDivision);

        if (runningWave.params.filter.state == Filter::on && MPIWrapper::MPIRank() == 0) {
            transformGridIfNecessary(runningWave.params.fieldSolver, worker.getGrid(), RtoC);
            fw.write(worker.getGrid(), "spectrum_after_div.csv", Complex);
            transformGridIfNecessary(runningWave.params.fieldSolver, worker.getGrid(), CtoR);
        }

        double t1 = omp_get_wtime();

        //MPIWrapper::showMessage("parallel field solver");
        parallelScheme(worker, runningWave.params.fieldSolver, runningWave.params.nParSteps,
            runningWave.params.nDomainSteps, runningWave.params.dt, runningWave.params.fileWriter);

        double t2 = omp_get_wtime();
        if (MPIWrapper::MPIRank() == 0)
            std::cout << "Time of parallel version is " << t2 - t1 << std::endl;

        //MPIWrapper::showMessage("writing to file parallel result");
        runningWave.params.fileWriter.write(worker.getGrid(), nameFileAfterExchange);

        return Status::OK;
    }

    virtual Status testBody() {
        if (runningWave.params.nParSteps != 0)
            return doParallelPart();
        return Status::OK;
    }

};