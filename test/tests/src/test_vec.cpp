#include "gtest.h"
#include "vector3d.h"
#include "my_complex.h"

TEST(TestVector, vector_length1) {
    vec3<double> a = vec3<double>(-4, 0, 3);
    EXPECT_EQ(a.getNorm(), 5);
}

TEST(TestVector, vector_length2) {
    vec3<double> a = vec3<double>(0, 0, 1);
    EXPECT_EQ(a.getNorm(), 1);
}

TEST(TestVector, vector_length3) {
    vec3<double> a = vec3<double>(2, 2, 2);
    EXPECT_EQ(a.getNorm(), sqrt(12));
}

TEST(TestVector, scalar_product1) {
    vec3<double> a = vec3<double>(1, 2, 3);
    a = a * 3;
    vec3<double> b = vec3<double>(3, 6, 9);
    EXPECT_TRUE(a.get_x() == b.get_x() && a.get_y() == b.get_y() && a.get_z() == b.get_z());
}

TEST(TestVector, scalar_product2) {
    vec3<double> a = vec3<double>(0, 0, 0);
    a = a * 3;
    vec3<double> b = vec3<double>(0, 0, 0);
    EXPECT_TRUE(a.get_x() == b.get_x() && a.get_y() == b.get_y() && a.get_z() == b.get_z());
}

TEST(TestVector, scalar_product3) {
    vec3<double> a = vec3<double>(1, 1, 1);
    a = a * 0;
    vec3<double> b = vec3<double>(0, 0, 0);
    EXPECT_TRUE(a.get_x() == b.get_x() && a.get_y() == b.get_y() && a.get_z() == b.get_z());
}

TEST(TestVector, scalar_product4) {
    vec3<double> a = vec3<double>(1, 1, 1);
    a = a + a;
    vec3<double> b = vec3<double>(2, 2, 2);
    EXPECT_TRUE(a.get_x() == b.get_x() && a.get_y() == b.get_y() && a.get_z() == b.get_z());
}

TEST(TestVector, scalar_product_double) {
    vec3<double> a(1, 2, 3), b(3, 2, 1);
    ASSERT_EQ(vec3<double>::dot(a, b), 10);
}

TEST(TestVector, scalar_product_complex) {
    vec3<MyComplex> a(complex_i, complex_i, complex_i);
    vec3<MyComplex> b(complex_i, complex_i, complex_i);
    ASSERT_EQ(vec3<MyComplex>::dot(a, b), MyComplex(-3, 0));
}

TEST(TestVector, vector_product_double) {
    vec3<double> a(1, 0, 0), b(0, 1, 0);
    ASSERT_EQ(vec3<double>::cross(a, b), vec3<double>(0, 0, 1));
}

TEST(TestVector, vector_product_complex) {
    vec3<MyComplex> a(complex_i, complex_0, complex_0);
    vec3<MyComplex> b(complex_0, complex_i, complex_0);
    ASSERT_EQ(vec3<MyComplex>::cross(a, b), vec3<MyComplex>(complex_0, complex_0, MyComplex(-1, 0)));
}

TEST(TestVector, vector_product_complex_2) {
    vec3<MyComplex> a(complex_i, complex_i*(-1), complex_i);
    vec3<MyComplex> b(complex_i*(-1), complex_i, complex_i*(-1));
    ASSERT_EQ(vec3<MyComplex>::cross(a, b), vec3<MyComplex>(complex_0, complex_0, complex_0));
}

TEST(TestVector, vector_product_complex_vectors_are_orthogonal) {
    vec3<MyComplex> a(MyComplex(1, 2), MyComplex(4, 7), MyComplex(1, 3));
    vec3<MyComplex> b(MyComplex(3, 9), MyComplex(2, 2), MyComplex(6, 7));
    vec3<MyComplex> c = vec3<MyComplex>::cross(a, b);
    ASSERT_EQ(complex_0, vec3<MyComplex>::dot(a, c));
    ASSERT_EQ(complex_0, vec3<MyComplex>::dot(b, c));
}

TEST(TestVector, decomposition_of_vector_into_components) {
    vec3<MyComplex> E(MyComplex(1, 2), MyComplex(4, 7), MyComplex(1, 3));
    vec3<MyComplex> K(MyComplex(1, 0), MyComplex(1, 0), MyComplex(1, 0));
    vec3<MyComplex> k = K.normilize();
    ASSERT_EQ(k.getNorm(), 1);

    vec3<MyComplex> El = k * vec3<MyComplex>::dot(k, E);
    vec3<MyComplex> kE = vec3<MyComplex>::cross(k, E);
    vec3<MyComplex> Et = (vec3<MyComplex>::cross(k, kE))*(-1);

    ASSERT_DOUBLE_EQ(E[0].getReal(), (El + Et)[0].getReal());
    ASSERT_DOUBLE_EQ(E[0].getImag(), (El + Et)[0].getImag());
    ASSERT_DOUBLE_EQ(E[1].getReal(), (El + Et)[1].getReal());
    ASSERT_DOUBLE_EQ(E[1].getImag(), (El + Et)[1].getImag());
    ASSERT_DOUBLE_EQ(E[2].getReal(), (El + Et)[2].getReal());
    ASSERT_DOUBLE_EQ(E[2].getImag(), (El + Et)[2].getImag());
}

TEST(TestVector, normalize_double) {
    vec3<double> a(1, 1, 1);
    ASSERT_EQ(a.normilize(), vec3<double>(1 / sqrt(3), 1 / sqrt(3), 1 / sqrt(3)));
}

TEST(TestVector, normalize_complex) {
    vec3<MyComplex> a(MyComplex(1, 0), MyComplex(1, 1), MyComplex(0, 1));
    vec3<MyComplex> res(MyComplex(0.5, 0), MyComplex(0.5, 0.5), MyComplex(0, 0.5));
    ASSERT_EQ(res, a.normilize());
}

TEST(TestVector, normalize_complex_2) {
    vec3<MyComplex> a(MyComplex(5, 6), MyComplex(-3, 4), MyComplex(4, 8));
    vec3<MyComplex> b = a;
    a.normilize();

    ASSERT_DOUBLE_EQ(b.getNorm(), sqrt(166));

    ASSERT_DOUBLE_EQ(a.get_x().getReal(), b.get_x().getReal()*(1.0 / sqrt(166)));
    ASSERT_DOUBLE_EQ(a.get_x().getImag(), b.get_x().getImag()*(1.0 / sqrt(166)));
    ASSERT_DOUBLE_EQ(a.get_y().getReal(), b.get_y().getReal()*(1.0 / sqrt(166)));
    ASSERT_DOUBLE_EQ(a.get_y().getImag(), b.get_y().getImag()*(1.0 / sqrt(166)));
    ASSERT_DOUBLE_EQ(a.get_z().getReal(), b.get_z().getReal()*(1.0 / sqrt(166)));
    ASSERT_DOUBLE_EQ(a.get_z().getImag(), b.get_z().getImag()*(1.0 / sqrt(166)));
}


