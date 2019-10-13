#include <string>
#include <iostream>
#include <omp.h>
#include "parser_spherical_wave.h"
#include "spherical_wave.h"
#include "fourier_transform.h"
#include "field_solver.h"


void testBody(SphericalWave& sphericalWave) {

    ParametersForSphericalWave& params = sphericalWave.params;

    double t1 = omp_get_wtime();

    for (int j = 0; j < params.nSeqSteps; j++) {
        sphericalWave.grid.setJ(j);
        params.fieldSolver->doFourierTransform(RtoC);
        params.fieldSolver->run(params.dt);
        params.fieldSolver->doFourierTransform(CtoR);
    }

    double t2 = omp_get_wtime();

    std::cout << "Time of sequential version is " << t2 - t1 << std::endl;

    sphericalWave.params.fileWriter.write(sphericalWave.grid, "sequential_result.csv",
        Double, "writing...");

}

int main(int argc, char** argv) {
    fftw_init_threads();
    SphericalWave sphericalWave;
    ParserSphericalWave parser;
    ParametersForSphericalWave params;
    Stat status = parser.parseArgsForSequential(argc, argv, params);
    if (status != Stat::OK)
    {
        std::cout << "ERROR: wrong status" << std::endl;
        return 0;
    }
    params.print();

    sphericalWave.setParamsForTest(params);
    testBody(sphericalWave);
}