#include "gtest.h"
#include "grid3d.h"

class TestGrid : public testing::Test {
public:
    vec3<int> n;
    vec3<> a, b, d;

    TestGrid() {
        n = { 10, 20, 30 };
        a = { 1, 2, 3 };
        b = { 11, 22, 33 };
        d = (b - a) / (vec3<>)n;
    }
};

TEST_F(TestGrid, created_correctly) {
    ASSERT_NO_THROW(Grid3d());
}

TEST_F(TestGrid, created_correctly_with_parameters) {
    ASSERT_NO_THROW(Grid3d(GridParams(a, d, n)););
}

TEST_F(TestGrid, copy_constructor) {
    Grid3d grid(GridParams(a, d, n));
    Grid3d grid2 = grid;
    ASSERT_TRUE(grid2 == grid);
}

TEST_F(TestGrid, copy_operator) {
    Grid3d grid(GridParams(a, d, n));
    Grid3d grid3(GridParams(a, d, n));
    grid3 = grid;
    ASSERT_TRUE(grid3 == grid);
}

TEST_F(TestGrid, operator_brackets_1) {
    Grid3d grid(GridParams(a, d, n));
    grid.E.x(1, 2, 3) = 25;
    ASSERT_EQ(grid.E.x(1, 2, 3), 25);
    ASSERT_EQ(grid.E(1, 2, 3).x, 25);
    ASSERT_EQ(grid.E(1, 2, 3).x, grid.E.x(1, 2, 3));
}

TEST_F(TestGrid, operator_brackets_2) {
    Grid3d grid(GridParams(a, d, n));
    grid.E(1, 2, 3) = vec3<double>(25, 25, 25);
    ASSERT_EQ(grid.E.x(1, 2, 3), 0);
    ASSERT_EQ(grid.E(1, 2, 3).x, 0);
    ASSERT_EQ(grid.E(1, 2, 3).x, grid.E.x(1, 2, 3));
}

TEST_F(TestGrid, operator_brackets_3) {
    Grid3d grid(GridParams(a, d, n));
    grid.E[0](1, 2, 3) = 25;
    ASSERT_EQ(grid.E[0](1, 2, 3), 25);
    ASSERT_EQ(grid.E(1, 2, 3)[0], 25);
    ASSERT_EQ(grid.E(1, 2, 3)[0], grid.E[0](1, 2, 3));
}