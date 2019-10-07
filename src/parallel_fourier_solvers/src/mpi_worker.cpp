#include "mpi_worker.h"

Stat MPIWorker::initialize(ProcessInfo & processInfo,
    MPIWrapperGrid & mpiWrapper, ParallelScheme & scheme, Grid3d & grid)
{
    this->processInfo.reset(&processInfo);
    this->scheme.reset(&scheme);
    this->mpiWrapper.reset(&mpiWrapper);
    this->grid.reset(&grid);
    return Stat::OK;
}

void MPIWorker::exchangeGuard()
{
    exchangeTwoProcesses(Coordinate::x);
    MPIWrapper::MPIBarrier();
    exchangeTwoProcesses(Coordinate::y);
    MPIWrapper::MPIBarrier();
    exchangeTwoProcesses(Coordinate::z);
    MPIWrapper::MPIBarrier();
}

void MPIWorker::assembleResultsToRoot(Grid3d & commonGrid)
{
    if (mpiWrapper->getRank() != vec3<int>(0))
        sendToRootProcess();
    else recvFromAllProcesses(commonGrid);
}

Stat MPIWorker::send(const Boards & boards, vec3<int> dest, int tag,
    MPI_Request & request, Grid3d & gridFrom)
{

    Stat s = mpiWrapper->MPIISendSubarray(gridFrom.E.x, boards, dest, tag, request);
    if (s == Stat::ERROR) return Stat::ERROR;
    mpiWrapper->MPIISendSubarray(gridFrom.E.y, boards, dest, tag, request);
    mpiWrapper->MPIISendSubarray(gridFrom.E.z, boards, dest, tag, request);
    mpiWrapper->MPIISendSubarray(gridFrom.B.x, boards, dest, tag, request);
    mpiWrapper->MPIISendSubarray(gridFrom.B.y, boards, dest, tag, request);
    mpiWrapper->MPIISendSubarray(gridFrom.B.z, boards, dest, tag, request);

    return Stat::OK;
}

Stat MPIWorker::recv(const Boards & boards, vec3<int> source, int tag, Grid3d & gridTo)
{

    Stat s = mpiWrapper->MPIRecvSubarray(gridTo.E.x, boards, source, tag);
    if (s == Stat::ERROR) return Stat::ERROR;
    mpiWrapper->MPIRecvSubarray(gridTo.E.y, boards, source, tag);
    mpiWrapper->MPIRecvSubarray(gridTo.E.z, boards, source, tag);
    mpiWrapper->MPIRecvSubarray(gridTo.B.x, boards, source, tag);
    mpiWrapper->MPIRecvSubarray(gridTo.B.y, boards, source, tag);
    mpiWrapper->MPIRecvSubarray(gridTo.B.z, boards, source, tag);

    return Stat::OK;
}

Stat MPIWorker::sendGuard(Coordinate coord, Side side, MPI_Request & request)
{

    mpiWrapper->sendGuard(grid->E.x, coord, side, request);
    mpiWrapper->sendGuard(grid->E.y, coord, side, request);
    mpiWrapper->sendGuard(grid->E.z, coord, side, request);
    mpiWrapper->sendGuard(grid->B.x, coord, side, request);
    mpiWrapper->sendGuard(grid->B.y, coord, side, request);
    mpiWrapper->sendGuard(grid->B.z, coord, side, request);

    return Stat::OK;
}

Stat MPIWorker::recvGuard(Coordinate coord, Side side)
{

    mpiWrapper->recvGuard(grid->E.x, coord, side);
    mpiWrapper->recvGuard(grid->E.y, coord, side);
    mpiWrapper->recvGuard(grid->E.z, coord, side);
    mpiWrapper->recvGuard(grid->B.x, coord, side);
    mpiWrapper->recvGuard(grid->B.y, coord, side);
    mpiWrapper->recvGuard(grid->B.z, coord, side);

    return Stat::OK;
}

void MPIWorker::exchangeTwoProcesses(Coordinate coord)
{
    if (scheme->getGuardSize()[(int)coord] == 0) return;

    MPIWrapper::MPIRequest request1, request2;

    // processInfo->message << "send left from " << scheme->getSendBoards[coord].left.left <<
    //     " to " << scheme->getSendBoards[coord].left.right << std::endl;
    sendGuard(coord, Side::left, request1);

    // processInfo->message << "send right from " << scheme->getSendBoards[coord].right.left <<
    //     " to " << scheme->getSendBoards[coord].right.right << std::endl;
    sendGuard(coord, Side::right, request2);

    // processInfo->message << "recv right from " << scheme->getRecvBoards[coord].right.left <<
    //     " to " << scheme->getRecvBoards[coord].right.right << std::endl;
    recvGuard(coord, Side::right);

    // processInfo->message << "recv left from " << scheme->getRecvBoards[coord].left.left <<
    //     " to " << scheme->getRecvBoards[coord].left.right << std::endl;
    recvGuard(coord, Side::left);

    MPIWrapper::MPIWait(request1);
    MPIWrapper::MPIWait(request2);
}

void MPIWorker::sendToRootProcess()
{
    vec3<int> dest(0, 0, 0);
    Boards boards(scheme->getGuardSize(), scheme->getGuardSize() + scheme->getDomainSize());
    MPIWrapper::MPIRequest request;
    send(boards, dest, 2, request, *grid);
    MPIWrapper::MPIWait(request);
}

void MPIWorker::recvFromAllProcesses(Grid3d & commonGrid)
{
    commonGrid = Grid3d(commonGrid.getGridParams());
    vec3<int> guardSize = scheme->getGuardSize();
    vec3<int> domainSize = scheme->getDomainSize();
    vec3<int> size = mpiWrapper->getSize();

    for (int i = 0; i < scheme->getDomainSize().x; i++)
        for (int j = 0; j < scheme->getDomainSize().y; j++)
            for (int k = 0; k < scheme->getDomainSize().z; k++) {
                commonGrid.E.write(i, j, k, grid->E(vec3<int>(i, j, k) + guardSize));
                commonGrid.B.write(i, j, k, grid->B(vec3<int>(i, j, k) + guardSize));
                commonGrid.J.write(i, j, k, grid->J(vec3<int>(i, j, k) + guardSize));
            }

    for (int i = 0; i < size.x; i++)
        for (int j = 0; j < size.y; j++)
            for (int k = 0; k < size.z; k++) {
                if (i == 0 && j == 0 && k == 0) continue;
                vec3<int> r(i, j, k);
                Boards boards(r*domainSize, (r + vec3<int>(1))*domainSize);
                recv(boards, r, 2, commonGrid);
            }
}
