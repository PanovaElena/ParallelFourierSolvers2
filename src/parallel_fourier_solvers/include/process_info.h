#pragma once
#include <string>
#include <sstream>
#include <fstream>
#include "vector3d.h"
#include "mpi_wrapper_3d.h"

class ProcessInfo {
    vec3<int> rank;
    vec3<int> size;

public:
    ProcessInfo() {}
    ProcessInfo(const MPIWrapperGrid& mw) {
        initialize(mw);
    }

    Stat initialize(const MPIWrapperGrid& mw) {
        rank = mw.getRank();
        size = mw.getSize();
        return Stat::OK;
    }

    ProcessInfo& operator=(const ProcessInfo& pi) {
        if (this != &pi) {
            rank = pi.rank;
            size = pi.size;
        }
        return *this;
    }

    vec3<int> getRank() {
        return rank;
    }
    vec3<int> getSize() {
        return size;
    }

    std::stringstream message;

    void displayMessage() {
        std::ofstream file("output_" + to_string(rank) + ".txt", std::ios_base::in);
        file << message.str();
        file.close();
    }


};