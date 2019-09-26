add_subdirectory(global_fft_omp)

if (USE_GLOBAL_FFT)
    add_subdirectory(global_fft_mpi)
endif()