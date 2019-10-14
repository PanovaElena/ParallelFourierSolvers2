#pragma once
#include <memory>
#include <string>
#include "vector3d.h"
#include "status.h"
#include "simple_types.h"
#include "mask.h"

class ParallelScheme {
protected:
    vec3<int> domainSize;
    vec3<int> guardSize;
    vec3<Pair<Boards>> sendBoards;
    vec3<Pair<Boards>> recvBoards;
    Operation operation;

public:
    virtual ParallelScheme* clone() const = 0;
    virtual Stat initialize(vec3<int> domainSize, vec3<int> guardSize) {
        this->guardSize = guardSize;
        this->domainSize = domainSize;
        setBoardsForExchange();
        return Stat::OK;
    }

    virtual void setBoardsForExchange() = 0;
    virtual void setMask(const std::shared_ptr<Mask>& mask) = 0;

    vec3<int> getGuardSize() {
        return guardSize;
    }
    Pair<Boards> getSendBoards(Coordinate coord) {
        return sendBoards[coord];
    }
    Pair<Boards> getRecvBoards(Coordinate coord) {
        return recvBoards[coord];
    }
    vec3<Pair<Boards>> getSendBoards() {
        return sendBoards;
    }
    vec3<Pair<Boards>> getRecvBoards() {
        return recvBoards;
    }
    vec3<int> getDomainSize() {
        return domainSize;
    }
    Operation getOperation() {
        return operation;
    }

    virtual std::string to_string() = 0;

};


class ParallelSchemeSum: public ParallelScheme {
public:
    ParallelSchemeSum() {}
    ParallelSchemeSum(vec3<int> domainSize, vec3<int> guardSize) {
        initialize(domainSize, guardSize);
    }

    Stat initialize(vec3<int> domainSize, vec3<int> guardSize) override;

    void setBoardsForExchange() override;

    void setMask(const std::shared_ptr<Mask>& mask) override {
        mask->setDomainAndGuardSize(domainSize, guardSize);
    }

    virtual ParallelScheme* clone() const override {
        return new ParallelSchemeSum(*this);
    }

    std::string to_string() override {
        return "sum";
    }
};


class ParallelSchemeCopy: public ParallelScheme {
public:
    ParallelSchemeCopy() {}
    ParallelSchemeCopy(vec3<int> domainSize, vec3<int> guardSize) {
        initialize(domainSize, guardSize);
    }

    Stat initialize(vec3<int> domainSize, vec3<int> guardSize) override;

    void setBoardsForExchange() override;

    void setMask(const std::shared_ptr<Mask>& mask) override {
        mask->setDomainAndGuardSize(domainSize + guardSize, guardSize / 2);
    }

    virtual ParallelScheme* clone() const override {
        return new ParallelSchemeCopy(*this);
    }

    std::string to_string() override {
        return "copy";
    }
};
