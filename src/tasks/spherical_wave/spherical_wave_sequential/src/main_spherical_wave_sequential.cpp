#include "parser_spherical_wave.h"
#include <string>
#include <iostream>
#include "spherical_wave.h"
#include "fourier_transform.h"
#include "field_solver.h"
#include "fftw3.h"


void testBody(SphericalWave& sphericalWave) {
    for (int j = 0; j < sphericalWave.params.nSeqSteps; j++) {
        sphericalWave.setJ(j);
        transformGridIfNecessary(sphericalWave.params.fieldSolver, sphericalWave.gr, RtoC);
        sphericalWave.params.fieldSolver(sphericalWave.gr, sphericalWave.params.dt);
        transformGridIfNecessary(sphericalWave.params.fieldSolver, sphericalWave.gr, CtoR);
    }

    sphericalWave.params.fileWriter.write(sphericalWave.gr, "sequential_result.csv", Double, "writing...");
}

int main(int argc, char** argv) {
    fftw_init_threads();
    SphericalWave sphericalWave;
    ParserSphericalWave parser;
    ParametersForSphericalWave params;
    int status = parser.parseArgsForSequential(argc, argv, params);
    if (status != 0) return 0;
    params.nParSteps = 0;
    params.print();
    sphericalWave.setParamsForTest(params);
    testBody(sphericalWave);
}