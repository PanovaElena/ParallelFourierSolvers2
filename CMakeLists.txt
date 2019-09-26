cmake_minimum_required(VERSION 2.8.11)

option(USE_MKL "using mkl" ON)
option(USE_FFTW "using fftw" OFF)
option(USE_GLOBAL_FFT "using global mpi fft" OFF)
option(USE_OMP "using omp" ON)
option(USE_TESTS "using tests" ON)
option(USE_BENCH_FFT "using benchmarks for fft" OFF)

# set FFTW_DIR if fftw has already installed
# create fftw wrapper if you want to use global parallel fft from mkl with name
# ${CMAKE_SOURCE_DIR}/wrappers/${CMAKE_STATIC_LIBRARY_PREFIX}fftw3x_cdft_lp64${CMAKE_STATIC_LIBRARY_SUFFIX}

set(PROJECT "parallel_fourier_solvers")
project(${PROJECT})

include(cmake/function.cmake)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

add_definitions(-DROOT_DIR="${CMAKE_SOURCE_DIR}")
if (USE_MKL)
    add_definitions(-D__USE_MKL__)
endif()
if (USE_FFTW)
    add_definitions(-D__USE_FFTW__)
endif()
if (USE_GLOBAL_FFT)
    add_definitions(-D__USE_GLOBAL_FFT__)
endif()

set(INSTALL_DIR "${CMAKE_BINARY_DIR}/3rdparty")
file(MAKE_DIRECTORY "${INSTALL_DIR}")

if (DCMAKE_CXX_COMPILER MATCHES CMAKE_CXX_COMPILER-NOTFOUND)
    message(WARNING "Cannot find ${DCMAKE_CXX_COMPILER} compiler")
endif()
message(STATUS "Using compiler: ${CMAKE_CXX_COMPILER_ID}")


if (CMAKE_VERSION VERSION_LESS "3.1")
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    endif()
else()
    set(CMAKE_CXX_STANDARD 11)
endif()


if (MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    set(CMAKE_EXE_LINKER_FLAGS "/NODEFAULTLIB:libcmt.lib")
    set(CMAKE_SHARED_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS})
    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(CMAKE_EXE_LINKER_FLAGS "/machine:x64 ${CMAKE_EXE_LINKER_FLAGS}")
        set(CMAKE_SHARED_LINKER_FLAGS "/machine:x64 ${CMAKE_EXE_LINKER_FLAGS}")
        set(CMAKE_MODULE_LINKER_FLAGS "/machine:x64")
        set(CMAKE_STATIC_LINKER_FLAGS "/machine:x64")
    endif()
endif()


# mpi
if (WIN32)
    if (NOT MPI_CXX_LIBRARIES)
        set(MPI_CXX_LIBRARIES "$ENV{I_MPI_ROOT}/intel64/lib/*.lib")
    endif()  
    if (NOT MPI_CXX_INCLUDE_DIRS)
        set(MPI_CXX_INCLUDE_DIRS "$ENV{I_MPI_ROOT}/intel64/include")
    endif()
    message(STATUS "Found MPI: ${MPI_CXX_LIBRARIES}")
elseif(UNIX)
    find_package(MPI)
    if (MPI_FOUND)
        message(STATUS "Found MPI: ${MPI_CXX_LIBRARIES}")
    else()
        message(STATUS "Cannot find MPI, mpi wrappers (mpicc ect.) are used")
        set(MPI_CXX_INCLUDE_DIRS "")
        set(MPI_CXX_LIBRARIES "") 
        if (${CMAKE_CXX_COMPILER} MATCHES "icc.*$")  # ICC
            set(CMAKE_CXX_COMPILER "mpiicc")
        elseif(${CMAKE_CXX_COMPILER} MATCHES "gcc.*$")  # GCC
            set(CMAKE_CXX_COMPILER "mpicc")
        endif()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static_mpi")
    endif()
endif()


# OpenMP
if(USE_OMP)
    find_package(OpenMP)
    if (OPENMP_FOUND)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
    else()
        message(WARNING "Cannot find OpenMP")
    endif()
endif()


# mkl fft
if (USE_MKL)
    find_package(MKL)
    if (MKL_FOUND)
        set(FFT_INCLUDES ${MKL_INCLUDE_DIRS})
        set(FFT_LIBRARIES ${MKL_LIBRARIES})
        if (USE_MPI_FFT)
            set(FFTW_WRAPPER ${CMAKE_SOURCE_DIR}/wrappers/${CMAKE_STATIC_LIBRARY_PREFIX}fftw3x_cdft_lp64${CMAKE_STATIC_LIBRARY_SUFFIX})
            if (EXISTS ${FFTW_WRAPPER})
                set(FFT_LIBRARIES ${FFTW_WRAPPER} ${FFT_LIBRARIES})
            else()
                message(WARNING "Cannot find fftw wrapper")
            endif()
        endif()
    else()    
        message(WARNING "Cannot find MKL")
    endif()
endif()

# fftw
if (USE_FFTW)

    if(NOT FFTW_DIR)
    
        set(FFTW_VERSION 3.3.8)
        message(STATUS "Downloading fftw-${FFTW_VERSION}")
        include(ExternalProject)
        ExternalProject_Add(project_fftw
            URL "http://fftw.org/fftw-${FFTW_VERSION}.tar.gz"
            PREFIX ${CMAKE_CURRENT_BINARY_DIR}/fftw
            CMAKE_ARGS
                "-DCMAKE_CFLAGS=${CMAKE_C_FLAGS}"
                "-DBUILD_SHARED_LIBS=OFF"
                "-DENABLE_AVX2=ON"
                "-DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}/fftw"
                "-DENABLE_OPENMP=ON"
                "-DBUILD_TESTS=OFF"
               )
        install(DIRECTORY "${INSTALL_DIR}" DESTINATION .)
        
        set(FFTW3_LIB ${INSTALL_DIR}/fftw/lib/${CMAKE_STATIC_LIBRARY_PREFIX}fftw3${CMAKE_STATIC_LIBRARY_SUFFIX})
        set(FFTW3_OMP_LIB ${INSTALL_DIR}/fftw/lib/${CMAKE_STATIC_LIBRARY_PREFIX}fftw3_omp${CMAKE_STATIC_LIBRARY_SUFFIX})
        
        set(FFT_INCLUDES ${INSTALL_DIR}/fftw/include)
        set(FFT_LIBRARIES ${FFTW3_OMP_LIB} ${FFTW3_LIB})
        
    else() # FFTW_DIR

        set(FFTW_LIBRARIES_PATH ${FFTW_DIR}/lib)
        find_library(FFTW3_LIB NAMES ${CMAKE_STATIC_LIBRARY_PREFIX}fftw3${CMAKE_STATIC_LIBRARY_SUFFIX} fftw3 PATHS ${FFTW_LIBRARIES_PATH})
        find_library(FFTW3_OMP_LIB NAMES ${CMAKE_STATIC_LIBRARY_PREFIX}fftw3_omp${CMAKE_STATIC_LIBRARY_SUFFIX} fftw3_omp PATHS ${FFTW_LIBRARIES_PATH})
        if(USE_MPI_FFT)
            find_library(FFTW3_MPI_LIB NAMES ${CMAKE_STATIC_LIBRARY_PREFIX}fftw3_mpi${CMAKE_STATIC_LIBRARY_SUFFIX} fftw3_mpi PATHS ${FFTW_LIBRARIES_PATH})
        endif()
        
        if (FFTW3_LIB AND FFTW3_OMP_LIB)
            set(FFT_INCLUDES ${FFTW_DIR}/include)
            if(USE_MPI_FFT AND NOT FFTW3_MPI_LIB)
                message(WARNING "Cannot find fftw3_mpi lib")
            endif()
            set(FFT_LIBRARIES ${FFTW3_MPI_LIB} ${FFTW3_OMP_LIB} ${FFTW3_LIB})
        else()
            message(WARNING "Cannot find fftw libs")
        endif()
        
    endif() # FFTW_DIR
    
endif()


add_subdirectory(src)

if (USE_TESTS)
    add_subdirectory(test)
endif()
