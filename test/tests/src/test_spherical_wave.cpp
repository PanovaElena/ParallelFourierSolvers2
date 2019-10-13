#include "gtest.h"
#include <cmath>
#include <string>
#include "physical_constants.h"
#include "grid3d.h"
#include "fourier_transform.h"
#include "class_member_ptr.h"
#include "spherical_wave.h"
#include "file_writer.h"
#include "field_solver.h"

class TestSphericalWave :public testing::Test, public SphericalWave {
public:

    std::string seqDir = std::string(ROOT_DIR) + "/scripts/spherical_wave/test/";
    std::string dirE = "E/";
    std::string dirB = "B/";
    std::string dirJ = "J/";

    FileWriter fileWriterE;
    FileWriter fileWriterB;
    FileWriter fileWriterJ;

    int nIterBetweenDumps = params.getNSteps() / 10;

    TestSphericalWave() :
        fileWriterE(seqDir + dirE, E, z, params.fileWriter.getSection()),
        fileWriterB(seqDir + dirB, B, z, params.fileWriter.getSection()),
        fileWriterJ(seqDir + dirJ, J, z, params.fileWriter.getSection())
    {
        params.dt = 0.5;
        params.nSeqSteps = 40;
        params.nParSteps = 0;
    }

    void MyTestBodyWrite(FieldSolver& fs) {
        params.setFieldSolver(fs, grid);

        write(fileWriterE, 0);
        write(fileWriterB, 0);

        for (int iter = 0; iter < params.getNSteps(); iter++) {
            if (iter%nIterBetweenDumps == 0) {
                write(fileWriterE, iter);
                write(fileWriterB, iter);
                write(fileWriterJ, iter);
            }
            grid.setJ(iter);

            params.fieldSolver->doFourierTransform(RtoC);
            params.fieldSolver->run(params.dt);
            params.fieldSolver->doFourierTransform(CtoR);

        }

        write(fileWriterE, params.getNSteps(), "last_iter.csv");
        write(fileWriterB, params.getNSteps(), "last_iter.csv");
        write(fileWriterJ, params.getNSteps(), "last_iter.csv");

        plotJ();
    }

    void write(FileWriter& fileWriter, int iter, std::string name = "") {
        if (name == "")
            name = params.fieldSolver->to_string() + "_iter_" + std::to_string(iter)
            + "_coord_" + std::to_string(fileWriter.getCoord()) + ".csv";
        fileWriter.write(grid, name);
    }

    void plotJ() {
        int iter = 2;
        grid.setJ(iter);
        write(fileWriterJ, iter);
    }

};

TEST_F(TestSphericalWave, runningWaveFDTD) {
    MyTestBodyWrite(FieldSolverFDTD());
}

TEST_F(TestSphericalWave, runningWavePSATD) {
    MyTestBodyWrite(FieldSolverPSATD());
}

TEST_F(TestSphericalWave, runningWavePSTD) {
    MyTestBodyWrite(FieldSolverPSTD());
}