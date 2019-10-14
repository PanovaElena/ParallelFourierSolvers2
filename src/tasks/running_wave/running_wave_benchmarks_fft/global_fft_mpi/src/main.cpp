#include "mpi_wrapper.h"
#include <string>
#include <iostream>
#include <fstream>
#include <omp.h>
#include "running_wave.h"
#include "parser_running_wave.h"
#include "fftw3.h"
#include "fftw3-mpi.h"

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
    MPIWrapper::MPIInitialize(argc, argv);
    fftw_mpi_init();
    fftw_init_threads();

    ParametersForRunningWave params;
    ParserRunningWave parser;

    Stat status = parser.parseArgsForParallel(argc, argv, params);

    std::ofstream file("output_" + std::to_string((long long)(MPIWrapper::MPIRank()))+".txt");
    if (status == Stat::OK) {
        if (MPIWrapper::MPIRank() == MPIWrapper::MPIROOT) params.print();

        // computing of local sizes
        ptrdiff_t localSizeX = -1, localStartX = -1;
        ptrdiff_t ret = fftw_mpi_local_size_3d(params.gridParams.n.x,
            params.gridParams.n.y, params.gridParams.n.z / 2 + 1, MPI_COMM_WORLD,
            &localSizeX, &localStartX);
        if (ret == 0) {
            if (MPIWrapper::MPIRank() == MPIWrapper::MPIROOT)
                file << "ERROR: can't create descriptor" << std::endl;
            file.close();
            MPIWrapper::MPIFinalize();
            return 0;
        }
        vec3<int> localGridSize(localSizeX, params.gridParams.n.y,
            2 * (params.gridParams.n.z / 2 + 1));
        vec3<int> localGridStart(localStartX, 0, 0);
        vec3<> aLocal(params.gridParams.a + (vec3<>)localGridStart*params.gridParams.d);
        ParametersForRunningWave localParams(params);
        localParams.gridParams.a = aLocal;
        localParams.gridParams.n = localGridSize;
        file << "\n" << "RANK " << MPIWrapper::MPIRank() << "\n";
        file << localSizeX << " " << localStartX << " " << ret << "\n";
        localParams.print(file);

        FourierFieldSolver* fs = dynamic_cast<FourierFieldSolver*>(params.fieldSolver.get());
        if (!fs) return 0;
        fs->setGlobalFourierTransform(params.gridParams.n);

        RunningWave runningWave;
        runningWave.setParamsForTest(localParams);  // + setting start conditions

        testBody(runningWave);


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
    else if (status == Stat::ERROR)
        std::cout << "There are some problems in args" << std::endl;

    file.close();
    MPIWrapper::MPIFinalize();
    return 0;
}