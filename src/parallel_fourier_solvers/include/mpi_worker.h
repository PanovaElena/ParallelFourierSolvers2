#pragma once
#include <memory>
#include <iostream>
#include "mpi_wrapper_3d.h"
#include "status.h"
#include "parallel_scheme.h"

class MPIWorker {
private:
    std::shared_ptr<MPIWrapperGrid> mpiWrapper;
    std::shared_ptr<ParallelScheme> scheme;
    Grid3d* grid = 0;

public:
    MPIWorker() {}

    MPIWorker(std::shared_ptr<MPIWrapperGrid>  mpiWrapper,
        std::shared_ptr<ParallelScheme> scheme, Grid3d * grid) {
        initialize(mpiWrapper, scheme, grid);
    }

    Stat initialize(std::shared_ptr<MPIWrapperGrid> mpiWrapper,
        std::shared_ptr<ParallelScheme> scheme, Grid3d * grid);

    void exchangeGuard();
    void assembleResultsToRoot(Grid3d& commonGrid);

private:

    vec3<int> mod(vec3<int> a, vec3<int> b) {
        return ((a + b) % b);
    }

    Stat send(const Boards& boards, vec3<int> dest, MPIWrapper::MPIRequest* const requests,
        Grid3d& gridFrom);
    Stat recv(const Boards& boards, vec3<int> source, Grid3d& gridTo);

    Stat sendGuard(Coordinate coord, Side side, MPIWrapper::MPIRequest* const requests);
    Stat recvGuard(Coordinate coord, Side side);
    void wait(MPIWrapper::MPIRequest* const requests);

    void exchangeTwoProcesses(Coordinate coord);

    void sendToRootProcess();
    void recvFromAllProcesses(Grid3d& commonGrid);

};