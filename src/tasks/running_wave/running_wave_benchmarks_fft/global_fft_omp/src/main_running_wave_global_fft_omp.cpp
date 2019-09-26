#include "parser_running_wave.h"
#include <string>
#include <iostream>
#include <fstream>
#include <omp.h>
#include "class_member_ptr.h"
#include "running_wave.h"
#include "status.h"
#include "grid3d.h"
#include "fftw3.h"

static fftw_plan planRtoC[3][3];
static fftw_plan planCtoR[3][3];

void createPlans(Grid3d& gr, int Nx, int Ny, int Nz);
void destroyPlans();
void useFFTW_FFTW(Array3d<double>& arr1, Array3d<MyComplex>& arr2, int Nx, int Ny, int Nz,
    Direction dir, fftw_plan& plan);
void fourierTransform(Grid3d & gr, Field _field, Coordinate _coord, Direction dir);
void fourierTransform(Grid3d & gr, Direction dir);

void testBody(RunningWave& runningWave) {
    createPlans(runningWave.gr, runningWave.gr.sizeReal().x, runningWave.gr.sizeReal().y, runningWave.gr.sizeReal().z);
    runningWave.setEB();
    {
    double t1 = omp_get_wtime();

    fourierTransform(runningWave.gr, RtoC);

    double t2 = omp_get_wtime();
    std::cout << "RtoC " << t2 - t1 << std::endl;
    }
    
    {
    double t1 = omp_get_wtime();

    fourierTransform(runningWave.gr, CtoR);

    double t2 = omp_get_wtime();
    std::cout << "CtoR " << t2 - t1 << std::endl;
    }
    destroyPlans();
    
}

void useFFTW_FFTW(Array3d<double>& arr1, Array3d<MyComplex>& arr2, int Nx, int Ny, int Nz,
    Direction dir, fftw_plan& plan) {
    switch (dir) {
    case RtoC:
        fftw_execute(plan);
        break;
    case CtoR:
        fftw_execute(plan);
        for (int i = 0; i < arr1.size1d(); i++)
            arr1[i] /= Nx * Ny * Nz;
        break;
    }
}

void fourierTransform(Grid3d & gr, Field _field, Coordinate _coord, Direction dir) {
    Array3d<double>& arrD = (gr.*getMemberPtrField<double>(_field)).*getMemberPtrFieldCoord<double>(_coord);
    Array3d<MyComplex>& arrC = (gr.*getMemberPtrField<MyComplex>(_field)).*getMemberPtrFieldCoord<MyComplex>(_coord);

    switch (dir) {
        case RtoC:
            useFFTW_FFTW(arrD, arrC, gr.sizeReal().x, gr.sizeReal().y, gr.sizeReal().z,
                dir, planRtoC[(int)_field][(int)_coord]);
            break;
        case CtoR:
            useFFTW_FFTW(arrD, arrC, gr.sizeReal().x, gr.sizeReal().y, gr.sizeReal().z,
                dir, planCtoR[(int)_field][(int)_coord]);
            break;
    }

}

void createPlans(Grid3d& gr, int Nx, int Ny, int Nz){
    for (int field=0; field<3; field++)
        for (int coord=0; coord<3; coord++){
            Array3d<double>& arrD = (gr.*getMemberPtrField<double>((Field)field)).*getMemberPtrFieldCoord<double>((Coordinate)coord);
            Array3d<MyComplex>& arrC = (gr.*getMemberPtrField<MyComplex>((Field)field)).*getMemberPtrFieldCoord<MyComplex>((Coordinate)coord);
            
            fftw_plan_with_nthreads(omp_get_max_threads());
            planRtoC[field][coord] = fftw_plan_dft_r2c_3d(Nx, Ny, Nz, &(arrD[0]), (fftw_complex*)&(arrC[0]),
                FFTW_MEASURE);
            fftw_plan_with_nthreads(omp_get_max_threads());
            planCtoR[field][coord] = fftw_plan_dft_c2r_3d(Nx, Ny, Nz, (fftw_complex*)&(arrC[0]), &(arrD[0]),
                FFTW_MEASURE);
        }
}

void destroyPlans(){
    for (int field=0; field<3; field++)
        for (int coord=0; coord<3; coord++){
            fftw_destroy_plan(planRtoC[field][coord]);
            fftw_destroy_plan(planCtoR[field][coord]);
        }
}

void fourierTransform(Grid3d & gr, Direction dir) {
    if (gr.getLastFourierTransformDirect() == dir) {
        std::cout << "Try to transform to the same direction: " << dir << std::endl;
        return;
    }

    fourierTransform(gr, E, x, dir);
    fourierTransform(gr, E, y, dir);
    fourierTransform(gr, E, z, dir);
    fourierTransform(gr, B, x, dir);
    fourierTransform(gr, B, y, dir);
    fourierTransform(gr, B, z, dir);
    fourierTransform(gr, J, x, dir);
    fourierTransform(gr, J, y, dir);
    fourierTransform(gr, J, z, dir);

    gr.setLastFourierTransformDirect(dir);
}

int main(int argc, char** argv) {
    fftw_init_threads();
    RunningWave runningWave;
    ParserRunningWave parser;
    ParametersForRunningWave params;
    int status = parser.parseArgsForSequential(argc, argv, params);
    if (status != 0) return 0;
    params.nParSteps = 0;
    params.print();
    runningWave.setParamsForTest(params);
    testBody(runningWave);
}