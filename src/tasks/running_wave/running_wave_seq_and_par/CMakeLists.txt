
create_executable(
	TARGET_NAME running_wave_seq_and_par
	INCLUDE_DIRS "../include" "../../include" "${CMAKE_SOURCE_DIR}/src/parallel_fourier_solvers/include"
	LIBS parallel_fourier_solvers
)