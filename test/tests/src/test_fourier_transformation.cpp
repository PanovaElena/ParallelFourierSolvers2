#include "gtest.h"
#include "grid3d.h"
#include "fourier_transform.h"
#include "physical_constants.h"
#include "class_member_ptr.h"
#include "my_complex.h"
#include "fftw3.h"

class TestFourierTransform : public testing::Test {
public:
    int nx = 10, ny = 5, nz = 3;
    double X = 6, Y = 8, Z = 10;
    Grid3d grid;

    double f(int index, Coordinate coord) {
        const double A = 10;
        switch (coord) {
        case Coordinate::x:
            return A * sin(2 * constants::pi*index / nx);
        case Coordinate::y:
            return A * sin(2 * constants::pi*index / nz);
        case Coordinate::z:
            return A * sin(2 * constants::pi*index / ny);
        }
    }

    TestFourierTransform() : grid({ nx, ny, nz }, { 0, 0, 0 }, { X, Y, Z }) {
        for (int i = 0; i < nx; i++)
            for (int j = 0; j < ny; j++)
                for (int k = 0; k < nz; k++) {
                    double x = f(k, Coordinate::x);
                    double y = f(i, Coordinate::y);
                    double z = f(j, Coordinate::z);

                    grid.E[0](i, j, k) = grid.B[0](i, j, k) = x;
                    grid.E[1](i, j, k) = grid.B[1](i, j, k) = y;
                    grid.E[2](i, j, k) = grid.B[2](i, j, k) = z;
                }
    }

    void MyTestBody(Field field, Coordinate coord) {

        MemberOfNode p = getMemberPtrField<double>(field);
        MemberOfField m = getMemberPtrFieldCoord<double>(coord);

        Grid3d grid2 = grid;

        fourierTransform(grid, field, coord, RtoC);

        bool flag = true;
        for (int i = 0; i < grid.sizeReal().x; i++)
            for (int j = 0; j < grid.sizeReal().y; j++)
                for (int k = 0; k < grid.sizeReal().z; k++)
                    if ((grid.*p.*m)(i, j, k) != (grid2.*p.*m)(i, j, k))
                        flag = false;
        if (flag)
            std::cout << "NOT SPOIL" << std::endl;

        fourierTransform(grid, field, coord, CtoR);

        for (int i = 0; i < grid.sizeReal().x; i++)
            for (int j = 0; j < grid.sizeReal().y; j++)
                for (int k = 0; k < grid.sizeReal().z; k++)
                    ASSERT_NEAR((grid.*p.*m)(i, j, k), (grid2.*p.*m)(i, j, k), 0.1);
    }
};

TEST_F(TestFourierTransform, no_throws_RtoC) {
    ASSERT_NO_THROW(fourierTransform(grid, E, x, RtoC));
}

TEST_F(TestFourierTransform, no_throws_CtoR) {
    ASSERT_NO_THROW(fourierTransform(grid, E, x, CtoR));
}

TEST_F(TestFourierTransform, transform_correctly_Ex) {
    MyTestBody(E, x);
}

TEST_F(TestFourierTransform, transform_correctly_Ey) {
    MyTestBody(E, y);
}

TEST_F(TestFourierTransform, transform_correctly_Ez) {
    MyTestBody(E, z);
}

TEST_F(TestFourierTransform, transform_correctly_Bx) {
    MyTestBody(B, x);
}

TEST_F(TestFourierTransform, transform_correctly_By) {
    MyTestBody(B, y);
}

TEST_F(TestFourierTransform, transform_correctly_Bz) {
    MyTestBody(B, z);
}


TEST_F(TestFourierTransform, fourier_transform_writes_data_correctly_to_grid) {
    Array3d<MyComplex> arr1(grid.sizeReal()), arr2(grid.sizeReal());
    for (int i = 0; i < arr1.size().x; i++)
        for (int j = 0; j < arr1.size().y; j++)
            for (int k = 0; k < arr1.size().z; k++)
                arr1(i, j, k).setReal(grid.E.x(i, j, k));

    fftw_plan plan = fftw_plan_dft_3d(grid.sizeReal().x, grid.sizeReal().y, grid.sizeReal().z,
        (fftw_complex*)&(arr1[0]), (fftw_complex*)&(arr2[0]), FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_execute(plan);

    fourierTransform(grid, RtoC);

    for (int i = 0; i < grid.sizeComplex().x; i++)
        for (int j = 0; j < grid.sizeComplex().y; j++)
            for (int k = 0; k < grid.sizeComplex().z; k++) {
                EXPECT_NEAR(grid.EF.x(i, j, k), arr2(i, j, k), 1e-12);
            }

}