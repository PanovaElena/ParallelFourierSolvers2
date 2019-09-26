#pragma once
#include <string>
#include "mpi_worker.h"
#include "test_parallel.h"
#include "spherical_wave.h"
#include "field_solver.h"
#include "file_writer.h"

class TestSphericalWaveParallel : public TestParallel {

    SphericalWave sphericalWave;
    MPIWorker& worker;

public:
    void setParamsForTest(const ParametersForSphericalWave& p) {
        sphericalWave.setParamsForTest(p);
    }

    TestSphericalWaveParallel(MPIWorker& _mpiWorker) : sphericalWave(), worker(_mpiWorker) {
        setNameFiles();
    }

    void doSequentialPart() {
        //MPIWrapper::showMessage("do first steps");
        transformGridIfNecessary(sphericalWave.params.fieldSolver, sphericalWave.gr, RtoC);
        for (int i = 0; i < sphericalWave.params.nSeqSteps; i++) {
            transformGridIfNecessary(sphericalWave.params.fieldSolver, sphericalWave.gr, CtoR);
            sphericalWave.setJ(i);
            transformGridIfNecessary(sphericalWave.params.fieldSolver, sphericalWave.gr, RtoC);
            sphericalWave.params.fieldSolver(sphericalWave.gr, sphericalWave.params.dt);
        }
        transformGridIfNecessary(sphericalWave.params.fieldSolver, sphericalWave.gr, CtoR);

        //MPIWrapper::showMessage("writing to file first steps");
        if (MPIWrapper::MPIRank() == 0)
            sphericalWave.params.fileWriter.write(sphericalWave.gr, nameFileFirstSteps);
    }

    void computeParallel(MPIWorker& worker) {
        double startTimeOfSource = sphericalWave.params.source.startTime;
        double endTimeOfSource = sphericalWave.params.source.getEndTime();
        double startTimeOfSeq = 0;
        double endTimeOfSeq = (sphericalWave.params.nSeqSteps - 1) * sphericalWave.params.dt;
        double startTimeOfPar = sphericalWave.params.nSeqSteps * sphericalWave.params.dt;
        double endTimeOfPar = sphericalWave.params.getNSteps() * sphericalWave.params.dt;

        int n1 = 0, n2 = 0;
        if (endTimeOfSource > endTimeOfSeq) {
            n1 = sphericalWave.params.nParSteps;
            n2 = 1;
        }
        else {
            n1 = 1;
            n2 = sphericalWave.params.nParSteps;;
        }

        for (int iter = 0; iter < n1; iter++)
            parallelScheme(worker, sphericalWave.params.fieldSolver, n2,
                sphericalWave.params.nDomainSteps, sphericalWave.params.dt,
                sphericalWave.params.fileWriter);
    }

    Status doParallelPart() {
        //MPIWrapper::showMessage("start init worker");
        vec3<int> g(worker.getMPIWrapper().MPISize().x == 1 ? 0 : sphericalWave.params.guard.x,
            worker.getMPIWrapper().MPISize().y == 1 ? 0 : sphericalWave.params.guard.y,
            worker.getMPIWrapper().MPISize().z == 1 ? 0 : sphericalWave.params.guard.z);
        if (worker.initialize(sphericalWave.gr, g,
            sphericalWave.params.mask, worker.getMPIWrapper()) == Status::ERROR)
            return Status::ERROR;

        //MPIWrapper::showMessage("start par: domain from " + to_string(worker.getMainDomainStart()) + " to " +
            //to_string(worker.getMainDomainEnd()) + "; guard is " + to_string(worker.getGuardSize()));

        //MPIWrapper::showMessage("writing to file first domain");
        sphericalWave.params.fileWriter.write(worker.getGrid(), nameFileAfterDivision);

        //MPIWrapper::showMessage("parallel field solver");
        computeParallel(worker);

        //MPIWrapper::showMessage("writing to file parallel result");
        sphericalWave.params.fileWriter.write(worker.getGrid(), nameFileAfterExchange);

        //MPIWrapper::showMessage("assemble");
        worker.assembleResultsToZeroProcess(sphericalWave.gr);

        if (sphericalWave.params.filter.state == Filter::on && MPIWrapper::MPIRank() == 0) {
            transformGridIfNecessary(sphericalWave.params.fieldSolver, sphericalWave.gr, RtoC);
            sphericalWave.params.filter(sphericalWave.gr);
            transformGridIfNecessary(sphericalWave.params.fieldSolver, sphericalWave.gr, CtoR);
        }

        //MPIWrapper::showMessage("writing to file assembled result");
        if (MPIWrapper::MPIRank() == 0)
            sphericalWave.params.fileWriter.write(sphericalWave.gr, nameFileSecondSteps);

        return Status::OK;
    }

    virtual Status testBody() {
        if (sphericalWave.params.nSeqSteps != 0)
            doSequentialPart();
        MPIWrapper::MPIBarrier();
        if (sphericalWave.params.nParSteps != 0)
            return doParallelPart();
        return Status::OK;
    }

};
