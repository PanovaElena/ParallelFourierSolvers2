#include "mpi_wrapper_3d.h"
#include <sstream>

Stat MPIWrapper3d::initialize(vec3<int> mpiSize)
{
    if (ifCartCreated) {
        MPI_Comm_free(&cartComm);
    }
    if (setSize(mpiSize) == Stat::ERROR)
        return Stat::ERROR;
    createCartTopology();
    ifCartCreated = true;
    return Stat::OK;
}

Stat MPIWrapper3d::setSize(vec3<int> size)
{
    if (size.x*size.y*size.z > MPIWrapper::MPISize())
        return Stat::ERROR;
    if (size.x*size.y*size.z == 0)
        return Stat::ERROR;
    this->size = size;
    return Stat::OK;
}

Stat MPIWrapper3d::MPISendSubarray(Array3d<double>& arr, Boards boards,
    vec3<int> dest, int tag, bool ifBlock) const
{
    MPI_Datatype subarray;
    if (createSubarray(subarray, boards, arr.size()) == Stat::ERROR)
        return Stat::ERROR;

    if (ifBlock) {
        MPI_Send(&(arr[0]), 1, subarray, getScalarRank(dest), tag, cartComm);
    }
    else {
        MPIRequest request;
        MPI_Isend(&(arr[0]), 1, subarray, getScalarRank(dest), tag, cartComm, &request);
        MPI_Request_free(&request);
    }

    freeSubarray(subarray);
    return Stat::OK;
}

Stat MPIWrapper3d::MPIRecvSubarray(Array3d<double>& arr, Boards boards, vec3<int> source, int tag) const
{
    MPI_Datatype subarray;
    if (createSubarray(subarray, boards, arr.size()) == Stat::ERROR)
        return Stat::ERROR;
    MPI_Recv(&(arr[0]), 1, subarray, getScalarRank(source), tag, cartComm,
        MPI_STATUS_IGNORE);
    freeSubarray(subarray);
    return Stat::OK;
}

Stat MPIWrapper3d::createSubarray(MPI_Datatype & subarray, Boards board, vec3<int> arrSize) const
{
    if (board.right - board.left < vec3<int>(0))
        return Stat::ERROR;

    int starts[3] = { board.left.x, board.left.y, board.left.z };
    int subsizes[3] = { board.right.x - board.left.x,
        board.right.y - board.left.y, board.right.z - board.left.z };
    int bigsizes[3] = { arrSize.x, arrSize.y, arrSize.z };

    MPI_Type_create_subarray(3, bigsizes, subsizes, starts,
        MPI_ORDER_C, MPI_DOUBLE, &subarray);
    MPI_Type_commit(&subarray);
    return Stat::OK;
}

void MPIWrapper3d::freeSubarray(MPI_Datatype & subarray) const
{
    MPI_Type_free(&subarray);
}

void MPIWrapper3d::createCartTopology()
{
    int sizeArr[3] = { size.x, size.y, size.z };
    int periods[3] = { true, true, true };
    MPI_Cart_create(MPI_COMM_WORLD, 3, sizeArr, periods, false, &cartComm);
}

Stat MPIWrapperGrid::initialize(vec3<int> mpiSize, vec3<Pair<Boards>> sendBoards,
    vec3<Pair<Boards>> recvBoards, vec3<int> globalArrSize, Operation op)
{
    if (MPIWrapper3d::initialize(mpiSize) == Stat::ERROR)
        return Stat::ERROR;
    if (prepare(sendBoards, recvBoards, globalArrSize, op)
        == Stat::ERROR)
        return Stat::ERROR;
    return Stat::OK;
}

Stat MPIWrapperGrid::prepare(vec3<Pair<Boards>> sendBoards,
    vec3<Pair<Boards>> recvBoards, vec3<int> globalArrSize, Operation op)
{
    this->sendBoards = sendBoards;
    this->recvBoards = recvBoards;
    this->operation = op;

    checkGuards();

    if (createSubarrays(globalArrSize) == Stat::ERROR)
        return Stat::ERROR;
    return Stat::OK;
}

void MPIWrapperGrid::checkGuards()
{
    for (int i = 0; i < 3; i++) {
        vec3<int> delta = sendBoards[i].left.right -
            sendBoards[i].left.left;
        if (delta.x == 0 || delta.y == 0 || delta.z == 0)
            ifExchange[i] = false;
        else ifExchange[i] = true;
    }
}

void MPIWrapperGrid::sendGuard(Array3d<double>& arr, Coordinate coord,
    Side side, int tag, bool ifBlock)
{
    if (!ifExchange[coord]) return;
    MPI_Datatype& subarray = sendTypes[coord].getElem(side);
    if (ifBlock) {
        MPI_Send(&(arr[0]), 1, subarray, getNeighbourRank(coord, side),
            tag, cartComm);
    }
    else {
        MPIRequest request;
        MPI_Isend(&(arr[0]), 1, subarray, getNeighbourRank(coord, side),
            tag, cartComm, &request);
        MPI_Request_free(&request);
    }
}

void MPIWrapperGrid::recvGuard(Array3d<double>& arr,
    Coordinate coord, Side side, int tag)
{
    if (!ifExchange[coord]) return;
    MPI_Datatype& subarray = recvTypes[coord].getElem(side);
    MPI_Recv(&(arr[0]), 1, subarray, getNeighbourRank(coord, side),
        tag, cartComm, MPI_STATUS_IGNORE);
}

Stat MPIWrapperGrid::createSubarrays(vec3<int> arrSize)
{
    if (ifSubarrInit) freeSubarrays();

    for (int coord = 0; coord < 3; coord++) {
        if (!ifExchange[coord]) continue;

        // send
        if (sendBoards[coord].left.right - sendBoards[coord].left.left !=
            sendBoards[coord].right.right - sendBoards[coord].right.left)
            return Stat::ERROR;

        createSubarray(sendTypes[coord].left, sendBoards[coord].left, arrSize);
        ifSendTypeCreated[coord].left = true;

        createSubarray(sendTypes[coord].right, sendBoards[coord].right, arrSize);
        ifSendTypeCreated[coord].right = true;

        // recv
        if (recvBoards[coord].left.right - recvBoards[coord].left.left !=
            recvBoards[coord].right.right - recvBoards[coord].right.left)
            return Stat::ERROR;

        if (operation == Operation::copy) {
            createSubarray(recvTypes[coord].left, recvBoards[coord].left, arrSize);
            ifRecvTypeCreated[coord].left = true;

            createSubarray(recvTypes[coord].right, recvBoards[coord].right, arrSize);
            ifRecvTypeCreated[coord].right = true;
        }
        else {
            vec3<int> n = recvBoards[coord].left.right - recvBoards[coord].left.left;

            createSubarray(recvTypes[coord].left, Boards(vec3<int>(0), n), n);
            ifRecvTypeCreated[coord].left = true;

            createSubarray(recvTypes[coord].right, Boards(vec3<int>(0), n), n);
            ifRecvTypeCreated[coord].right = true;
        }
    }
    ifSubarrInit = true;
    return Stat::OK;
}

void MPIWrapperGrid::freeSubarrays()
{
    for (int coord = 0; coord < 3; coord++) {
        if (ifSendTypeCreated[coord].left) {
            freeSubarray(sendTypes[coord].left);
            ifSendTypeCreated[coord].left = false;
        }
        if (ifSendTypeCreated[coord].right) {
            freeSubarray(sendTypes[coord].right);
            ifSendTypeCreated[coord].right = false;
        }
        if (ifRecvTypeCreated[coord].left) {
            freeSubarray(recvTypes[coord].left);
            ifRecvTypeCreated[coord].left = false;
        }
        if (ifRecvTypeCreated[coord].right) {
            freeSubarray(recvTypes[coord].right);
            ifRecvTypeCreated[coord].right = false;
        }
    }
    ifSubarrInit = false;
}

int MPIWrapperGrid::getNeighbourRank(Coordinate coord, Side side) const
{
    int source, dest;
    MPI_Cart_shift(cartComm, (int)coord, 1, &source, &dest);
    switch (side) {
    case Side::left:
        return source;
    default:
        return dest;
    }
}

void MPIWrapperGrid::accumulate(Array3d<double>& arrTo, Array3d<double>& arrFrom, Boards boards)
{
    if (operation == Operation::sum) {
        bool f = true;
#pragma omp parallel for
        for (int i = boards.left.x; i < boards.right.x; i++)
            for (int j = boards.left.y; j < boards.right.y; j++)
                // #pragma omp simd
                for (int k = boards.left.z; k < boards.right.z; k++)
                    arrTo(i, j, k) += arrFrom(i - boards.left.x,
                        j - boards.left.y, k - boards.left.z);
    }
}
