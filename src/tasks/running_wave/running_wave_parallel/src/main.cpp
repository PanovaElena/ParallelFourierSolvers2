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

    Stat initialize(int& argc, char**& argv) {
        Stat status = parser.parseArgs(argc, argv, task);
        if (status != Stat::OK)
            return status;

        parser.print(task);
        if (solver.initialize(task.gridParams, task.guard,
            *task.mask, *task.filter, task.np,
            *task.scheme, *task.fieldSolver, task.fileWriter) == Stat::ERROR)
            return Stat::ERROR;

        return Stat::OK;
    }

    void testBody() {

        double t1 = omp_get_wtime();
        solver.run(task.nParIter, task.nIterLocal, task.gridParams.dt);
        double t2 = omp_get_wtime();

        if (MPIWrapper::MPIRank() == MPIWrapper::MPIROOT)
            std::cout << "Time of parallel version is " << t2 - t1 << "\n";
    }

};

int main(int argc, char** argv) {
    MPIWrapper::MPIInitialize(argc, argv);
    fftw_init_threads();
    
    Configuration configuration;
    Stat status = configuration.initialize(argc, argv);
    if (status == Stat::OK)
        configuration.testBody();
    else if (status == Stat::ERROR)
        std::cout << "ERROR!!!" << std::endl;

    MPIWrapper::MPIFinalize();
    return 0;
}