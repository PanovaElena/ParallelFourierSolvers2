#include "gtest.h"
#include "array3d.h"

TEST(TestArray3d, create_array_no_throws) {
    ASSERT_NO_THROW(Array3d<int> arr;);
}

TEST(TestArray3d, create_array_correctly) {
    Array3d<int> arr({ 10, 20, 30 });
    ASSERT_NO_THROW(arr(9, 19, 29));
}

TEST(TestArray3d, operator_brackets_works_correctly) {
    Array3d<int> arr({ 10, 20, 30 });
    arr(9, 19, 29) = 123;
    ASSERT_EQ(123, arr(9, 19, 29));
}