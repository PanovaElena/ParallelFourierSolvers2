#pragma once
#include <iostream>
#include <omp.h>
#include <iostream>
#include "command_line_parser.h"
#include "task_parameters.h"
#include "vector3d.h"

class TaskParser: public CommandLineParser {
    vec3<int> getVecUseNumOfProcesses(vec3<int> value, vec3<int> numOfPr) {
        return vec3<int>(numOfPr.x > 1 ? value.x : 0,
            numOfPr.y > 1 ? value.y : 0,
            numOfPr.z > 1 ? value.z : 0);
    }
public:
    void help(TaskParameters& p1, Task task) override {
        ParallelTaskParameters& p = static_cast<ParallelTaskParameters&>(p1);

        std::cout <<
            "-help                       get help\n" <<
            "-dt                         set time step, default value is " << p.dt << "\n" <<
            "-ax, -ay, -az               set start of area (end will be set automatically), default value is " << p.gridParams.a << "\n" <<
            "-nx, -ny, -nz               set size of grid, default value is "<< p.gridParams.n << "\n" <<
            "-dx, -dy, -dz               set grid spacing, default value is " << p.gridParams.d << "\n" <<
            "-solver                     set solver (\"PSTD\", \"PSATD\", \"FDTD\"), default value is \"PSATD\"\n" <<
            "-nseqi                      set number of sequential iterations, default value is " << p.nSeqSteps << "\n" <<
			"-dump                       set value whether dump (\"on\" or \"off\"), default value is \"on\"\n" <<
			"-nthreads                   set number of threads\"on\"\n" <<
            "-dir                        set output directory" << "\n";
        if (task == Task::parallel) {
            std::cout <<
                "-npari                      set number of parallel iterations, default value is " << p.nParSteps << "\n" <<
                "-ndomi                      set number of iterations before every exchange, default is " << p.nDomainSteps << "\n" <<
                "-gx, -gy, -gz               set width of guard, default value is " << p.guard << "\n" <<
                "-mask                       set mask (\"simple\" or \"smooth\"), default value is "<< p.mask->to_string() <<"\n" <<
                "-mwx, -mwy, -mwz            set width of mask (for \"smooth\"), default value is " << p.mask->getMaskWidth() << "\n" <<
                "-filter                     set on or off for low-frequency filter (\"on\" or \"off\"), default value is " << p.filter->to_string() << "\n" <<
                "-fwx, -fwy, -fwz            set width of low-frequency filter, default value is " << p.filter->getWidth() << "\n" <<
                "-fnzx, -fnzy, -fnzz         set num of frequences which should be set to zeros for low-frequency filter, default value is " << p.filter->getNumZeroFreq() << "\n" <<
                "-npx, -npy, -npz            set num of domains for every dimension, default value is (MPISize,1,1)\n" << 
                "-scheme                     set parallel scheme (\"copy\" or \"sum\"), default value is \"sum\"" << "\n";
        }
    }

    Stat saveArgs(TaskParameters& p, Task task) override {

        ParallelTaskParameters& params = static_cast<ParallelTaskParameters&>(p);

        if (m.find("-ax") != m.end()) params.gridParams.a.x = std::stod(m.find("-ax")->second);
        if (m.find("-ay") != m.end()) params.gridParams.a.y = std::stod(m.find("-ay")->second);
        if (m.find("-az") != m.end()) params.gridParams.a.z = std::stod(m.find("-az")->second);

        if (m.find("-dx") != m.end()) params.gridParams.d.x = std::stod(m.find("-dx")->second);
        if (m.find("-dy") != m.end()) params.gridParams.d.y = std::stod(m.find("-dy")->second);
        if (m.find("-dz") != m.end()) params.gridParams.d.z = std::stod(m.find("-dz")->second);

        if (m.find("-nx") != m.end()) params.gridParams.n.x = std::stoi(m.find("-nx")->second);
        if (m.find("-ny") != m.end()) params.gridParams.n.y = std::stoi(m.find("-ny")->second);
        if (m.find("-nz") != m.end()) params.gridParams.n.z = std::stoi(m.find("-nz")->second);

        if (m.find("-dt") != m.end()) params.dt = std::stod(m.find("-dt")->second);

        if (m.find("-solver") != m.end()) {
            std::string solver = m.find("-solver")->second;
            if (solver == "FDTD")
                params.fieldSolver.reset(new FieldSolverFDTD());
            if (solver == "PSTD")
                params.fieldSolver.reset(new FieldSolverPSTD());
            if (solver == "PSATD")
                params.fieldSolver.reset(new FieldSolverPSATD());
        }

        if (m.find("-nseqi") != m.end()) params.nSeqSteps = std::stoi(m.find("-nseqi")->second);
		if (m.find("-dump") != m.end() && m.find("-dump")->second=="off")
			params.fileWriter.turnOff();
        if (m.find("-dir") != m.end()) params.fileWriter.setDirectory(m.find("-dir")->second);
		if (m.find("-nthreads") != m.end()) {
			omp_set_num_threads(std::stoi(m.find("-nthreads")->second));
			std::cout << "number of threads = " << m.find("-nthreads")->second << std::endl;
		}

        if (task == Task::parallel) {

            if (m.find("-scheme") != m.end()) {
                std::string scheme = m.find("-scheme")->second;
                if(scheme == "sum")
                    params.scheme.reset(new ParallelSchemeSum());
                if (scheme == "copy")
                    params.scheme.reset(new ParallelSchemeCopy());
            }

            if (m.find("-gx") != m.end()) params.guard.x = std::stoi(m.find("-gx")->second);
            if (m.find("-gy") != m.end()) params.guard.y = std::stoi(m.find("-gy")->second);
            if (m.find("-gz") != m.end()) params.guard.z = std::stoi(m.find("-gz")->second);

            if (m.find("-npx") != m.end()) params.numOfProcesses.x = std::stoi(m.find("-npx")->second);
            if (m.find("-npy") != m.end()) params.numOfProcesses.y = std::stoi(m.find("-npy")->second);
            if (m.find("-npz") != m.end()) params.numOfProcesses.z = std::stoi(m.find("-npz")->second);

            if (m.find("-npari") != m.end()) params.nParSteps = std::stoi(m.find("-npari")->second);
            if (m.find("-ndomi") != m.end()) params.nDomainSteps = std::stoi(m.find("-ndomi")->second);

            if (m.find("-mask") != m.end()) {
                std::string mask = m.find("-mask")->second;
                if (mask == "simple")
                    params.mask.reset(new SimpleMask());
                if (mask == "smooth")
                    params.mask.reset(new SmoothMask());
            }

            vec3<int> mw(0);
            if (m.find("-mwx") != m.end()) mw.x = std::stoi(m.find("-mwx")->second);
            if (m.find("-mwy") != m.end()) mw.y = std::stoi(m.find("-mwy")->second);
            if (m.find("-mwz") != m.end()) mw.z = std::stoi(m.find("-mwz")->second);
            params.mask->setMaskWidth(mw);

            if (m.find("-filter") != m.end()) {
                if (m.find("-filter")->second == "on")
                    params.filter->turnOn();
                else if (m.find("-filter")->second == "off")
                    params.filter->turnOff();
            }

            vec3<int> fw(0), fnz(0);
            if (m.find("-fwx") != m.end()) fw.x = std::stoi(m.find("-fwx")->second);
            if (m.find("-fwy") != m.end()) fw.y = std::stoi(m.find("-fwy")->second);
            if (m.find("-fwz") != m.end()) fw.z = std::stoi(m.find("-fwz")->second);
            if (m.find("-fnzx") != m.end()) fnz.x = std::stoi(m.find("-fnzx")->second);
            if (m.find("-fnzy") != m.end()) fnz.y = std::stoi(m.find("-fnzy")->second);
            if (m.find("-fnzz") != m.end()) fnz.z = std::stoi(m.find("-fnzz")->second);
            params.filter->setParams(fw, fnz);

        }
        return Stat::OK;
    }
};
