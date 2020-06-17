#pragma once
#include "mpi_wrapper_3d.h"
#include <iostream>
#include <functional>
#include "status.h"
#include "parallel_scheme.h"

class MPIWorker {
private:
    static const int NFIELDS = 2;
    static const int NCOORDS = 3;

    MPIWrapperGrid * mpiWrapper;
    ParallelScheme * scheme;
    Grid3d* grid = 0;

    vec3<Array3d<double>> tmpArrays;  // for sum scheme

public:
    MPIWorker() {}

    MPIWorker(MPIWrapperGrid * mpiWrapper,
        ParallelScheme * scheme, Grid3d * grid) {
        initialize(mpiWrapper, scheme, grid);
    }

    Stat initialize(MPIWrapperGrid * mpiWrapper,
        ParallelScheme * scheme, Grid3d * grid);

    void exchangeGuard();
    void assembleResultsToRoot(Grid3d* commonGrid);

private:

    vec3<int> mod(vec3<int> a, vec3<int> b) {
        return ((a + b) % b);
    }

    // do smth in func for every field in grid
    // Array3d is array of field, int is tag of field
    void doForEveryField(Grid3d& grid, std::function<void(Array3d<double>&, int)> func);

    void exchangeTwoProcesses(Coordinate coord);

    void sendToRootProcess();
    void recvFromAllProcesses(Grid3d* commonGrid);

};