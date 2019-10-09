#include <iostream>
#include <omp.h>
#include "parser_running_wave.h"
#include "running_wave.h"

void testBody(RunningWave& runningWave) {
    {
    double t1 = omp_get_wtime();

    runningWave.params.fieldSolver->doFourierTransform(RtoC);

    double t2 = omp_get_wtime();
    std::cout << "RtoC " << t2 - t1 << std::endl;
    }
    
    {
    double t1 = omp_get_wtime();

    runningWave.params.fieldSolver->doFourierTransform(CtoR);

    double t2 = omp_get_wtime();
    std::cout << "CtoR " << t2 - t1 << std::endl;
    }
    
}

int main(int argc, char** argv) {
    fftw_init_threads();
    RunningWave runningWave;
    ParserRunningWave parser;
    ParametersForRunningWave params;
    int status = parser.parseArgsForSequential(argc, argv, params);
    if (status != Stat::OK) return 0;
    params.nParSteps = 0;
    params.print();
    runningWave.setParamsForTest(params);
    testBody(runningWave);
}