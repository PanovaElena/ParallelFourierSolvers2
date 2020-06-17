#include <string>
#include <iostream>
#include <omp.h>
#include "fftw3.h"
#include "running_wave.h"
#include "grid3d.h"


struct Configuration {

    RunningWaveTask task;
    RunningWaveTaskParser parser;
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
        double t1 = omp_get_wtime();
        task.fieldSolver->doFourierTransform(RtoC);
        for (int j = 0; j < task.nIter; j++)
            task.fieldSolver->run(task.gridParams.dt);
        task.fieldSolver->doFourierTransform(CtoR);
        double t2 = omp_get_wtime();

        std::cout << "Time of sequential version is " << t2 - t1 << std::endl;

        task.fileWriter.write(*grid, "sequential_result",
            Double, "writing sequential result..");
    }

};

int main(int argc, char** argv) {
    fftw_init_threads();

    Configuration configuration;
    Stat status = configuration.initialize(argc, argv);
    if (status == Stat::OK)
        configuration.testBody();
    else if (status == Stat::ERROR)
        std::cout << "ERROR!!!" << std::endl;

    return 0;
}