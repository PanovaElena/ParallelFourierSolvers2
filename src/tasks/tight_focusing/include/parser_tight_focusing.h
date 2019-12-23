#pragma once
#include <map>
#include <iostream>
#include "task_parser.h"
#include "tight_focusing.h"
#include "mask.h"
#include "filter.h"
#include "field_solver.h"

class ParserTightFocusing : public TaskParser {
public:
    void help(TaskParameters& p1, Task task) override {
        TaskParser::help(p1, task);
        ParametersForTightFocusing& p = static_cast<ParametersForTightFocusing&>(p1);

        std::cout <<
            "-factor                     set factor, default value is " << p.factor << "\n" <<
            std::endl;
    }

    Stat saveArgs(TaskParameters& p, Task task) override {
        Stat s = TaskParser::saveArgs(p, task);
        if (s == Stat::ERROR || s == Stat::STOP) return s;

        ParametersForTightFocusing& params = static_cast<ParametersForTightFocusing&>(p);

        if (m.find("-factor") != m.end()) {
            params.factor = std::stoi(m.find("-factor")->second);
            params.gridParams.n = params.n_start * params.factor;
            params.gridParams.d = (params.gridParams.b - params.gridParams.a) / (vec3<>)params.gridParams.n;
        }

        return Stat::OK;
    }

};