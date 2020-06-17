#include "mpi_worker.h"


Stat MPIWorker::initialize(MPIWrapperGrid * mpiWrapper,
    ParallelScheme * scheme, Grid3d * grid)
{
    this->scheme = scheme;
    this->mpiWrapper = mpiWrapper;
    this->grid = grid;

    if (dynamic_cast<ParallelSchemeSum*>(this->scheme)) {
        for (int coord = 0; coord < NCOORDS; coord++) {
            Boards boards = this->scheme->getSendBoards()[coord].left;
            tmpArrays[coord].create(boards.right - boards.left);
        }
    }
    return Stat::OK;
}

void MPIWorker::exchangeGuard()
{
    mpiWrapper->MPIBarrier();
    exchangeTwoProcesses(Coordinate::x);
    mpiWrapper->MPIBarrier();
    exchangeTwoProcesses(Coordinate::y);
    mpiWrapper->MPIBarrier();
    exchangeTwoProcesses(Coordinate::z);
    mpiWrapper->MPIBarrier();
}

void MPIWorker::exchangeTwoProcesses(Coordinate coord)
{
    if (scheme->getGuardSize()[(int)coord] == 0) return;

    if (dynamic_cast<ParallelSchemeCopy*>(scheme)) {

        doForEveryField(*grid, [this, coord](Array3d<double>& arr, int tag) {
            mpiWrapper->iSendGuard(arr, coord, Side::left, tag);
        });
        doForEveryField(*grid, [this, coord](Array3d<double>& arr, int tag) {
            mpiWrapper->iSendGuard(arr, coord, Side::right, tag);
        });
        doForEveryField(*grid, [this, coord](Array3d<double>& arr, int tag) {
            mpiWrapper->recvGuard(arr, coord, Side::right, tag);
        });
        doForEveryField(*grid, [this, coord](Array3d<double>& arr, int tag) {
            mpiWrapper->recvGuard(arr, coord, Side::left, tag);
        });
    }
    else if (dynamic_cast<ParallelSchemeSum*>(scheme)) {
        if (mpiWrapper->getRank()[coord] % 2 == 0)
        {
            doForEveryField(*grid, [this, coord](Array3d<double>& arr, int tag) {
                mpiWrapper->sendGuard(arr, coord, Side::left, tag);
                mpiWrapper->recvGuard(tmpArrays[coord], coord, Side::left, tag);
                mpiWrapper->accumulate(arr, tmpArrays[coord], scheme->getRecvBoards()[coord].left);
            });
            doForEveryField(*grid, [this, coord](Array3d<double>& arr, int tag) {
                mpiWrapper->sendGuard(arr, coord, Side::right, tag);
                mpiWrapper->recvGuard(tmpArrays[coord], coord, Side::right, tag);
                mpiWrapper->accumulate(arr, tmpArrays[coord], scheme->getRecvBoards()[coord].right);
            });
        }
        else {
            doForEveryField(*grid, [this, coord](Array3d<double>& arr, int tag) {
                mpiWrapper->recvGuard(tmpArrays[coord], coord, Side::right, tag);
                mpiWrapper->sendGuard(arr, coord, Side::right, tag);
                mpiWrapper->accumulate(arr, tmpArrays[coord], scheme->getRecvBoards()[coord].right);
            });
            doForEveryField(*grid, [this, coord](Array3d<double>& arr, int tag) {
                mpiWrapper->recvGuard(tmpArrays[coord], coord, Side::left, tag);
                mpiWrapper->sendGuard(arr, coord, Side::left, tag);
                mpiWrapper->accumulate(arr, tmpArrays[coord], scheme->getRecvBoards()[coord].left);
            });
        }
    }
}

void MPIWorker::assembleResultsToRoot(Grid3d* commonGrid)
{
    sendToRootProcess();
    recvFromAllProcesses(commonGrid);
    mpiWrapper->MPIBarrier();
}

void MPIWorker::sendToRootProcess()
{
    vec3<int> dest = mpiWrapper->getRank(MPIWrapper::MPIROOT);
    Boards boards(scheme->getGuardSize(), scheme->getGuardSize() + scheme->getDomainSize());
    doForEveryField(*grid, [this, boards, dest](Array3d<double>& arr, int tag) {
        mpiWrapper->MPIISendSubarray(arr, boards, dest, tag);
    });
}

void MPIWorker::recvFromAllProcesses(Grid3d* commonGrid)
{
    if (mpiWrapper->getScalarRank() == MPIWrapper::MPIROOT) {
        vec3<int> guardSize = scheme->getGuardSize();
        vec3<int> domainSize = scheme->getDomainSize();

        for (int i = 0; i < mpiWrapper->getScalarSize(); i++) {
            vec3<int> r = mpiWrapper->getRank(i);
            Boards boards(r*domainSize, (r + vec3<int>(1))*domainSize);
            doForEveryField(*commonGrid, [this, boards, r](Array3d<double>& arr, int tag) {
                mpiWrapper->MPIRecvSubarray(arr, boards, r, tag);
            });
        }
    }
}

void MPIWorker::doForEveryField(Grid3d& grid, std::function<void(Array3d<double>&, int)> func)
{
    const int tagEx = 10, tagEy = 20, tagEz = 30;
    const int tagBx = 100, tagBy = 200, tagBz = 300;
    func(grid.E.x, tagEx);
    func(grid.E.y, tagEy);
    func(grid.E.z, tagEz);
    func(grid.B.x, tagBx);
    func(grid.B.y, tagBy);
    func(grid.B.z, tagBz);
}