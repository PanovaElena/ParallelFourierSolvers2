#pragma once
#include "mpi.h"
#include <iostream>
#include <string>
#include "array3d.h"

class MPIWrapper {
public:
    static int MPISize() {
        int size;
        MPI_Comm_size(MPI_COMM_WORLD, &size);
        return size;
    }
    static int MPIRank() {
        int rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        return rank;
    }
    static void MPIISend(double*& buf, int size, int dest, int tag, MPI_Request& request) {
        MPI_Isend(buf, size, MPI_DOUBLE, dest, tag, MPI_COMM_WORLD, &request);
    }
    static void  MPIRecv(double*& buf, int size, int source, int tag) {
        MPI_Recv(buf, size, MPI_DOUBLE, source, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
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
    static void MPIBarrier() {
        MPI_Barrier(MPI_COMM_WORLD);
    }

    static void showMessage(std::string message) {
        std::cout << "rank " << MPIRank() << ": " << message << std::endl;
    }
};