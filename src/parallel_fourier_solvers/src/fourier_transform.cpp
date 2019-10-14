#include <omp.h>
#include "mpi_wrapper.h"
#include "fftw3.h"
#include "grid3d.h"
#include "my_complex.h"
#include "fourier_transform.h"
#include "array3d.h"
#include "class_member_ptr.h"

#ifdef __USE_GLOBAL_FFT__
#include "fftw3-mpi.h"
#endif __USE_GLOBAL_FFT__

void FourierTransformOfGrid::createPlans(Grid3d & grid)
{
    int Nx = globalSize.x, Ny = globalSize.y, Nz = globalSize.z;
    for (int field = 0; field < 3; field++)
        for (int coord = 0; coord < 3; coord++) {
            Array3d<double>& arrD = (grid.*getMemberPtrField<double>((Field)field))
                .*getMemberPtrFieldCoord<double>((Coordinate)coord);
            Array3d<MyComplex>& arrC = (grid.*getMemberPtrField<MyComplex>((Field)field))
                .*getMemberPtrFieldCoord<MyComplex>((Coordinate)coord);

            fftw_plan_with_nthreads(omp_get_max_threads());
            plans[RtoC][field][coord] = fftw_plan_dft_r2c_3d(Nx, Ny, Nz, &(arrD[0]), (fftw_complex*)&(arrC[0]),
                FFTW_MEASURE);
            fftw_plan_with_nthreads(omp_get_max_threads());
            plans[CtoR][field][coord] = fftw_plan_dft_c2r_3d(Nx, Ny, Nz, (fftw_complex*)&(arrC[0]), &(arrD[0]),
                FFTW_MEASURE);

        }
}

void FourierTransformOfGrid::destroyPlans()
{
    for (int field = 0; field < 3; field++)
        for (int coord = 0; coord < 3; coord++) {
            fftw_destroy_plan(plans[RtoC][field][coord]);
            fftw_destroy_plan(plans[CtoR][field][coord]);
        }
}

void FourierTransformOfGrid::makeFFT(Array3d<double>& arr1, Array3d<MyComplex>& arr2,
    vec3<int> N, Direction dir, fftw_plan& plan)
{
    switch (dir) {
    case RtoC:
        fftw_execute(plan);
        break;
    default:
        fftw_execute(plan);
        for (int i = 0; i < arr1.size1d(); i++)
            arr1[i] /= N.x * N.y * N.z;
        break;
    }
}

void FourierTransformOfGrid::fourierTransform(Grid3d & grid, Direction dir)
{
    if (grid.getLastFourierTransformDirect() == dir) {
        std::cout << "Try to transform to the same direction: " << dir << std::endl;
        return;
    }

    fourierTransform(grid, E, x, dir);
    fourierTransform(grid, E, y, dir);
    fourierTransform(grid, E, z, dir);
    fourierTransform(grid, B, x, dir);
    fourierTransform(grid, B, y, dir);
    fourierTransform(grid, B, z, dir);
    fourierTransform(grid, J, x, dir);
    fourierTransform(grid, J, y, dir);
    fourierTransform(grid, J, z, dir);

    grid.setLastFourierTransformDirect(dir);
}

void FourierTransformOfGrid::fourierTransform(Grid3d & grid, Field _field, Coordinate _coord,
    Direction dir)
{
    Array3d<double>& arrD = (grid.*getMemberPtrField<double>(_field))
        .*getMemberPtrFieldCoord<double>(_coord);
    Array3d<MyComplex>& arrC = (grid.*getMemberPtrField<MyComplex>(_field))
        .*getMemberPtrFieldCoord<MyComplex>(_coord);

    makeFFT(arrD, arrC, globalSize, dir, plans[dir][_field][_coord]);
}


void FourierMpiTransformOfGrid::createPlans(Grid3d & grid)
{
#ifdef __USE_GLOBAL_FFT__
    int Nx = globalSize.x, Ny = globalSize.y, Nz = globalSize.z;
    for (int field = 0; field < 3; field++)
        for (int coord = 0; coord < 3; coord++) {
            Array3d<double>& arrD = (grid.*getMemberPtrField<double>((Field)field))
                .*getMemberPtrFieldCoord<double>((Coordinate)coord);
            Array3d<MyComplex>& arrC = (grid.*getMemberPtrField<MyComplex>((Field)field))
                .*getMemberPtrFieldCoord<MyComplex>((Coordinate)coord);

            fftw_plan_with_nthreads(omp_get_max_threads());
            plans[RtoC][field][coord] = fftw_mpi_plan_dft_r2c_3d(Nx, Ny, Nz, &(arrD[0]), (fftw_complex*)&(arrC[0]),
                MPI_COMM_WORLD, FFTW_MEASURE);
            fftw_plan_with_nthreads(omp_get_max_threads());
            plans[CtoR][field][coord] = fftw_mpi_plan_dft_c2r_3d(Nx, Ny, Nz, (fftw_complex*)&(arrC[0]), &(arrD[0]),
                MPI_COMM_WORLD, FFTW_MEASURE);

        }
#else __USE_GLOBAL_FFT__
    FourierTransformOfGrid::createPlans(grid);
#endif __USE_GLOBAL_FFT__
}
