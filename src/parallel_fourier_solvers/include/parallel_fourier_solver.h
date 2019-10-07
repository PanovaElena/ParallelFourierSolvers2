#pragma once
#include <memory>
#include "grid3d.h"
#include "status.h"
#include "grid_params.h"
#include "mask.h"
#include "filter.h"
#include "vector3d.h"
#include "process_info.h"
#include "status.h"
#include "mpi_worker.h"
#include "parallel_scheme.h"
#include "file_writer.h"
#include "field_solver.h"

class ParallelFourierSolver {

    Grid3d localGrid;
    std::shared_ptr<Mask> mask;
    std::shared_ptr<Filter> filter;

    vec3<int> globalSize;
    vec3<int> domainSize;
    vec3<int> guardSize;
    vec3<int> domainStart;  // in global localGrid
    vec3<int> leftGuardStart;
    vec3<int> rightGuardStart;

    ProcessInfo processInfo;
    MPIWorker mpiWorker;
    std::shared_ptr<FileWriter> fileWriter;
    std::shared_ptr<MPIWrapperGrid> mpiWrapper;
    std::shared_ptr<ParallelScheme> scheme;
    std::shared_ptr<FieldSolver> fieldSolver;

public:

    // common localGrid on zero process
    Stat initialize(Grid3d& globalGrid, vec3<int> guardSize,
        const Mask& mask, const Filter& filter, vec3<int> mpiSize,
        const ParallelScheme& scheme, const FieldSolver& fieldSolver,
        const FileWriter& fileWriter);

    // without common localGrid
    Stat initialize(const GridParams& globalGP, vec3<int> guardSize,
        const Mask& mask, const Filter& filter, vec3<int> mpiSize,
        const ParallelScheme& scheme, const FieldSolver& fieldSolver,
        const FileWriter& fileWriter);

    vec3<int> getFullSize() {
        return domainSize + 2 * guardSize;
    }

    void applyMask();
    void applyFilter();

    void run(int numIter, int maxIterBetweenExchange, double dt);

private:
    Stat init(const GridParams& globalGP, vec3<int> guardSize,
        const Mask& mask, const Filter& filter, vec3<int> size,
        const ParallelScheme& scheme, const FieldSolver& fieldSolver,
        const FileWriter& fileWriter);

    Stat validate();  // only after processInfo init

    void setDomainInfo(vec3<int> globalSize, vec3<int> guardSize);  // only after processInfo init

    void createGrid(const GridParams& globalGP);

    GridParams getLocalGridParams(const GridParams& globalGP);

    vec3<int> mod(vec3<int> a, vec3<int> b) {
        return ((a + b) % b);
    }

    void setMask();
    void setFilter();

    void doOneExchange(int numIter, double dt, bool ifWrite);

};