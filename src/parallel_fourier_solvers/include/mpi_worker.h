#pragma once
#include <memory>
#include "mpi_wrapper_3d.h"
#include "process_info.h"
#include "status.h"
#include "parallel_scheme.h"

class MPIWorker {
private:
    std::shared_ptr<ProcessInfo> processInfo;
    std::shared_ptr<MPIWrapperGrid> mpiWrapper;
    std::shared_ptr<ParallelScheme> scheme;
    std::shared_ptr<Grid3d> grid;

public:
    MPIWorker() {}

    MPIWorker(ProcessInfo& processInfo, MPIWrapperGrid& mpiWrapper,
        ParallelScheme& scheme, Grid3d& grid) {
        initialize(processInfo, mpiWrapper, scheme, grid);
    }

    Stat initialize(ProcessInfo& processInfo, MPIWrapperGrid& mpiWrapper,
        ParallelScheme& scheme, Grid3d& grid);

    void exchangeGuard();
    void assembleResultsToRoot(Grid3d& commonGrid);

private:

    vec3<int> mod(vec3<int> a, vec3<int> b) {
        return ((a + b) % b);
    }

    Stat send(const Boards& boards, vec3<int> dest, int tag, MPI_Request& request,
        Grid3d& gridFrom);
    Stat recv(const Boards& boards, vec3<int> source, int tag, Grid3d& gridTo);

    Stat sendGuard(Coordinate coord, Side side, MPI_Request & request);
    Stat recvGuard(Coordinate coord, Side side);

    void exchangeTwoProcesses(Coordinate coord);

    void sendToRootProcess();
    void recvFromAllProcesses(Grid3d& commonGrid);

};