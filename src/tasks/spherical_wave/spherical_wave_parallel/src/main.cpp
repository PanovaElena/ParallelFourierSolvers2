#include "mpi_wrapper.h"
#include <string>
#include <iostream>
#include "test_parallel.h"
#include "test_spherical_wave_parallel.h"
#include "parser_spherical_wave.h"
#include "status.h"

int main(int argc, char** argv) {
    MPIWrapper::MPIInitialize(argc, argv);
    fftw_init_threads();
    ParametersForSphericalWave params;
    ParserSphericalWave parser;
    Stat status = parser.parseArgsForParallel(argc, argv, params);
    if (status == Stat::OK) {
        if (MPIWrapper::MPIRank() == MPIWrapper::MPIROOT) params.print();
        TestSphericalWaveParallel test;
        test.setParamsForTest(params);
        Stat status2 = test.testBody();
        if (status == Stat::ERROR)
            std::cout << "ERROR: fail run test" << std::endl;
    }
    else if (status == Stat::ERROR)
        std::cout << "ERROR: there are some problems in args" << std::endl;
    MPIWrapper::MPIFinalize();
}