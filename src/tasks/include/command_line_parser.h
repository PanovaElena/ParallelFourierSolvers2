#pragma once
#include "mpi_worker.h"
#include "mpi_wrapper_3d.h"
#include <cmath>
#include <map>
#include <string>
#include <iostream>
#include "mpi_worker_copy.h"
#include "mpi_worker_sum.h"
#include "task_parameters.h"
#include "status.h"

enum Task {
    sequential,
    parallel
};

class CommandLineParser {
protected:

    std::map<std::string, std::string> m;

public:

    virtual void help(TaskParameters& p, Task task) = 0;
    virtual Status saveArgs(TaskParameters& p, Task task) = 0;

    Status parseArgsForSequential(int& argc, char**& argv, TaskParameters& p) {
        return checkArgs(argc, argv, p, Task::sequential);
    }

    Status parseArgsForParallel(int& argc, char**& argv, TaskParameters& p) {
        Status s = checkArgs(argc, argv, p, Task::parallel);
        return s;
    }

protected:
    Status parseArgs(int& argc, char**& argv, TaskParameters& p, Task task) {
        if (argc > 1 && std::string(argv[1]) == "-help") {
            help(p, task);
            return Status::STOP;
        }
        else {
            for (int i = 1; i < argc && i + 1 < argc; i += 2)
                m.emplace(argv[i], argv[i + 1]);
        }
        return Status::OK;
    }

    Status checkArgs(int& argc, char**& argv, TaskParameters& p, Task task) {
        Status res = parseArgs(argc, argv, p, task);
        if (res != 0) return res;
        return saveArgs(p, task);
    }
};