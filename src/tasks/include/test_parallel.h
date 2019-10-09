#pragma once
#include <string>
#include "file_writer.h"
#include "status.h"

class TestParallel {
public:

    std::string nameFileFirstSteps;
    std::string nameFileSecondSteps;

    std::string nameFileAfterDivision;
    std::string nameFileAfterExchange;

    virtual Stat testBody() = 0;

    void setNameFiles() {
        nameFileFirstSteps = "first_sequential_steps.csv";
        nameFileSecondSteps = "parallel_result.csv";
        nameFileAfterDivision = "after_division_into_domains_rank_" + std::to_string((long long)MPIWrapper::MPIRank()) + ".csv";
        nameFileAfterExchange = "after_last_exchange_rank_" + std::to_string((long long)MPIWrapper::MPIRank()) + ".csv";
    }

};
