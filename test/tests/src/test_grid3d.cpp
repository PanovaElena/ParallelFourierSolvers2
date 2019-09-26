#include "gtest.h"
#include "grid3d.h"

TEST(TestGrid, created_correctly) {
    ASSERT_NO_THROW(Grid3d());
}

TEST(TestGrid, created_correctly_with_parameters) {
    ASSERT_NO_THROW(Grid3d({ 10, 20, 30 }, { 1, 2, 3 }, { 11, 22, 33 }););
}

TEST(TestGrid, copy_constructor) {
    Grid3d grid({ 10, 20, 30 }, { 1, 2, 3 }, { 11, 22, 33 });
    Grid3d grid2 = grid;
    ASSERT_TRUE(grid2 == grid);
}

TEST(TestGrid, copy_operator) {
    Grid3d grid({ 10, 20, 30 }, { 1, 2, 3 }, { 11, 22, 33 });
    Grid3d grid3({ 10, 20, 30 }, { 1, 2, 3 }, { 11, 22, 33 });
    grid3 = grid;
    ASSERT_TRUE(grid3 == grid);
}

TEST(TestGrid, operator_brackets_1) {
    Grid3d grid({ 10, 20, 30 }, { 1, 2, 3 }, { 11, 22, 33 });
    grid.E.x(1, 2, 3) = 25;
    ASSERT_EQ(grid.E.x(1, 2, 3), 25);
    ASSERT_EQ(grid.E(1, 2, 3).x, 25);
    ASSERT_EQ(grid.E(1, 2, 3).x, grid.E.x(1, 2, 3));
}

TEST(TestGrid, operator_brackets_2) {
    Grid3d grid({ 10, 20, 30 }, { 1, 2, 3 }, { 11, 22, 33 });
    grid.E(1, 2, 3) = vec3<double>(25, 25, 25);
    ASSERT_EQ(grid.E.x(1, 2, 3), 0);
    ASSERT_EQ(grid.E(1, 2, 3).x, 0);
    ASSERT_EQ(grid.E(1, 2, 3).x, grid.E.x(1, 2, 3));
}

TEST(TestGrid, operator_brackets_3) {
    Grid3d grid({ 10, 20, 30 }, { 1, 2, 3 }, { 11, 22, 33 });
    grid.E[0](1, 2, 3) = 25;
    ASSERT_EQ(grid.E[0](1, 2, 3), 25);
    ASSERT_EQ(grid.E(1, 2, 3)[0], 25);
    ASSERT_EQ(grid.E(1, 2, 3)[0], grid.E[0](1, 2, 3));
}