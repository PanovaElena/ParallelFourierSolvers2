#include "parallel_fourier_solver.h"

Stat ParallelFourierSolver::initialize(const Grid3d & globalGrid, vec3<int> guardSize,
    const Mask& mask, const Filter& filter, vec3<int> mpiSize,
    const ParallelScheme& scheme, const FieldSolver& fieldSolver,
    const FileWriter& fileWriter)
{
    return init(globalGrid.getGridParams(), guardSize, mask, filter,
        mpiSize, scheme, fieldSolver, fileWriter,
        [this, &globalGrid]() {this->setLocalGrid(globalGrid); });
}

Stat ParallelFourierSolver::initialize(const GridParams & globalGP, vec3<int> guardSize,
    const Mask& mask, const Filter& filter, vec3<int> mpiSize,
    const ParallelScheme& scheme, const FieldSolver& fieldSolver,
    const FileWriter& fileWriter)
{
    return init(globalGP, guardSize, mask, filter,
        mpiSize, scheme, fieldSolver, fileWriter,
        [this, &globalGP]() {this->setLocalGrid(globalGP); });
}

Stat ParallelFourierSolver::init(const GridParams & globalGP, vec3<int> guardSize,
    const Mask& mask, const Filter& filter, vec3<int> mpiSize,
    const ParallelScheme& scheme, const FieldSolver& fieldSolver,
    const FileWriter& fileWriter, std::function<void()> setLocalGridF)
{
    this->scheme.reset(scheme.clone());
    this->mask.reset(mask.clone());
    this->filter.reset(filter.clone());
    this->mpiWrapper.reset(new MPIWrapperGrid(mpiSize));
    this->fieldSolver.reset(fieldSolver.clone());
    this->fileWriter = fileWriter;

    if (this->mpiWrapper->initialize(mpiSize) == Stat::ERROR)
        return Stat::ERROR;
    
    setDomainInfo(globalGP.n, guardSize);
    if (validate() == Stat::ERROR)
        return Stat::ERROR;
    
    localGrid.initialize(getLocalGridParams(globalGP));
    this->fieldSolver->initialize(localGrid);
    localGrid.setShifts(fieldSolver.getSpatialShift(), fieldSolver.getTimeShift());
    setLocalGridF();

    if (this->scheme->initialize(this->domainSize, this->guardSize) == Stat::ERROR)
        return Stat::ERROR;
    if (this->mpiWorker.initialize(mpiWrapper,
        this->scheme, &localGrid) == Stat::ERROR)
        return Stat::ERROR;
    if (this->mpiWrapper->prepareExchanges(this->scheme->getSendBoards(),
        this->scheme->getRecvBoards(), localGrid.sizeReal(), this->scheme->getOperation())
        == Stat::ERROR)
        return Stat::ERROR;
    
    setMask();
    setFilter();

    return Stat::OK;
}

void ParallelFourierSolver::applyMask()
{
    mask->apply(localGrid);
}

void ParallelFourierSolver::applyFilter()
{
    filter->apply(localGrid);
}

Stat ParallelFourierSolver::validate()
{
    vec3<int> rank = mpiWrapper->getRank();
    vec3<int> size = mpiWrapper->getSize();
    if (globalSize % size != vec3<int>(0)) {
        std::cout << "ERROR: domain size % MPISize != 0";
        return Stat::ERROR;
    }
    if (globalSize.x < size.x || globalSize.y < size.y || globalSize.z < size.z) {
        std::cout << "ERROR: domain size is less than MPISize";
        return Stat::ERROR;
    }
    if (globalSize.x == 0 || globalSize.y == 0 || globalSize.z == 0) {
        std::cout << "ERROR: domain size is 0";
        return Stat::ERROR;
    }
    return Stat::OK;
}

void ParallelFourierSolver::setDomainInfo(vec3<int> globalSize, vec3<int> guardSize)
{
    vec3<int> rank = mpiWrapper->getRank();
    vec3<int> size = mpiWrapper->getSize();

    this->globalSize = globalSize;
    domainSize = globalSize / size;
    domainStart = domainSize * rank;

    this->guardSize = vec3<int>(size.x == 1 ? 0 : guardSize.x,
        size.y == 1 ? 0 : guardSize.y,
        size.z == 1 ? 0 : guardSize.z);

    leftGuardStart = domainStart - guardSize;
    if (rank.x == 0) leftGuardStart.x = globalSize.x - guardSize.x;
    if (rank.y == 0) leftGuardStart.y = globalSize.y - guardSize.y;
    if (rank.z == 0) leftGuardStart.z = globalSize.z - guardSize.z;

    rightGuardStart = domainStart + domainSize;
    if (rank.x == size.x - 1) rightGuardStart.x = 0;
    if (rank.y == size.y - 1) rightGuardStart.y = 0;
    if (rank.z == size.z - 1) rightGuardStart.z = 0;
}

void ParallelFourierSolver::setLocalGrid(const GridParams & globalGP)
{
    GridParams localGP = localGrid.getGridParams();
    for (int i = 0; i < localGP.n.x; i++)
        for (int j = 0; j < localGP.n.y; j++)
            for (int k = 0; k < localGP.n.z; k++) {
                vec3<int> indexInGlobalGrid =
                    mod(vec3<int>(i, j, k) + domainStart - guardSize, localGP.n);
                localGrid.E.write(i, j, k, globalGP.fE(indexInGlobalGrid, 0));
                localGrid.B.write(i, j, k, globalGP.fB(indexInGlobalGrid, 0));
                localGrid.J.write(i, j, k, globalGP.fJ(indexInGlobalGrid, 0));
            }
}

void ParallelFourierSolver::setLocalGrid(const Grid3d & globalGrid)
{
    GridParams localGP = localGrid.getGridParams();
    for (int i = 0; i < localGP.n.x; i++)
        for (int j = 0; j < localGP.n.y; j++)
            for (int k = 0; k < localGP.n.z; k++) {
                vec3<int> indexInGlobalGrid =
                    mod(vec3<int>(i, j, k) + domainStart - guardSize, localGP.n);
                localGrid.E.write(i, j, k, globalGrid.E(indexInGlobalGrid));
                localGrid.B.write(i, j, k, globalGrid.B(indexInGlobalGrid));
                localGrid.J.write(i, j, k, globalGrid.J(indexInGlobalGrid));
            }
}

GridParams ParallelFourierSolver::getLocalGridParams(const GridParams & globalGP)
{
    GridParams gp = globalGP;
    gp.a = (vec3<>)(domainStart - guardSize)*globalGP.d + globalGP.a;
    gp.n = getFullSize();

    return gp;
}

void ParallelFourierSolver::setMask()
{
    scheme->setMask(mask);
}

void ParallelFourierSolver::setFilter()
{
}

void ParallelFourierSolver::run(int numIter, int maxIterBetweenExchange, double dt,
    bool writeFile)
{
    if (numIter == 0 || maxIterBetweenExchange == 0) return;
    int numExchanges = numIter / maxIterBetweenExchange;
    int numIterBeforeLastExchange = numIter % maxIterBetweenExchange;

    for (int i = 0; i < numExchanges - 1; i++)
        doOneExchange(maxIterBetweenExchange, dt, false);

    if (numIterBeforeLastExchange != 0) {
        if (numExchanges != 0)
            doOneExchange(maxIterBetweenExchange, dt, false);
        doOneExchange(numIterBeforeLastExchange, dt, writeFile);
    }
    else doOneExchange(maxIterBetweenExchange, dt, writeFile);
}

void ParallelFourierSolver::doOneExchange(int numIter, double dt, bool ifWrite)
{
    if (ifWrite)
        fileWriter.write(localGrid, "1_iter_rank_" + std::to_string(MPIWrapper::MPIRank()) +
            "_before_mask.csv", Double);
    mask->apply(localGrid);
    if (ifWrite)
        fileWriter.write(localGrid, "2_iter_rank_" + std::to_string(MPIWrapper::MPIRank()) +
            "_after_mask.csv", Double);

    fieldSolver->doFourierTransform(RtoC);

    for (int i = 0; i < numIter; i++)
        fieldSolver->run(dt);

    fieldSolver->doFourierTransform(CtoR);

    if (ifWrite)
        fileWriter.write(localGrid, "3_iter_rank_" + std::to_string(MPIWrapper::MPIRank()) +
            "_before_last_exc.csv", Double);
    mpiWorker.exchangeGuard();
    if (ifWrite)
        fileWriter.write(localGrid, "4_iter_rank_" + std::to_string(MPIWrapper::MPIRank()) +
            "_after_last_exc.csv", Double);
}
