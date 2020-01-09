#include "mpi_wrapper.h"
#include <string>
#include <iostream>
#include <fstream>
#include <omp.h>
#include "parser_tight_focusing.h"
#include "tight_focusing.h"
#include "fftw3.h"
#include "fftw3-mpi.h"

void testBody(TightFocusing& tightFocusing) {

    double t1 = omp_get_wtime();
    ParametersForTightFocusing& p = tightFocusing.params;

    p.fileWriterEx.write(tightFocusing.grid, "global_fft_result_Ex_rank_" +
        std::to_string((long long)(MPIWrapper::MPIRank())) + "_iter_" + std::to_string((long long)(0)) + ".csv", Double);
    p.fileWriterEy.write(tightFocusing.grid, "global_fft_result_Ey_rank_" +
        std::to_string((long long)(MPIWrapper::MPIRank())) + "_iter_" + std::to_string((long long)(0)) + ".csv", Double);
    p.fileWriterEz.write(tightFocusing.grid, "global_fft_result_Ez_rank_" +
        std::to_string((long long)(MPIWrapper::MPIRank())) + "_iter_" + std::to_string((long long)(0)) + ".csv", Double);

    for (int j = 0; j < p.nSeqSteps; j++) {

        p.fieldSolver->doFourierTransform(RtoC);
        p.fieldSolver->run(p.dt);
        p.fieldSolver->doFourierTransform(CtoR);

        p.fileWriterEx.write(tightFocusing.grid, "global_fft_result_Ex_rank_" +
            std::to_string((long long)(MPIWrapper::MPIRank())) + "_iter_" + std::to_string((long long)(j+1)) + ".csv", Double);
        p.fileWriterEy.write(tightFocusing.grid, "global_fft_result_Ey_rank_" +
            std::to_string((long long)(MPIWrapper::MPIRank())) + "_iter_" + std::to_string((long long)(j+1)) + ".csv", Double);
        p.fileWriterEz.write(tightFocusing.grid, "global_fft_result_Ez_rank_" +
            std::to_string((long long)(MPIWrapper::MPIRank())) + "_iter_" + std::to_string((long long)(j+1)) + ".csv", Double);
    }

    double t2 = omp_get_wtime();
    std::cout << MPIWrapper::MPIRank() << ": Time of mpi fft version is " << t2 - t1 << std::endl;
}

int main(int argc, char** argv) {
	MPIWrapper::MPIInitialize(argc, argv);
	fftw_mpi_init();
	fftw_init_threads();

	ParametersForTightFocusing params;
	ParserTightFocusing parser;

	Stat status = parser.parseArgsForParallel(argc, argv, params);

	if (status == Stat::OK) {
		if (MPIWrapper::MPIRank() == MPIWrapper::MPIROOT) params.print();

		// computing of local sizes
		ptrdiff_t localSizeX = -1, localStartX = -1;
		ptrdiff_t allocLocal = fftw_mpi_local_size_3d(params.gridParams.n.x,
			params.gridParams.n.y, params.gridParams.n.z / 2 + 1, MPI_COMM_WORLD,
			&localSizeX, &localStartX);
		if (allocLocal == 0) {
			if (MPIWrapper::MPIRank() == MPIWrapper::MPIROOT)
				std::cout << "ERROR: can't create descriptor" << std::endl;
			MPIWrapper::MPIFinalize();
			return 0;
		}
		vec3<int> localGridSize(localSizeX, params.gridParams.n.y, params.gridParams.n.z);
		vec3<int> localGridStart(localStartX, 0, 0);
		vec3<> aLocal(params.gridParams.a + (vec3<>)localGridStart * params.gridParams.d);
		ParametersForTightFocusing localParams(params);
		localParams.gridParams.a = aLocal;
		localParams.gridParams.n = localGridSize;
		std::cout << "\n" << "RANK " << MPIWrapper::MPIRank() << " " << allocLocal <<
			" " << localSizeX << " " << localStartX << "\n";

		FourierFieldSolver* fs = dynamic_cast<FourierFieldSolver*>(localParams.fieldSolver.get());
		if (!fs) return 0;
		fs->setGlobalFourierTransform(params.gridParams.n, localStartX);

		TightFocusing tightFocusing;
		tightFocusing.setParamsForTest(localParams, allocLocal);  // + setting start conditions

		testBody(tightFocusing);


		//// simple test for mpi fftw
		//const ptrdiff_t N0 = 16, N1 = 12, N2 = 12;
		//fftw_plan plan;
		//double *rin;
		//fftw_complex *out;
		//ptrdiff_t alloc_local, local_n0, local_0_start;

		///* get local data size and allocate */
		//alloc_local = fftw_mpi_local_size_3d(N0, N1, N2 / 2 + 1, MPI_COMM_WORLD,
		//	&local_n0, &local_0_start);
		//rin = fftw_alloc_real(2 * alloc_local);
		//out = fftw_alloc_complex(alloc_local);

		//std::cout << "RANK " << std::to_string(MPIWrapper::MPIRank()) << " " <<
		//	alloc_local << " " << local_n0 << " " << local_0_start << std::endl;

		///* create plan for in-place forward DFT */
		//plan = fftw_mpi_plan_dft_r2c_3d(N0, N1, N2, rin, out, MPI_COMM_WORLD, FFTW_ESTIMATE);

		///* initialize data to some function my_function(x,y) */
		//for (int i = 0; i < local_n0; ++i)
		//	for (int j = 0; j < N1; ++j)
		//		for (int k = 0; k < N2; ++k)
		//		{
		//			rin[(i*N1 + j) * 2 * (N2 / 2 + 1) + k] =
		//				params.gridParams.fE({ i + (int)local_0_start, j, 0 }, 0).y;
		//		}

		//for (int i = 0; i < local_n0; ++i) {
		//	for (int j = 0; j < N1; ++j)
		//		std::cout << rin[(i*N1 + j) * 2 * (N2 / 2 + 1)] << ";";
		//	std::cout << "\n";
		//}
		//std::cout << "\n";

		///* compute transforms, in-place, as many times as desired */
		//fftw_execute(plan);

		//for (int i = 0; i < local_n0; ++i) {
		//	std::cout << "(" << out[(i*N1) * 2 * (N2 / 2 + 1)][0] << " ,"
		//		<< out[(i*N1) * 2 * (N2 / 2 + 1)][1] << ");";
		//}

		//fftw_destroy_plan(plan);

	}
	else if (status == Stat::ERROR)
		std::cout << "There are some problems in args" << std::endl;

	MPIWrapper::MPIFinalize();
	return 0;
}
