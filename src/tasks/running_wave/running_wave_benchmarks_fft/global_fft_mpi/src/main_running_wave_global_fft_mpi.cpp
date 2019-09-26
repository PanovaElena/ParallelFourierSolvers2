#include "mpi_wrapper.h"
#include <string>
#include <iostream>
#include <fstream>
#include <omp.h>
#include "class_member_ptr.h"
#include "running_wave.h"
#include "parser_running_wave.h"
#include "status.h"
#include "grid3d.h"

#include "fftw3.h"
#include "fftw3-mpi.h"

static fftw_plan planRtoC[3][3];
static fftw_plan planCtoR[3][3];

void createPlans(Grid3d& gr, int Nx, int Ny, int Nz);
void destroyPlans();
void useFFTWMPI_FFTW(Array3d<double>& arr1, Array3d<MyComplex>& arr2, int Nx, int Ny, int Nz,
    Direction dir, fftw_plan& plan);
void fourierTransformMPI(Grid3d & gr, Field _field, Coordinate _coord, Direction dir);
void fourierTransformMPI(Grid3d & gr, Direction dir);
void testBody(RunningWave& runningWave);

static vec3<int> globalSize;

int main(int argc, char** argv) {
    MPIWrapper::MPIInitialize(argc, argv);
    fftw_mpi_init();
    fftw_init_threads();

    ParametersForRunningWave params;
    ParserRunningWave parser;

    Status status = parser.parseArgsForParallel(argc, argv, params);

    std::ofstream file("output_" + std::to_string((long long)(MPIWrapper::MPIRank()))+".txt");

    if (status == Status::OK) {
        if (MPIWrapper::MPIRank() == 0) params.print();

        // computing of local sizes
        ptrdiff_t localSizeX = -1, localStartX = -1;
        ptrdiff_t ret = fftw_mpi_local_size_3d(params.n.x, params.n.y, params.n.z / 2 + 1, MPI_COMM_WORLD,
            &localSizeX, &localStartX);
        if (ret == 0) {
            if (MPIWrapper::MPIRank() == 0)
                file << "ERROR: can't create descriptor" << std::endl;
            file.close();
            MPIWrapper::MPIFinalize();
            return 0;
        }
        vec3<int> localGridSize(localSizeX, params.n.y, 2 * (params.n.z / 2 + 1));
        vec3<int> localGridStart(localStartX, 0, 0);
        vec3<> aLocal(params.a + (vec3<>)localGridStart*params.d),
            bLocal(aLocal + (vec3<>)localGridSize*params.d);
        ParametersForRunningWave localParams(params);
        localParams.a = aLocal; localParams.b = bLocal; localParams.n = localGridSize;
        file << "\n" << "RANK " << MPIWrapper::MPIRank() << "\n";
        file << localSizeX << " " << localStartX << " " << ret << "\n";
        localParams.print(file);

        globalSize = params.n;

        RunningWave runningWave;
        runningWave.setParamsForTest(localParams);
        
        createPlans(runningWave.gr, globalSize.x, globalSize.y, globalSize.z);
        runningWave.setEB();

        testBody(runningWave);
        
        destroyPlans();


        // // simple test for mpi fftw
        // const ptrdiff_t N0 = 16, N1 = 16;
        // fftw_plan plan;
        // fftw_complex *data;
        // ptrdiff_t alloc_local, local_n0, local_0_start;

        // /* get local data size and allocate */
        // alloc_local = fftw_mpi_local_size_2d(N0, N1, MPI_COMM_WORLD,
           // &local_n0, &local_0_start);
        // data = fftw_alloc_complex(alloc_local);

        // file << alloc_local << " " << local_n0 << " " << local_0_start << std::endl;

        // /* create plan for in-place forward DFT */
        // plan = fftw_mpi_plan_dft_2d(N0, N1, data, data, MPI_COMM_WORLD,
           // FFTW_FORWARD, FFTW_ESTIMATE);

        // /* initialize data to some function my_function(x,y) */
        // for (int i = 0; i < local_n0; ++i)
           // for (int j = 0; j < N1; ++j){
               // data[i*N1 + j][0] = params.startCond->fE({ i + (int)local_0_start, j, 0 }).y;
               // data[i*N1 + j][1] = 0;
           // }
           
        // for (int i = 0; i < local_n0; ++i){
           // for (int j = 0; j < N1; ++j)
               // file<<"("<<data[i*N1 + j][0]<<" ,"<<data[i*N1 + j][1]<<");";
           // file<<"\n";
        // }
        // file<<"\n";

        // /* compute transforms, in-place, as many times as desired */
        // fftw_execute(plan);
        
        // for (int i = 0; i < local_n0; ++i){
           // for (int j = 0; j < N1; ++j)
               // file<<"("<<data[i*N1 + j][0]<<" ,"<<data[i*N1 + j][1]<<");";
           // file<<"\n";
        // }

        // fftw_destroy_plan(plan);

    }
    else if (status == Status::ERROR)
        std::cout << "There are some problems in args" << std::endl;

    file.close();
    MPIWrapper::MPIFinalize();
    return 0;
}

void useFFTWMPI_FFTW(Array3d<double>& arr1, Array3d<MyComplex>& arr2, int Nx, int Ny, int Nz,
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

void fourierTransformMPI(Grid3d & gr, Field _field, Coordinate _coord, Direction dir) {
    Array3d<double>& arrD = (gr.*getMemberPtrField<double>(_field)).*getMemberPtrFieldCoord<double>(_coord);
    Array3d<MyComplex>& arrC = (gr.*getMemberPtrField<MyComplex>(_field)).*getMemberPtrFieldCoord<MyComplex>(_coord);

    switch (dir) {
        case RtoC:
            useFFTWMPI_FFTW(arrD, arrC, globalSize.x, globalSize.y, globalSize.z, dir, planRtoC[(int)_field][(int)_coord]);
            break;
        case CtoR:
            useFFTWMPI_FFTW(arrD, arrC, globalSize.x, globalSize.y, globalSize.z, dir, planCtoR[(int)_field][(int)_coord]);
            break;
    }

}

void createPlans(Grid3d& gr, int Nx, int Ny, int Nz){
    for (int field=0; field<3; field++)
        for (int coord=0; coord<3; coord++){
            Array3d<double>& arrD = (gr.*getMemberPtrField<double>((Field)field)).*getMemberPtrFieldCoord<double>((Coordinate)coord);
            Array3d<MyComplex>& arrC = (gr.*getMemberPtrField<MyComplex>((Field)field)).*getMemberPtrFieldCoord<MyComplex>((Coordinate)coord);
            
            fftw_plan_with_nthreads(omp_get_max_threads());
            planRtoC[field][coord] = fftw_mpi_plan_dft_r2c_3d(Nx, Ny, Nz, &(arrD[0]), (fftw_complex*)&(arrC[0]),
                MPI_COMM_WORLD, FFTW_MEASURE);
            fftw_plan_with_nthreads(omp_get_max_threads());
            planCtoR[field][coord] = fftw_mpi_plan_dft_c2r_3d(Nx, Ny, Nz, (fftw_complex*)&(arrC[0]), &(arrD[0]),
                MPI_COMM_WORLD, FFTW_MEASURE);
        }
}

void destroyPlans(){
    for (int field=0; field<3; field++)
        for (int coord=0; coord<3; coord++){
            fftw_destroy_plan(planRtoC[field][coord]);
            fftw_destroy_plan(planCtoR[field][coord]);
        }
}

void testBody(RunningWave& runningWave) {

    {
    double t1 = omp_get_wtime();

    fourierTransformMPI(runningWave.gr, RtoC);

    double t2 = omp_get_wtime();
    std::cout << "RtoC " << t2 - t1 << std::endl;
    }
    
    {
    double t1 = omp_get_wtime();

    fourierTransformMPI(runningWave.gr, CtoR);

    double t2 = omp_get_wtime();
    std::cout << "CtoR " << t2 - t1 << std::endl;
    }
}

void fourierTransformMPI(Grid3d & gr, Direction dir) {
    if (gr.getLastFourierTransformDirect() == dir) {
        std::cout << "Try to transform to the same direction: " << dir << std::endl;
        return;
    }

    fourierTransformMPI(gr, E, x, dir);
    fourierTransformMPI(gr, E, y, dir);
    fourierTransformMPI(gr, E, z, dir);
    fourierTransformMPI(gr, B, x, dir);
    fourierTransformMPI(gr, B, y, dir);
    fourierTransformMPI(gr, B, z, dir);
    fourierTransformMPI(gr, J, x, dir);
    fourierTransformMPI(gr, J, y, dir);
    fourierTransformMPI(gr, J, z, dir);

    gr.setLastFourierTransformDirect(dir);
}