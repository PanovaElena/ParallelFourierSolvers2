

add_subdirectory(running_wave_sequential)
add_subdirectory(running_wave_parallel)
add_subdirectory(running_wave_seq_and_par)

if(USE_GLOBAL_FFT)
    add_subdirectory(running_wave_global_fft)
endif()

if(USE_BENCH_FFT)
    add_subdirectory(running_wave_benchmarks_fft)
endif()
