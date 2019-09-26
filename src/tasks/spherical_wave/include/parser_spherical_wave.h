#pragma once
#include <map>
#include "task_parser.h"
#include "spherical_wave.h"
#include "mask.h"
#include "filter.h"
#include "field_solver.h"

class ParserSphericalWave : public TaskParser {
public:

    void help(TaskParameters& p1, Task task) override {
        TaskParser::help(p1, task);
        ParametersForSphericalWave& p = static_cast<ParametersForSphericalWave&>(p1);

        std::cout <<
            "-scx, -scy, -scz            set coordinate of source, default value is " << p.source.coord << "\n" <<
            "-somega                     set frequency of source, default value is " << p.source.omega << "\n" <<
            "-somenv                     set frequency of envelop for source, default value is " << p.source.omegaEnv << "\n" <<
            "-stime                      set working time of source, default value is " << p.source.time << "\n" <<
            "-stimest                    set start time of source, default value is " << p.source.startTime << "\n" <<
            "-swx, -swy, -swz            set width of source, default value is " << p.source.width << "\n" <<
            std::endl;
    }

    Status saveArgs(TaskParameters& p, Task task) override {
        Status s = TaskParser::saveArgs(p, task);
        if (s == Status::ERROR || s == Status::STOP) return s;

        ParametersForSphericalWave& params = static_cast<ParametersForSphericalWave&>(p);

        if (m.find("-scx") != m.end()) params.source.coord.x = std::stod(m.find("-scx")->second);
        if (m.find("-scy") != m.end()) params.source.coord.y = std::stod(m.find("-scy")->second);
        if (m.find("-scz") != m.end()) params.source.coord.z = std::stod(m.find("-scz")->second);

        if (m.find("-swx") != m.end()) params.source.width.x = std::stod(m.find("-swx")->second);
        if (m.find("-swy") != m.end()) params.source.width.y = std::stod(m.find("-swy")->second);
        if (m.find("-swz") != m.end()) params.source.width.z = std::stod(m.find("-swz")->second);

        if (m.find("-somega") != m.end()) params.source.omega = std::stod(m.find("-somega")->second);
        if (m.find("-somenv") != m.end()) params.source.omegaEnv = std::stod(m.find("-somenv")->second);
        if (m.find("-stime") != m.end()) params.source.time = std::stod(m.find("-stime")->second);
        if (m.find("-stimest") != m.end()) params.source.startTime = std::stod(m.find("-stimest")->second);

        return Status::OK;

    }
};