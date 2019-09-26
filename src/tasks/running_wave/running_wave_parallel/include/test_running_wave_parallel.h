#pragma once
#include <omp.h>
#include "mpi_worker.h"
#include "mpi_wrapper_3d.h"
#include "test_parallel.h"
#include "string"
#include "running_wave.h"
#include "field_solver.h"
#include "file_writer.h"

class TestRunningWaveParallel : public TestParallel {
    RunningWave runningWave;
    MPIWorker& worker;

public:
    void setParamsForTest(ParametersForRunningWave p) {
        runningWave.setParamsForTest(p);
    }


    TestRunningWaveParallel(MPIWorker& _mpiWorker) : runningWave(), worker(_mpiWorker) {
        setNameFiles();
    }

    void doSequentialPart() {
        fourierTransform(runningWave.gr, RtoC);
        for (int i = 0; i < runningWave.params.nSeqSteps; i++) {
            runningWave.params.fieldSolver(runningWave.gr, runningWave.params.dt);
        }
        fourierTransform(runningWave.gr, CtoR);

        //MPIWrapper::showMessage("writing to file first steps");
        if (MPIWrapper::MPIRank() == 0)
            runningWave.params.fileWriter.write(runningWave.gr, nameFileFirstSteps);
    }

    Status doParallelPart() {

        FileWriter fw(runningWave.params.fileWriter.getDir(), runningWave.params.fileWriter.getField(),
            runningWave.params.fileWriter.getCoord(), Section(Section::XOZ, Section::center, Section::XOY, Section::start));

        if (MPIWrapper::MPIRank() == 0) {
            transformGridIfNecessary(runningWave.params.fieldSolver, runningWave.gr, RtoC);
            fw.write(runningWave.gr, "spectrum_before_div.csv", Complex);
            transformGridIfNecessary(runningWave.params.fieldSolver, runningWave.gr, CtoR);
        }

        vec3<int> g(worker.getMPIWrapper().MPISize().x == 1 ? 0 : runningWave.params.guard.x,
            worker.getMPIWrapper().MPISize().y == 1 ? 0 : runningWave.params.guard.y,
            worker.getMPIWrapper().MPISize().z == 1 ? 0 : runningWave.params.guard.z);
        if (worker.initialize(runningWave.gr, g,
            runningWave.params.mask, worker.getMPIWrapper()) == Status::ERROR)
            return Status::ERROR;

        //MPIWrapper::showMessage("start par: domain from " + to_string(worker.getMainDomainStart()) + " to " +
           // to_string(worker.getMainDomainEnd()) + "; guard is " + to_string(worker.getGuardSize()));

        //MPIWrapper::showMessage("writing to file first domain");
        runningWave.params.fileWriter.write(worker.getGrid(), nameFileAfterDivision);

        if (MPIWrapper::MPIRank() == 0) {
            transformGridIfNecessary(runningWave.params.fieldSolver, worker.getGrid(), RtoC);
            fw.write(worker.getGrid(), "spectrum_after_div.csv", Complex);
            transformGridIfNecessary(runningWave.params.fieldSolver, worker.getGrid(), CtoR);
        }

        double t1 = omp_get_wtime();

        //MPIWrapper::showMessage("parallel field solver");
        parallelScheme(worker, runningWave.params.fieldSolver, runningWave.params.nParSteps, runningWave.params.nDomainSteps,
            runningWave.params.dt, runningWave.params.fileWriter);

        double t2 = omp_get_wtime();
        if (MPIWrapper::MPIRank() == 0)
            std::cout << "Time of parallel version is " << t2 - t1 << std::endl;

        //MPIWrapper::showMessage("writing to file parallel result");
        runningWave.params.fileWriter.write(worker.getGrid(), nameFileAfterExchange);

        //MPIWrapper::showMessage("assemble");
        worker.assembleResultsToZeroProcess(runningWave.gr);

        if (MPIWrapper::MPIRank() == 0) {
            transformGridIfNecessary(runningWave.params.fieldSolver, runningWave.gr, RtoC);
            fw.write(runningWave.gr, "spectrum_before_filter.csv", Complex);
            runningWave.params.filter(runningWave.gr);
            fw.write(runningWave.gr, "spectrum_after_filter.csv", Complex);
            transformGridIfNecessary(runningWave.params.fieldSolver, runningWave.gr, CtoR);
        }

        //MPIWrapper::showMessage("writing to file assembled result");
        if (MPIWrapper::MPIRank() == 0)
            runningWave.params.fileWriter.write(runningWave.gr, nameFileSecondSteps);

        return Status::OK;
    }

    virtual Status testBody() {
        if (runningWave.params.nSeqSteps != 0)
            doSequentialPart();
        MPIWrapper::MPIBarrier();
        if (runningWave.params.nParSteps != 0)
            return doParallelPart();
        return Status::OK;
    }

};