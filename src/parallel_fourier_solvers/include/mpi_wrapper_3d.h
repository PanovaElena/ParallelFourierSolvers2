#pragma once
#include <algorithm>
#include "mpi_wrapper.h"
#include "vector3d.h"
#include "status.h"
#include "simple_types.h"
#include "array3d.h"

class MPIWrapper3d : public MPIWrapper {
protected:
    vec3<int> size;
    MPI_Comm cartComm;
    bool ifCartCreated = false;

public:
    MPIWrapper3d() : size(1, 1, 1) {}
    MPIWrapper3d(vec3<int> np) {
        initialize(np);
    }
    virtual ~MPIWrapper3d() {
        if (ifCartCreated) MPI_Comm_free(&cartComm);
    }

    Stat initialize(vec3<int> mpiSize);

    Stat setSize(vec3<int> size);

    vec3<int> getSize() const {
        return size;
    }

    int getScalarSize() const {
        return MPIWrapper::MPISize(cartComm);
    }

    vec3<int> getRank() const {
        int coords[3];
        MPI_Cart_coords(cartComm, MPIWrapper::MPIRank(cartComm), 3, coords);
        return vec3<int>(coords[0], coords[1], coords[2]);
    }

    int getScalarRank() const {
        return MPIRank(cartComm);
    }

    vec3<int> getRank(int scalarRank) const {
        int coords[3];
        MPI_Cart_coords(cartComm, scalarRank, 3, coords);
        return vec3<int>(coords[0], coords[1], coords[2]);
    }

    int getScalarRank(vec3<int> vecRank) const {
        int coords[3] = {vecRank.x, vecRank.y, vecRank.z};
        int rank;
        MPI_Cart_rank(cartComm, coords, &rank);
        return rank;
    }

    void MPIISend(double*& buf, vec3<int> dest, int tag, MPI_Request& request) const {
        MPIWrapper::MPIISend(buf, getScalarSize(), getScalarRank(), tag, request, cartComm);
    }

    void MPIRecv(double*& buf, vec3<int> source, int tag) const {
        MPIWrapper::MPIRecv(buf, getScalarSize(), getScalarRank(), tag, cartComm);
    }

    void MPIBarrier() {
        MPIWrapper::MPIBarrier(cartComm);
    }

    Stat MPIISendSubarray(Array3d<double>& arr, Boards boards,
        vec3<int> dest, int tag) const;
    Stat MPISendSubarray(Array3d<double>& arr, Boards boards,
        vec3<int> dest, int tag) const;

    Stat MPIRecvSubarray(Array3d<double>& arr, Boards boards,
        vec3<int> source, int tag) const;

protected:

    Stat createSubarray(std::pair<MPI_Datatype, bool>& subarray, Boards board,
        vec3<int> arrSize) const;

    void freeSubarray(MPI_Datatype& subarray) const;

    void createCartTopology();
};


class MPIWrapperGrid: public MPIWrapper3d {
    vec3<Pair<std::pair<MPI_Datatype, bool>>> sendTypes;
    vec3<Pair<std::pair<MPI_Datatype, bool>>> recvTypes;

    vec3<Pair<Boards>> sendBoards;
    vec3<Pair<Boards>> recvBoards;

    Operation operation;

    vec3<bool> ifExchange;

public:
    MPIWrapperGrid() {
        create();
    }
    MPIWrapperGrid(vec3<int> np): MPIWrapper3d(np) {
        create();
    }
    ~MPIWrapperGrid() {
        freeSubarrays();
    }

    Stat initialize(vec3<int> mpiSize) {
        if (MPIWrapper3d::initialize(mpiSize) == Stat::ERROR)
            return Stat::ERROR;
        return Stat::OK;
    }

    Stat initialize(vec3<int> mpiSize, vec3<Pair<Boards>> sendBoards,
        vec3<Pair<Boards>> recvBoards, vec3<int> globalArrSize, Operation op);

    Stat prepareExchanges(vec3<Pair<Boards>> sendBoards,
        vec3<Pair<Boards>> recvBoards, vec3<int> globalArrSize, Operation op);

    void sendGuard(Array3d<double>& arr, Coordinate coord, Side side, int tag);
    void iSendGuard(Array3d<double>& arr, Coordinate coord, Side side, int tag);
    void recvGuard(Array3d<double>& arr, Coordinate coord, Side side, int tag);

    void accumulate(Array3d<double>& arr, Array3d<double>& tmpArray, Boards boards);

protected:
    void checkGuards();

    Stat createSubarrays(vec3<int> arrSize);
    void freeSubarrays();

    int getNeighbourRank(Coordinate coord, Side side) const;

    void create() {
        for (int coord = 0; coord < 3; coord++) {
            sendTypes[coord].left.second = false;
            sendTypes[coord].right.second = false;
            recvTypes[coord].left.second = false;
            recvTypes[coord].right.second = false;
        }
    }
};
