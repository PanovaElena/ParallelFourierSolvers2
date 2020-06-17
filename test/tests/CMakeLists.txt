
create_executable(
	TARGET_NAME tests
	INCLUDE_DIRS "include" "${CMAKE_SOURCE_DIR}/src/parallel_fourier_solvers/include"
	LIBS parallel_fourier_solvers gtest
)
