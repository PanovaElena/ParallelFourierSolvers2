#pragma once
#include <omp.h>
#include <string>
#include "parallel_fourier_solver.h"
#include "test_parallel.h"
#include "spherical_wave.h"
#include "field_solver.h"
#include "file_writer.h"

class TestSphericalWaveParallel : public TestParallel {
    SphericalWave task;
    ParallelFourierSolver parallelSolver;

public:
    void setParamsForTest(const ParametersForSphericalWave& p) {
        task.setParamsForTest(p);
    }

    TestSphericalWaveParallel() : task() {
        setNameFiles();
    }

    Stat initializeParallelSolver() {
        return parallelSolver.initialize(task.grid, task.params.guard, *task.params.mask,
            *task.params.filter, task.params.numOfProcesses, *task.params.scheme,
            *task.params.fieldSolver, task.params.fileWriter);
    }

    void doSequentialPart() {

        for (int j = 0; j < task.params.nSeqSteps; j++) {
            task.grid.setJ(j);
            task.params.fieldSolver->doFourierTransform(RtoC);
            task.params.fieldSolver->run(task.params.dt);
            task.params.fieldSolver->doFourierTransform(CtoR);
        }

        //std::cout << "sequential part done\n";
        if (MPIWrapper::MPIRank() == MPIWrapper::MPIROOT)
            task.params.fileWriter.write(task.grid, nameFileFirstSteps);
    }

    Stat doParallelPart() {

        if (initializeParallelSolver() == Stat::ERROR)
            return Stat::ERROR;

        double endTimeSource = task.params.source.getEndTime();
        double startTimePar = task.params.source.startTime + task.params.dt*task.params.nSeqSteps;

        //std::cout << "parallel field solver\n";
        double t1 = omp_get_wtime();

        if (endTimeSource > startTimePar) {
            for (int j = 0; j < task.params.nParSteps; j++) {
                parallelSolver.getGrid().setJ(task.params.nSeqSteps + j);
                parallelSolver.run(1, 1, task.params.dt, j == task.params.nParSteps - 1);
            }
        }
        else parallelSolver.run(task.params.nParSteps, task.params.nDomainSteps, task.params.dt);

        double t2 = omp_get_wtime();
        if (MPIWrapper::MPIRank() == MPIWrapper::MPIROOT)
            std::cout << "Time of parallel version is " << t2 - t1 << "\n";


        //std::cout << "assemble\n";
        parallelSolver.assembleResults(task.grid);

        //std::cout << "writing to file assembled result\n";
        if (MPIWrapper::MPIRank() == MPIWrapper::MPIROOT)
            task.params.fileWriter.write(task.grid, nameFileSecondSteps);

        return Stat::OK;
    }

    virtual Stat testBody() {
        if (task.params.nSeqSteps != 0)
            doSequentialPart();
        MPIWrapper::MPIBarrier();
        if (task.params.nParSteps != 0)
            return doParallelPart();
        return Stat::OK;
    }

};
