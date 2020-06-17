#pragma once
#include <cmath>
#include <map>
#include <string>
#include <iostream>
#include <vector>
#include <memory>
#include "simple_types.h"
#include "parallel_scheme.h"
#include "status.h"
#include "field_solver.h"

class CommandLineParser {
protected:
    std::map<std::string, std::string> params;
    std::map<std::string, std::string> args;

public:

    CommandLineParser(const std::map<std::string, std::string>& params) : params(params)
    {}

public:

    void printHelp() {
        auto firstPart = [](const std::string& arg) {
            std::string str = arg;
            for (int i = 0; i < (15 - arg.size()); i++) str += " ";
            return str;
        };
        std::string output = "-" + firstPart("help") + "get help" + "\n";
        for (auto it = params.begin(); it != params.end(); it++)
            output += "-" + firstPart(it->first) + "set " + it->second + "\n";
        std::cout << output;
    }

    Stat parseArgs(int& argc, char**& argv) {
        if (argc == 2 && std::string(argv[1]) == "-help") {
            printHelp();
            return Stat::STOP;
        }
        for (int i = 1; i < argc && i + 1 < argc; i += 2) {
            std::string param = argv[i];
            if (param[0] != '-') {
                std::cout << "PARSE ERROR: arg " + std::to_string(i) + " is incorrect\n";
                return Stat::ERROR;
            }
            args.insert(std::make_pair(param.substr(1), std::string(argv[i + 1])));
        }
        for (auto it = args.begin(); it != args.end(); it++)
            if (params.find(it->first) == params.end())
                std::cout << "PARSE WARNING: arg " + it->first + " does not exist\n";
        return Stat::OK;
    }

    void getArgInt(const std::string& arg, int& var) {
        if (args.find(arg) != args.end())
            var = std::stoi(args.find(arg)->second);
    }

    void getArgDouble(const std::string& arg, double& var) {
        if (args.find(arg) != args.end())
            var = std::stod(args.find(arg)->second);
    }

    void getArgVecInt(const std::string& argx, const std::string& argy,
        const std::string& argz, vec3<int>& var) {
        if (args.find(argx) != args.end())
            var.x = std::stoi(args.find(argx)->second);
        if (args.find(argy) != args.end())
            var.y = std::stoi(args.find(argy)->second);
        if (args.find(argz) != args.end())
            var.z = std::stoi(args.find(argz)->second);
    }

    void getArgVecDouble(const std::string& argx, const std::string& argy,
        const std::string& argz, vec3<double>& var) {
        if (args.find(argx) != args.end())
            var.x = std::stod(args.find(argx)->second);
        if (args.find(argy) != args.end())
            var.y = std::stod(args.find(argy)->second);
        if (args.find(argz) != args.end())
            var.z = std::stod(args.find(argz)->second);
    }

    void getArgFieldSolver(const std::string& arg,
        std::unique_ptr<FieldSolver>& solver) {
        if (args.find(arg) != args.end()) {
            std::string solverStr = args.find(arg)->second;
            if (solverStr == "FDTD")
                solver.reset(new FieldSolverFDTD());
            else if (solverStr == "PSTD")
                solver.reset(new FieldSolverPSTD());
            else if (solverStr == "PSATD")
                solver.reset(new FieldSolverPSATD());
        }
    }

    void getArgParallelScheme(const std::string& arg,
        std::unique_ptr<ParallelScheme>& scheme) {
        if (args.find(arg) != args.end()) {
            std::string schemeStr = args.find(arg)->second;
            if (schemeStr == "sum")
                scheme.reset(new ParallelSchemeSum());
            else if (schemeStr == "copy")
                scheme.reset(new ParallelSchemeCopy());
        }
    }

    void getArgMask(const std::string& arg,
        std::unique_ptr<Mask>& mask) {
        if (args.find(arg) != args.end()) {
            std::string maskStr = args.find(arg)->second;
            if (maskStr == "simple")
                mask.reset(new SimpleMask());
            else if (maskStr == "smooth")
                mask.reset(new SmoothMask());
        }
    }

    void getArgStr(const std::string& arg, std::string& var) {
        if (args.find(arg) != args.end())
            var = args.find(arg)->second;
    }

    void getArgState(const std::string& arg, State& state) {
        if (args.find(arg) != args.end()) {
            std::string stateStr = args.find(arg)->second;
            if (stateStr == "on")
                state = State::on;
            else if (stateStr == "off")
                state = State::off;
        }
    }
};