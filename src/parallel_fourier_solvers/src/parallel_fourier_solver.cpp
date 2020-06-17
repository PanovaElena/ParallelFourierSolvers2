#include "parallel_fourier_solver.h"
#include <omp.h>

Stat ParallelFourierSolver::initialize(const GridParams & globalGP, vec3<int> guardSize,
    const Mask& mask, const Filter& filter, vec3<int> mpiSize,
    const ParallelScheme& scheme, const FieldSolver& fieldSolver,
    const FileWriter& fileWriter, const Grid3d* globalGrid)
{
    this->scheme.reset(scheme.clone());
    this->mask.reset(mask.clone());
    this->filter.reset(filter.clone());
    this->mpiWrapper.reset(new MPIWrapperGrid(mpiSize));
    this->fieldSolver.reset(fieldSolver.clone());
    this->localGrid.reset(new Grid3d());
    this->fileWriter = fileWriter;

    if (this->mpiWrapper->initialize(mpiSize) == Stat::ERROR)
        return Stat::ERROR;
    
    setDomainInfo(globalGP.n, guardSize);
    if (validate() == Stat::ERROR)
        return Stat::ERROR;
    
    localGrid->create(getLocalGridParams(globalGP));
    this->fieldSolver->initialize(localGrid.get());
    if (globalGrid) this->setLocalGrid(globalGrid);
    else this->setLocalGrid(globalGP);

    if (this->scheme->initialize(this->domainSize, this->guardSize) == Stat::ERROR)
        return Stat::ERROR;
    if (this->mpiWorker.initialize(mpiWrapper.get(),
        this->scheme.get(), localGrid.get()) == Stat::ERROR)
        return Stat::ERROR;
    if (this->mpiWrapper->prepareExchanges(this->scheme->getSendBoards(),
        this->scheme->getRecvBoards(), localGrid->E.x.size(), this->scheme->getOperation())
        == Stat::ERROR)
        return Stat::ERROR;
    
    setMask();
    setFilter();

    return Stat::OK;
}

void ParallelFourierSolver::applyMask()
{
    mask->apply(localGrid.get());
}

void ParallelFourierSolver::applyFilter()
{
    filter->apply(localGrid.get());
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
    localGrid->setFields();
}

void ParallelFourierSolver::setLocalGrid(const Grid3d* globalGrid)
{
    GridParams localGP = localGrid->getGridParams();
    for (int i = 0; i < localGP.n.x; i++)
        for (int j = 0; j < localGP.n.y; j++)
            for (int k = 0; k < localGP.n.z; k++) {
                vec3<int> indexInGlobalGrid =
                    mod(vec3<int>(i, j, k) + domainStart - guardSize, globalGrid->getGridParams().n);
                localGrid->E.write(i, j, k, globalGrid->E(indexInGlobalGrid));
                localGrid->B.write(i, j, k, globalGrid->B(indexInGlobalGrid));
                localGrid->J.write(i, j, k, globalGrid->J(indexInGlobalGrid));
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
    scheme->setMask(mask.get());
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

    auto ifLast = [numExchanges, numIterBeforeLastExchange](int iter) {
        if (numIterBeforeLastExchange > 0)
            return iter == numExchanges;
        else return iter == numExchanges - 1;
    };

    for (int i = 0; i < numExchanges; i++)
        doOneExchange(maxIterBetweenExchange, dt, ifLast(i) && writeFile);
    if (numIterBeforeLastExchange > 0)
        doOneExchange(numIterBeforeLastExchange, dt, ifLast(numExchanges) && writeFile);
}

void ParallelFourierSolver::doOneExchange(int numIter, double dt, bool ifWrite)
{
    if (ifWrite)
        fileWriter.write(*localGrid, "1__rank_" + std::to_string((long long)MPIWrapper::MPIRank()) +
            "_before_mask", Double);

    double t1 = omp_get_wtime();
    mask->apply(localGrid.get());
    double t2 = omp_get_wtime();

    if (ifWrite)
        fileWriter.write(*localGrid, "2__rank_" + std::to_string((long long)MPIWrapper::MPIRank()) +
            "_after_mask", Double);

    double t3 = omp_get_wtime();
    fieldSolver->doFourierTransform(RtoC);
    double t4 = omp_get_wtime();

    for (int i = 0; i < numIter; i++)
        fieldSolver->run(dt);

    double t5 = omp_get_wtime();
    fieldSolver->doFourierTransform(CtoR);
    double t6 = omp_get_wtime();

    if (ifWrite)
        fileWriter.write(*localGrid, "3__rank_" + std::to_string((long long)MPIWrapper::MPIRank()) +
            "_before_last_exc", Double);

    double t7 = omp_get_wtime();
    mpiWorker.exchangeGuard();
    double t8 = omp_get_wtime();

    if (ifWrite)
        fileWriter.write(*localGrid, "4__rank_" + std::to_string((long long)MPIWrapper::MPIRank()) +
            "_after_last_exc", Double);

    //std::cout << "RANK " << MPIWrapper::MPIRank() << " : time mask is " << t2 - t1 << " s" << std::endl;
    //std::cout << "RANK " << MPIWrapper::MPIRank() << " : time RtoC is " << t4 - t3 << " s" << std::endl;
    //std::cout << "RANK " << MPIWrapper::MPIRank() << " : time PSATD is " << t5 - t4 << " s" << std::endl;
    //std::cout << "RANK " << MPIWrapper::MPIRank() << " : time CtoR is " << t6 - t5 << " s" << std::endl;
    //std::cout << "RANK " << MPIWrapper::MPIRank() << " : time exchange is " << t7 - t6 << " s" << std::endl;
}
