#include <string>
#include <iostream>
#include <omp.h>
#include "fftw3.h"
#include "running_wave.h"
#include "grid3d.h"
#include "parallel_fourier_solver.h"


struct Configuration {

    RunningWaveParallelTask task;
    RunningWaveParallelTaskParser parser;
    ParallelFourierSolver solver;
    std::unique_ptr<Grid3d> grid;

    Stat initialize(int& argc, char**& argv) {
        Stat status = parser.parseArgs(argc, argv, task);
        if (status != Stat::OK)
            return status;

        parser.print(task);
        grid.reset(new Grid3d(task.gridParams));
        task.fieldSolver->initialize(grid.get());
        grid->setFields();

        return Stat::OK;
    }

    void testBody() {

        // sequential part 
        {
            double t1 = omp_get_wtime();
            task.fieldSolver->doFourierTransform(RtoC);
            for (int j = 0; j < task.nIter - task.nParIter; j++)
                task.fieldSolver->run(task.gridParams.dt);
            task.fieldSolver->doFourierTransform(CtoR);
            double t2 = omp_get_wtime();

            if (MPIWrapper::MPIRank() == MPIWrapper::MPIROOT) {
                std::cout << "Time of sequential part is " << t2 - t1 << std::endl;

                task.fileWriter.write(*grid, "sequential_part",
                    Double, "writing sequential result..");
            }

            MPIWrapper::MPIBarrier();
        }
        // parallel part
        {
            if (solver.initialize(task.gridParams, task.guard,
                *task.mask, *task.filter, task.np,
                *task.scheme, *task.fieldSolver, task.fileWriter,
                grid.get()) == Stat::ERROR) {
                std::cout << "ERROR: parallel part can not be initialized\n";
                return;
            }

            double t1 = omp_get_wtime();
            solver.run(task.nParIter, task.nIterLocal, task.gridParams.dt);
            double t2 = omp_get_wtime();

            solver.assembleResults(grid.get());

            if (MPIWrapper::MPIRank() == MPIWrapper::MPIROOT) {
                std::cout << "Time of parallel part is " << t2 - t1 << "\n";

                task.fileWriter.write(*grid, "parallel_result",
                    Double, "writing parallel result..");
            }
        }
    }
};

int main(int argc, char** argv) {
    MPIWrapper::MPIInitialize(argc, argv);
    fftw_init_threads();
    {
        Configuration configuration;
        Stat status = configuration.initialize(argc, argv);
        if (status == Stat::OK)
            configuration.testBody();
        else if (status == Stat::ERROR)
            std::cout << "ERROR!!!" << std::endl;
    }

    MPIWrapper::MPIFinalize();
    return 0;
}
