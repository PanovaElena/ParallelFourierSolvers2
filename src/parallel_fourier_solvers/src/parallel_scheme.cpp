#include "parallel_scheme.h"

Stat ParallelSchemeSum::initialize(vec3<int> domainSize, vec3<int> guardSize)
{
    if (ParallelScheme::initialize(domainSize, guardSize) == Stat::ERROR)
        return Stat::ERROR;
    operation = Operation::sum;
    return Stat::OK;
}

void ParallelSchemeSum::setBoardsForExchange()
{
    for (int i = 0; i < 3; i++) {
        vec3<int> fullSize = domainSize + 2 * guardSize;
        vec3<int> base = vec3<int>::getBaseVector(i);
        vec3<int> invBase(base.x == 0 ? 1 : 0,
            base.y == 0 ? 1 : 0, base.z == 0 ? 1 : 0);
        vec3<int> fullInvBase(invBase*fullSize);

        sendBoards[i].left.left = vec3<int>(0);
        sendBoards[i].left.right = 2 * guardSize * base + fullInvBase;
        sendBoards[i].right.left = domainSize * base;
        sendBoards[i].right.right = fullSize * base + fullInvBase;
        recvBoards[i].left.left = vec3<int>(0);
        recvBoards[i].left.right = 2 * guardSize * base + fullInvBase;
        recvBoards[i].right.left = domainSize * base;
        recvBoards[i].right.right = fullSize * base + fullInvBase;
    }
}

Stat ParallelSchemeCopy::initialize(vec3<int> domainSize, vec3<int> guardSize)
{
    if (ParallelScheme::initialize(domainSize, guardSize) == Stat::ERROR)
        return Stat::ERROR;
    operation = Operation::copy;
    return Stat::OK;
}

void ParallelSchemeCopy::setBoardsForExchange()
{
    for (int i = 0; i < 3; i++) {
        vec3<int> fullSize = domainSize + 2 * guardSize;
        vec3<int> base = vec3<int>::getBaseVector(i);
        vec3<int> invBase(base.x == 0 ? 1 : 0,
            base.y == 0 ? 1 : 0, base.z == 0 ? 1 : 0);
        vec3<int> fullInvBase(invBase*fullSize);

        sendBoards[i].left.left = guardSize * base;
        sendBoards[i].left.right = 2 * guardSize * base + fullInvBase;
        sendBoards[i].right.left = domainSize * base;
        sendBoards[i].right.right = (domainSize + guardSize) * base + fullInvBase;
        recvBoards[i].left.left = vec3<int>(0);
        recvBoards[i].left.right = guardSize * base + fullInvBase;
        recvBoards[i].right.left = (domainSize + guardSize) * base;
        recvBoards[i].right.right = fullSize;
    }
}
