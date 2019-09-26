#include "mpi_wrapper.h"
#include <string>
#include <iostream>
#include "test_parallel.h"
#include "test_running_wave_parallel.h"
#include "parser_running_wave.h"
#include "status.h"
#include "fftw3.h"

int main(int argc, char** argv) {
    MPIWrapper::MPIInitialize(argc, argv);
    fftw_init_threads();
    ParametersForRunningWave params;
    ParserRunningWave parser;
    MPIWorker* mpiWorker = 0;
    Status status = parser.parseArgsForParallel(argc, argv, params);
    Status status1 = parser.createMPIWorker(mpiWorker);
    Status status2 = Status::OK;
    if (status == Status::OK && status1 == Status::OK) {
        if (MPIWrapper::MPIRank() == 0) params.print();
        TestRunningWaveParallel test(*mpiWorker);
        test.setParamsForTest(params);
        status2 = test.testBody();
    }
    else if (status == Status::ERROR || status1 == Status::ERROR || status2 == Status::ERROR)
        std::cout << "There are some problems in args" << std::endl;
    delete mpiWorker;
    MPIWrapper::MPIFinalize();
}