#pragma once
#include <cmath>
#include <map>
#include <string>
#include <iostream>
#include "parallel_scheme.h"
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
    virtual Stat saveArgs(TaskParameters& p, Task task) = 0;

    Stat parseArgsForSequential(int& argc, char**& argv, TaskParameters& p) {
        return checkArgs(argc, argv, p, Task::sequential);
    }

    Stat parseArgsForParallel(int& argc, char**& argv, TaskParameters& p) {
        Stat s = checkArgs(argc, argv, p, Task::parallel);
        return s;
    }

protected:
    Stat parseArgs(int& argc, char**& argv, TaskParameters& p, Task task) {
        if (argc > 1 && std::string(argv[1]) == "-help") {
            help(p, task);
            return Stat::STOP;
        }
        else {
            for (int i = 1; i < argc && i + 1 < argc; i += 2)
                m.insert(std::pair<std::string, std::string>(argv[i], argv[i + 1]));
        }
        return Stat::OK;
    }

    Stat checkArgs(int& argc, char**& argv, TaskParameters& p, Task task) {
        Stat res = parseArgs(argc, argv, p, task);
        if (res != 0) return res;
        return saveArgs(p, task);
    }
};