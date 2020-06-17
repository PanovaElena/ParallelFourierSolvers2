#pragma once
#include "mpi_worker.h"
#include <memory>
#include <functional>
#include "grid3d.h"
#include "status.h"
#include "grid_params.h"
#include "mask.h"
#include "filter.h"
#include "vector3d.h"
#include "status.h"
#include "parallel_scheme.h"
#include "file_writer.h"
#include "field_solver.h"

class ParallelFourierSolver {

    std::unique_ptr<Grid3d> localGrid;
    std::unique_ptr<Mask> mask;
    std::unique_ptr<Filter> filter;

    vec3<int> globalSize;
    vec3<int> domainSize;
    vec3<int> guardSize;
    vec3<int> domainStart;  // in global localGrid
    vec3<int> leftGuardStart;
    vec3<int> rightGuardStart;

    MPIWorker mpiWorker;
    FileWriter fileWriter;
    std::unique_ptr<MPIWrapperGrid> mpiWrapper;
    std::unique_ptr<ParallelScheme> scheme;
    std::unique_ptr<FieldSolver> fieldSolver;

public:

    Stat initialize(const GridParams& globalGP, vec3<int> guardSize,
        const Mask& mask, const Filter& filter, vec3<int> mpiSize,
        const ParallelScheme& scheme, const FieldSolver& fieldSolver,
        const FileWriter& fileWriter, const Grid3d* globalGrid = 0);

    vec3<int> getFullSize() {
        return domainSize + 2 * guardSize;
    }

    void applyMask();
    void applyFilter();

    void run(int numIter, int maxIterBetweenExchange, double dt, bool writeFile = true);
    void assembleResults(Grid3d* commonGrid) {
        mpiWorker.assembleResultsToRoot(commonGrid);
    }

    vec3<int> getDomainSize() {
        return domainSize;
    }
    vec3<int> getGuardSize() {
        return guardSize;
    }
    vec3<int> getDomainStart() {
        return domainStart;
    }
    vec3<int> getDomainEnd() {
        return domainStart + domainSize;
    }

    Grid3d* getGrid() {
        return localGrid.get();
    }

    FieldSolver& getFieldSolver() {
        return *fieldSolver;
    }

    void writeFile(std::string fileName) {
        fileWriter.write(*localGrid, fileName);
    }

private:

    Stat validate();  // only after processInfo init

    void setDomainInfo(vec3<int> globalSize, vec3<int> guardSize);  // only after processInfo init

    void setLocalGrid(const GridParams& globalGP);
    void setLocalGrid(const Grid3d* globalGrid);

    GridParams getLocalGridParams(const GridParams& globalGP);

    vec3<int> mod(vec3<int> a, vec3<int> b) {
        return ((a + b) % b);
    }

    void setMask();
    void setFilter();

    void doOneExchange(int numIter, double dt, bool ifWrite);
};