#include "parallel_fourier_solver.h"

Stat ParallelFourierSolver::initialize(Grid3d & globalGrid, vec3<int> guardSize,
    const Mask& mask, const Filter& filter, vec3<int> mpiSize,
    const ParallelScheme& scheme)
{
    return init(globalGrid.getGridParams(), guardSize, mask, filter, mpiSize, scheme);
}

Stat ParallelFourierSolver::initialize(const GridParams & globalGP, vec3<int> guardSize,
    const Mask& mask, const Filter& filter, vec3<int> mpiSize,
    const ParallelScheme& scheme)
{
    return init(globalGP, guardSize, mask, filter, mpiSize, scheme);
}

Stat ParallelFourierSolver::init(const GridParams & globalGP, vec3<int> guardSize,
    const Mask& mask, const Filter& filter, vec3<int> size, const ParallelScheme& scheme)
{
    this->scheme.reset(scheme.clone());
    this->mask.reset(mask.clone());
    this->filter.reset(filter.clone());
    this->mpiWrapper.reset(new MPIWrapperGrid());

    if (this->mpiWrapper->initialize(size) == Stat::ERROR)
        return Stat::ERROR;
    if (this->processInfo.initialize(*mpiWrapper) == Stat::ERROR)
        return Stat::ERROR;

    setDomainInfo(globalGP.n, guardSize);
    if (validate() == Stat::ERROR)
        return Stat::ERROR;

    createGrid(globalGP);

    if (this->scheme->initialize(this->domainSize, this->guardSize) == Stat::ERROR)
        return Stat::ERROR;
    if (this->mpiWorker.initialize(processInfo, *mpiWrapper,
        *(this->scheme), localGrid) == Stat::ERROR)
        return Stat::ERROR;
    if (this->mpiWrapper->initializeGuardExchangeInfo(this->scheme->getSendBoards(),
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
    vec3<int> rank = processInfo.getRank();
    vec3<int> size = processInfo.getSize();
    if (globalSize % size != vec3<int>(0)) {
        processInfo.message << "ERROR: domain size % MPISize != 0";
        return Stat::ERROR;
    }
    if (globalSize.x < size.x || globalSize.y < size.y || globalSize.z < size.z) {
        processInfo.message << "ERROR: domain size is less than MPISize";
        return Stat::ERROR;
    }
    if (globalSize.x == 0 || globalSize.y == 0 || globalSize.z == 0) {
        processInfo.message << "ERROR: domain size is 0";
        return Stat::ERROR;
    }
    return Stat::OK;
}

void ParallelFourierSolver::setDomainInfo(vec3<int> globalSize, vec3<int> guardSize)
{
    vec3<int> rank = processInfo.getRank();
    vec3<int> size = processInfo.getSize();

    this->globalSize = globalSize;
    domainSize = globalSize / size;
    domainStart = domainSize * rank;
    this->guardSize = guardSize;

    leftGuardStart = domainStart - guardSize;
    if (rank.x == 0) leftGuardStart.x = globalSize.x - guardSize.x;
    if (rank.y == 0) leftGuardStart.y = globalSize.y - guardSize.y;
    if (rank.z == 0) leftGuardStart.z = globalSize.z - guardSize.z;

    rightGuardStart = domainStart + domainSize;
    if (rank.x == size.x - 1) rightGuardStart.x = 0;
    if (rank.y == size.y - 1) rightGuardStart.y = 0;
    if (rank.z == size.z - 1) rightGuardStart.z = 0;
}

void ParallelFourierSolver::createGrid(const GridParams & globalGP)
{
    GridParams localGP = getLocalGridParams(globalGP);
    localGrid = Grid3d(getLocalGridParams(localGP));
    for (int i = 0; i < localGP.n.x; i++)
        for (int j = 0; j < localGP.n.y; j++)
            for (int k = 0; k < localGP.n.z; k++) {
                vec3<int> indexInGlobalGrid =
                    mod(vec3<int>(i, j, k) + domainStart - guardSize, localGP.n);
                localGrid.E.write(i, j, k, globalGP.fE(indexInGlobalGrid, 0.0));
                localGrid.B.write(i, j, k, globalGP.fB(indexInGlobalGrid, 0.0));
                localGrid.J.write(i, j, k, globalGP.fJ(indexInGlobalGrid, 0.0));
            }
}

void ParallelFourierSolver::setMask()
{
    scheme->setMask(mask);
}

void ParallelFourierSolver::setFilter()
{
}

GridParams ParallelFourierSolver::getLocalGridParams(const GridParams & globalGP)
{
    GridParams gp = globalGP;
    gp.a = (vec3<>)(domainStart - guardSize)*globalGP.d + globalGP.a;
    gp.n = getFullDomainSize();
    return gp;
}
