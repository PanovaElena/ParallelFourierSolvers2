#include "gtest.h"
#include "my_complex.h"

TEST(TestMyComplex, can_add) {
    MyComplex z1(1, 2), z2(3, 4);
    EXPECT_EQ(MyComplex(4, 6), z1 + z2);
}

TEST(TestMyComplex, can_multiply) {
    MyComplex z1(1, 2), z2(3, 4);
    EXPECT_EQ(MyComplex(-5, 10), z1 * z2);
}

TEST(TestMyComplex, can_multiply_i) {
    MyComplex z1(1, 2);
    EXPECT_EQ(MyComplex(-2, 1), complex_i*z1);
    EXPECT_EQ(MyComplex(-2, 1), z1*complex_i);
}

TEST(TestMyComplex, can_multiply_i_double) {
    EXPECT_EQ(MyComplex(0, 5), complex_i * 5);
    EXPECT_EQ(MyComplex(0, 5), 5 * complex_i);
}