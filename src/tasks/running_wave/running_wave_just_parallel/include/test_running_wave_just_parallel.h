#pragma once
#include <omp.h>
#include <string>
#include "parallel_fourier_solver.h"
#include "test_parallel.h"
#include "running_wave.h"
#include "field_solver.h"
#include "file_writer.h"


class TestRunningWaveJustParallel : public TestParallel {
    RunningWave task;
    ParallelFourierSolver parallelSolver;

public:
    void setParamsForTest(ParametersForRunningWave p) {
        task.setParamsForTest(p);
    }

    TestRunningWaveJustParallel() : task(false) {
        setNameFiles();
    }

    Stat initializeParallelSolver() {
        return parallelSolver.initialize(task.params.gridParams, task.params.guard, *task.params.mask,
            *task.params.filter, task.params.numOfProcesses, *task.params.scheme,
            *task.params.fieldSolver, task.params.fileWriter);
    }

    Stat doParallelPart() {

        if (initializeParallelSolver() == Stat::ERROR)
            return Stat::ERROR;

        //std::cout << "writing to file first domain\n";
        parallelSolver.writeFile(nameFileAfterDivision);

        //std::cout << "parallel field solver\n";
        double t1 = omp_get_wtime();

        parallelSolver.run(task.params.nParSteps, task.params.nDomainSteps, task.params.dt);

        double t2 = omp_get_wtime();
        if (MPIWrapper::MPIRank() == MPIWrapper::MPIROOT)
            std::cout << "Time of parallel version is " << t2 - t1 << "\n";

        //std::cout << "writing to file parallel result\n";
        parallelSolver.writeFile(nameFileAfterExchange);

        return Stat::OK;
    }

    virtual Stat testBody() {
        if (task.params.nParSteps != 0)
            return doParallelPart();
        return Stat::OK;
    }

};