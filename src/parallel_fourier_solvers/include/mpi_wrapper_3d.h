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

public:
    MPIWrapper3d() : size(1, 1, 1) {}
    MPIWrapper3d(vec3<int> np) {
        initialize(np);
    }

    Stat initialize(vec3<int> mpiSize);

    Stat checkSize(vec3<int> size);

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
        vec3<int> dest, int tag, MPI_Request& request) const;

    Stat MPIRecvSubarray(Array3d<double>& arr, Boards boards,
        vec3<int> source, int tag) const;

protected:

    Stat createSubarray(MPI_Datatype& subarray, Boards board,
        vec3<int> arrSize) const;

    void freeSubarray(MPI_Datatype& subarray) const {
        MPI_Type_free(&subarray);
    }

    void createCartTopology();
};


class MPIWrapperGrid: public MPIWrapper3d {
    vec3<Pair<MPI_Datatype>> sendTypes;
    vec3<Pair<MPI_Datatype>> recvTypes;
    vec3<Pair<Boards>> sendBoards;
    vec3<Pair<Boards>> recvBoards;
    vec3<Array3d<double>> tmpArray;
    Operation operation;

public:
    MPIWrapperGrid(): MPIWrapper3d(vec3<int>(1, 1, 1)) {}
    MPIWrapperGrid(vec3<int> np): MPIWrapper3d(np) {}
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

    Stat initializeGuardExchangeInfo(vec3<Pair<Boards>> sendBoards,
        vec3<Pair<Boards>> recvBoards, vec3<int> globalArrSize, Operation op);

    void sendGuard(Array3d<double>& arr, Coordinate coord, Side side, MPIRequest& request) const;
    void recvGuard(Array3d<double>& arr, Coordinate coord, Side side) const;

protected:

    Stat createSubarrays(vec3<int> arrSize);
    void freeSubarrays();

    int getNeighbourRank(Coordinate coord, Side side) const;

    void accumulateLocalSubarrays(Array3d<double>& tmpArr,
        Array3d<double>& arr, const Boards& boards, Operation op) const;
};
