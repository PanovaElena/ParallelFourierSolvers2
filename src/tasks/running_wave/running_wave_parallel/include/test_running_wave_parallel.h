#pragma once
#include <omp.h>
#include <string>
#include "parallel_fourier_solver.h"
#include "test_parallel.h"
#include "running_wave.h"
#include "field_solver.h"
#include "file_writer.h"


class TestRunningWaveParallel : public TestParallel {
    RunningWave task;
    ParallelFourierSolver parallelSolver;

public:
    void setParamsForTest(ParametersForRunningWave p) {
        task.setParamsForTest(p);
    }

    TestRunningWaveParallel() : task() {
        setNameFiles();
    }

    Stat initializeParallelSolver() {
        return parallelSolver.initialize(task.grid, task.params.guard, *task.params.mask,
            *task.params.filter, task.params.numOfProcesses, *task.params.scheme,
            *task.params.fieldSolver, task.params.fileWriter);
    }

    void doSequentialPart() {
        task.params.fieldSolver->doFourierTransform(RtoC);
        for (int i = 0; i < task.params.nSeqSteps; i++) {
            task.params.fieldSolver->run(task.params.dt);
        }
        task.params.fieldSolver->doFourierTransform(CtoR);

        std::cout << "sequential part done\n";
        if (MPIWrapper::MPIRank() == MPIWrapper::MPIROOT)
            task.params.fileWriter.write(task.grid, nameFileFirstSteps);
    }

    Stat doParallelPart() {

        FileWriter fw(task.params.fileWriter.getDirectory(), task.params.fileWriter.getField(),
            task.params.fileWriter.getCoord(), Section(Section::XOZ, Section::center, Section::XOY, Section::start));
        // it is for spectrum writing

        if (MPIWrapper::MPIRank() == MPIWrapper::MPIROOT) {
            task.params.fieldSolver->doFourierTransform(RtoC);
            fw.write(task.grid, "spectrum_before_div.csv", Complex);
            task.params.fieldSolver->doFourierTransform(CtoR);
        }

        if (initializeParallelSolver() == Stat::ERROR)
            return Stat::ERROR;

        //std::cout << "start par: domain from " << parallelSolver.getDomainStart() << " to " <<
        //    parallelSolver.getDomainEnd() << "; guard is " << parallelSolver.getGuardSize() <<"\n";

        //std::cout << "writing to file first domain\n";
        parallelSolver.writeFile(nameFileAfterDivision);

        if (MPIWrapper::MPIRank() == MPIWrapper::MPIROOT) {
            task.params.fieldSolver->doFourierTransform(RtoC);
            fw.write(parallelSolver.getGrid(), "spectrum_after_div.csv", Complex);
            task.params.fieldSolver->doFourierTransform(CtoR);
        }

        //std::cout << "parallel field solver\n";
        double t1 = omp_get_wtime();

        parallelSolver.run(task.params.nParSteps, task.params.nDomainSteps, task.params.dt);

        double t2 = omp_get_wtime();
        if (MPIWrapper::MPIRank() == MPIWrapper::MPIROOT)
            std::cout << "Time of parallel version is " << t2 - t1 << "\n";

        //std::cout << "writing to file parallel result\n";
        parallelSolver.writeFile(nameFileAfterExchange);

        //std::cout << "assemble\n";
        parallelSolver.assembleResults(task.grid);

        if (MPIWrapper::MPIRank() == MPIWrapper::MPIROOT) {
            task.params.fieldSolver->doFourierTransform(RtoC);
            fw.write(task.grid, "spectrum_before_filter.csv", Complex);
            task.params.filter->apply(task.grid);
            fw.write(task.grid, "spectrum_after_filter.csv", Complex);
            task.params.fieldSolver->doFourierTransform(CtoR);
        }

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