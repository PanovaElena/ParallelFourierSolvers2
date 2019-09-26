#pragma once
#include "mpi_wrapper.h"
#include "vector3d.h"
#include "status.h"

class MPIWrapper3d {
    int sizeX = 1, sizeY = 1, sizeZ = 1;
public:
    MPIWrapper3d() {}
    MPIWrapper3d(vec3<int> np) {
        sizeX = np.x;
        sizeY = np.y;
        sizeZ = np.z;
    }

    Status checkAndSetSize(int _sizeX, int _sizeY, int _sizeZ) {
        if (_sizeX*_sizeY*_sizeZ != MPIWrapper::MPISize())
            return Status::ERROR;
        setSize(_sizeX, _sizeY, _sizeZ);
        return Status::OK;
    }

    void setSize(int _sizeX, int _sizeY, int _sizeZ) {
        sizeX = _sizeX;
        sizeY = _sizeY;
        sizeZ = _sizeZ;
    }

    vec3<int> MPISize() {
        return vec3<int>(sizeX, sizeY, sizeZ);
    }
    int getScalarRank(vec3<int> rank) {
        return (rank.x*sizeY + rank.y)*sizeZ + rank.z;
    }
    int getTag(vec3<int> dim) {
        int N = 3;
        dim = dim + vec3<int>(N);
        return (dim.x*N + dim.y)*N + dim.z;
    }
    vec3<int> getVecRank(int rank) {
        int z = rank%sizeZ;
        int tmp = rank / sizeZ;
        int y = tmp%sizeY;
        int x = tmp / sizeY;
        return vec3<int>(x, y, z);
    }
    vec3<int> MPIRank() {
        int rank = MPIWrapper::MPIRank();
        return getVecRank(rank);
    }
    void MPIISend(double*& buf, int size, vec3<int> dest, int tag, MPI_Request& request) {
        MPIWrapper::MPIISend(buf, size, getScalarRank(dest), tag, request);
    }
    void MPIRecv(double*& buf, int size, vec3<int> source, int tag) {
        MPIWrapper::MPIRecv(buf, size, getScalarRank(source), tag);
    }
    void MPIISendSubarray(Array3d<double>& arr, vec3<int> n1, vec3<int> n2,
        vec3<int> dest, int tag, MPI_Request& request) {
        MPI_Datatype mysubarray;

        int starts[3] = { n1.x, n1.y, n1.z };
        int subsizes[3] = { n2.x - n1.x, n2.y - n1.y, n2.z - n1.z };
        int bigsizes[3] = { arr.size().x, arr.size().y, arr.size().z };

        MPI_Type_create_subarray(3, bigsizes, subsizes, starts,
            MPI_ORDER_C, MPI_DOUBLE, &mysubarray);
        MPI_Type_commit(&mysubarray);

        MPI_Isend(&(arr[0]), 1, mysubarray, getScalarRank(dest), tag, MPI_COMM_WORLD, &request);
        MPI_Type_free(&mysubarray);
    }
    void MPIIRecvSubarray(Array3d<double>& arr, vec3<int> n1, vec3<int> n2,
        vec3<int> source, int tag) {
        MPI_Datatype mysubarray;

        int starts[3] = { n1.x, n1.y, n1.z };
        int subsizes[3] = { n2.x - n1.x, n2.y - n1.y, n2.z - n1.z };
        int bigsizes[3] = { arr.size().x, arr.size().y, arr.size().z };

        MPI_Type_create_subarray(3, bigsizes, subsizes, starts,
            MPI_ORDER_C, MPI_DOUBLE, &mysubarray);
        MPI_Type_commit(&mysubarray);

        MPI_Recv(&(arr[0]), 1, mysubarray, getScalarRank(source), tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Type_free(&mysubarray);
    }
};
