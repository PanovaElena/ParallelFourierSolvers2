#include "mpi_worker.h"


Stat MPIWorker::initialize(std::shared_ptr<MPIWrapperGrid> mpiWrapper,
    std::shared_ptr<ParallelScheme> scheme, Grid3d * grid)
{
    this->scheme = scheme;
    this->mpiWrapper = mpiWrapper;
    this->grid = grid;
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
    if (mpiWrapper->getRank() != mpiWrapper->getRank(MPIWrapper::MPIROOT))
        sendToRootProcess();
    else recvFromAllProcesses(commonGrid);
}

Stat MPIWorker::send(const Boards & boards, vec3<int> dest,
    MPIWrapper::MPIRequest* const requests, Grid3d & gridFrom)
{
    Stat s = mpiWrapper->MPIISendSubarray(gridFrom.E.x, boards, dest, 10, requests[0]);
    if (s == Stat::ERROR) return Stat::ERROR;
    mpiWrapper->MPIISendSubarray(gridFrom.E.y, boards, dest, 20, requests[1]);
    mpiWrapper->MPIISendSubarray(gridFrom.E.z, boards, dest, 30, requests[2]);
    mpiWrapper->MPIISendSubarray(gridFrom.B.x, boards, dest, 40, requests[3]);
    mpiWrapper->MPIISendSubarray(gridFrom.B.y, boards, dest, 50, requests[4]);
    mpiWrapper->MPIISendSubarray(gridFrom.B.z, boards, dest, 60, requests[5]);

    return Stat::OK;
}

Stat MPIWorker::recv(const Boards & boards, vec3<int> source, Grid3d & gridTo)
{
    Stat s = mpiWrapper->MPIRecvSubarray(gridTo.E.x, boards, source, 10);
    if (s == Stat::ERROR) return Stat::ERROR;
    mpiWrapper->MPIRecvSubarray(gridTo.E.y, boards, source, 20);
    mpiWrapper->MPIRecvSubarray(gridTo.E.z, boards, source, 30);
    mpiWrapper->MPIRecvSubarray(gridTo.B.x, boards, source, 40);
    mpiWrapper->MPIRecvSubarray(gridTo.B.y, boards, source, 50);
    mpiWrapper->MPIRecvSubarray(gridTo.B.z, boards, source, 60);

    return Stat::OK;
}

Stat MPIWorker::sendGuard(Coordinate coord, Side side, MPIWrapper::MPIRequest* const request)
{
    mpiWrapper->sendGuard(grid->E.x, coord, side, 10, request[0]);
    mpiWrapper->sendGuard(grid->E.y, coord, side, 20, request[1]);
    mpiWrapper->sendGuard(grid->E.z, coord, side, 30, request[2]);
    mpiWrapper->sendGuard(grid->B.x, coord, side, 40, request[3]);
    mpiWrapper->sendGuard(grid->B.y, coord, side, 50, request[4]);
    mpiWrapper->sendGuard(grid->B.z, coord, side, 60, request[5]);

    return Stat::OK;
}

Stat MPIWorker::recvGuard(Coordinate coord, Side side)
{
    mpiWrapper->recvGuard(grid->E.x, coord, side, 10);
    mpiWrapper->recvGuard(grid->E.y, coord, side, 20);
    mpiWrapper->recvGuard(grid->E.z, coord, side, 30);
    mpiWrapper->recvGuard(grid->B.x, coord, side, 40);
    mpiWrapper->recvGuard(grid->B.y, coord, side, 50);
    mpiWrapper->recvGuard(grid->B.z, coord, side, 60);

    return Stat::OK;
}

void MPIWorker::wait(MPIWrapper::MPIRequest* const requests)
{
    mpiWrapper->MPIWait(requests[0]);
    mpiWrapper->MPIWait(requests[1]);
    mpiWrapper->MPIWait(requests[2]);
    mpiWrapper->MPIWait(requests[3]);
    mpiWrapper->MPIWait(requests[4]);
    mpiWrapper->MPIWait(requests[5]);
}

void MPIWorker::exchangeTwoProcesses(Coordinate coord)
{
    if (scheme->getGuardSize()[(int)coord] == 0) return;

    MPIWrapper::MPIRequest request1[6], request2[6];  // 6 fields requests

    // std::cout << mpiWrapper->getRank() << " send left from " << scheme->getSendBoards()[coord].left.left <<
    //     " to " << scheme->getSendBoards()[coord].left.right << std::endl;
    sendGuard(coord, Side::left, request1);

    // std::cout << mpiWrapper->getRank() << " send right from " << scheme->getSendBoards()[coord].right.left <<
    //     " to " << scheme->getSendBoards()[coord].right.right << std::endl;
    sendGuard(coord, Side::right, request2);

    // std::cout << mpiWrapper->getRank() << " recv right from " << scheme->getRecvBoards()[coord].right.left <<
    //     " to " << scheme->getRecvBoards()[coord].right.right << std::endl;
    recvGuard(coord, Side::right);

    // std::cout << mpiWrapper->getRank() << " recv left from " << scheme->getRecvBoards()[coord].left.left <<
    //     " to " << scheme->getRecvBoards()[coord].left.right << std::endl;
    recvGuard(coord, Side::left);

    wait(request1);
    wait(request2);
}

void MPIWorker::sendToRootProcess()
{
    vec3<int> dest = mpiWrapper->getRank(MPIWrapper::MPIROOT);
    MPIWrapper::MPIRequest requests[6];
    Boards boards(scheme->getGuardSize(), scheme->getGuardSize() + scheme->getDomainSize());
    send(boards, dest, requests, *grid);
    wait(requests);
}

void MPIWorker::recvFromAllProcesses(Grid3d & commonGrid)
{
    vec3<int> guardSize = scheme->getGuardSize();
    vec3<int> domainSize = scheme->getDomainSize();

    for (int i = 0; i < domainSize.x; i++)
        for (int j = 0; j < domainSize.y; j++)
            for (int k = 0; k < domainSize.z; k++) {
                commonGrid.E.write(i, j, k, grid->E(vec3<int>(i, j, k) + guardSize));
                commonGrid.B.write(i, j, k, grid->B(vec3<int>(i, j, k) + guardSize));
                commonGrid.J.write(i, j, k, grid->J(vec3<int>(i, j, k) + guardSize));
            }

    for (int i = 0; i < mpiWrapper->getScalarSize(); i++) {
        vec3<int> r = mpiWrapper->getRank(i);
        if (r == vec3<int>(0)) continue;
        Boards boards(r*domainSize, (r + vec3<int>(1))*domainSize);
        recv(boards, r, commonGrid);
    }
}
