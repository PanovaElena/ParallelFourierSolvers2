#include <string>
#include <iostream>
#include <omp.h>
#include "parser_running_wave.h"
#include "running_wave.h"
#include "fourier_transform.h"
#include "field_solver.h"


void testBody(RunningWave& runningWave) {

    double t1 = omp_get_wtime();

    runningWave.params.fieldSolver->doFourierTransform(RtoC);

    double t2 = omp_get_wtime();

    for (int j = 0; j < runningWave.params.nSeqSteps; j++) {
        runningWave.params.fieldSolver->run(runningWave.params.dt);
    }

    double t3 = omp_get_wtime();

    runningWave.params.fieldSolver->doFourierTransform(CtoR);
	
    double t4 = omp_get_wtime();

    std::cout << "Time of sequential version is " << t4 - t1 << std::endl;
    std::cout << "Time of direct transform is " << t2 - t1 << std::endl;
    std::cout << "Time of field solver is " << t3 - t2 << std::endl;
    std::cout << "Time of inverse transform is " << t4 - t3 << std::endl;

    runningWave.params.fileWriter.write(runningWave.grid, "sequential_result.csv",
        Double, "writing sequential result..");

}

int main(int argc, char** argv) {
    fftw_init_threads();
    RunningWave runningWave;
    ParserRunningWave parser;
    ParametersForRunningWave params;
    Stat status = parser.parseArgsForSequential(argc, argv, params);
    if (status != Stat::OK)
    {
        std::cout << "ERROR: wrong status" << std::endl;
        return 0;
    }
    params.print();

    runningWave.setParamsForTest(params);
    testBody(runningWave);
}