#include "gtest.h"
#include "fourier_transform.h"
#include "grid3d.h"
#include "my_complex.h"
#include "physical_constants.h"

class test_lag : public testing::Test {
public:

    int n = 6;
    double a = 0, b = n;
    Grid3d gr;
    double A = 1000;
    double d = (b - a) / n;
    int step = 1;
    double c = step * d;

    test_lag() : gr({ n, n, n }, { a, a, a }, { b, b, b }) {
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                for (int k = 0; k < n; k++)
                    gr.E.x(i, j, k) = f(i, j, k);
    }

    double f(int i, int j, int k) {
        return A * sin(2 * constants::pi*(i*gr.getStep().x + j * gr.getStep().y + k * gr.getStep().z) / (b - a));
    }

    void MyTestBodyLag() {
        fourierTransform(gr, RtoC);
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                for (int k = 0; k < n / 2 + 1; k++) {
                    vec3<MyComplex> K = getFreqVector(vec3<int>(i, j, k), gr);
                    gr.EF.x(i, j, k) *= MyComplex::getTrig(1, -c * (K.x + K.y + K.z));
                }
        fourierTransform(gr, CtoR);
    }

};

TEST_F(test_lag, lag_is_correct) {
    MyTestBodyLag();

    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            for (int k = 0; k < n; k++) {
                ASSERT_NEAR(f(i - step, j - step, k - step), gr.E.x(i, j, k), 1E-7);
            }
}