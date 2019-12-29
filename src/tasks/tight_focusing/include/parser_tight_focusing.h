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
            "-strip                      set if strip, default value is " << p.ifStrip << "\n" <<
            std::endl;
    }

    Stat saveArgs(TaskParameters& p, Task task) override {
        Stat s = TaskParser::saveArgs(p, task);
        if (s == Stat::ERROR || s == Stat::STOP) return s;

        ParametersForTightFocusing& params = static_cast<ParametersForTightFocusing&>(p);

        if (m.find("-strip") != m.end() && std::stoi(m.find("-strip")->second) == 1)
            params.ifStrip = true;
        else params.ifStrip = false;

        if (m.find("-dir") != m.end()) {
            params.dir = m.find("-dir")->second;
            params.fileWriterEx.initialize(params.dir, E, x, Section(Section::XOY, Section::center));
            params.fileWriterEy.initialize(params.dir, E, y, Section(Section::XOY, Section::center));
            params.fileWriterEz.initialize(params.dir, E, z, Section(Section::XOY, Section::center));
        }

        if (m.find("-factor") != m.end())
            params.factor = std::stoi(m.find("-factor")->second);

        if (params.ifStrip == false)
            params.updateNotStrip();
        else params.updateStrip();

        return Stat::OK;
    }

};