#include "gtest.h"
#include <cmath>
#include <string>
#include "running_wave.h"
#include "field_solver.h"
#include "file_writer.h"

class TestRunningWave :public testing::Test, public RunningWave {
public:
    std::string seqDir = std::string(ROOT_DIR) + "/scripts/running_wave/test/";
    std::string dirE = "E/";
    std::string dirB = "B/";

    FileWriter fileWriterE;
    FileWriter fileWriterB;

    int nIterBetweenDumps;

    TestRunningWave() :
        fileWriterE(seqDir + dirE, E, y, params.fileWriter.getSection()),
        fileWriterB(seqDir + dirB, B, z, params.fileWriter.getSection())
    {
        params.nSeqSteps = 5;
        params.nParSteps = 0;
        nIterBetweenDumps = 1;
    }

    void write(FileWriter& fileWriter, int iter, std::string name = "") {
        if (name == "")
            name = params.fieldSolver->to_string()+"_iter_" + std::to_string(iter) + "_coord_" +
            std::to_string(fileWriter.getCoord()) + ".csv";
        fileWriter.write(grid, name);
    }

    void MyTestBodyWrite(FieldSolver& fs) {
        params.setFieldSolver(fs, grid);

        write(fileWriterE, 0);
        write(fileWriterB, 0);

        params.fieldSolver->doFourierTransform(RtoC);

        for (int j = 0; j < params.getNSteps(); j++) {
            params.fieldSolver->run(params.dt);

            if (j%nIterBetweenDumps == 0) {
                params.fieldSolver->doFourierTransform(CtoR);

                write(fileWriterE, j);
                write(fileWriterB, j);

                params.fieldSolver->doFourierTransform(RtoC);
            }
        }

        write(fileWriterE, params.getNSteps(), "last_iter.csv");
        write(fileWriterB, params.getNSteps(), "last_iter.csv");

    }
};

TEST_F(TestRunningWave, runningWaveFDTD) {
    MyTestBodyWrite(FieldSolverFDTD());
}

TEST_F(TestRunningWave, runningWavePSATD) {
    MyTestBodyWrite(FieldSolverPSATD());
}

TEST_F(TestRunningWave, runningWavePSTD) {
    MyTestBodyWrite(FieldSolverPSTD());
}
