#pragma once
#include "mpi.h"
#include <iostream>
#include <string>
#include <map>
#include "array3d.h"
#include "status.h"

class MPIWrapper {
public:
    static int MPISize(MPI_Comm comm = MPI_COMM_WORLD) {
        int size;
        MPI_Comm_size(comm, &size);
        return size;
    }
    static int MPIRank(MPI_Comm comm = MPI_COMM_WORLD) {
        int rank;
        MPI_Comm_rank(comm, &rank);
        return rank;
    }
    static void MPIISend(double*& buf, int size, int dest, int tag,
        MPI_Request& request, MPI_Comm comm = MPI_COMM_WORLD) {
        MPI_Isend(buf, size, MPI_DOUBLE, dest, tag, comm, &request);
    }
    static void  MPIRecv(double*& buf, int size, int source, int tag,
        MPI_Comm comm = MPI_COMM_WORLD) {
        MPI_Recv(buf, size, MPI_DOUBLE, source, tag, comm, MPI_STATUS_IGNORE);
    }
    static void MPIWait(MPI_Request& request) {
        MPI_Status status;
        MPI_Wait(&request, &status);
    }
    static void MPIInitialize(int& argc, char**& argv) {
        MPI_Init(&argc, &argv);
    }
    static void MPIFinalize() {
        MPI_Finalize();
    }
    static void MPIBarrier(MPI_Comm comm = MPI_COMM_WORLD) {
        MPI_Barrier(comm);
    }

    static void showMessage(std::string message) {
        std::cout << "rank " << MPIRank() << ": " << message << std::endl;
    }

    typedef MPI_Request MPIRequest;
};