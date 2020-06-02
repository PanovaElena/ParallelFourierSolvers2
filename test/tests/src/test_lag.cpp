#include "gtest.h"
#include "fourier_transform.h"
#include "grid3d.h"
#include "my_complex.h"
#include "physical_constants.h"
#include "field_solver.h"

class testLag : public testing::Test {
public:

    int n = 6;
    double a = 0, b = n;
    Grid3d gr;
    double A = 1000;
    double d = (b - a) / n;
    int step = 1;
    double c = step * d;
    FourierTransformOfGrid ft;

    GridParams::FieldFunc fE;
    GridParams::FieldFunc fB;
    GridParams::FieldFunc fJ;

    void initializeFieldFuncs() {
        fE = [this](vec3<int> index, double t, const GridParams&) -> vec3<double> {
            double v = A * sin(2 * constants::pi*(index.x*gr.getStep().x +
                index.y * gr.getStep().y + index.z * gr.getStep().z) / (b - a));
            return vec3<>(v, 0, 0);
        };

        fB = [this](vec3<int> index, double t, const GridParams&) -> vec3<double> { return vec3<>(0); };

        fJ = [this](vec3<int> index, double t, const GridParams&) -> vec3<double> {return vec3<>(0); };
    }

    double f(vec3<int> index) {
        return A * sin(2 * constants::pi*(index.x*gr.getStep().x +
            index.y * gr.getStep().y + index.z * gr.getStep().z) / (b - a));
    }

    testLag() {
        vec3<int> n = vec3<int>(this->n);
        vec3<> a = vec3<>(this->a), b = vec3<>(this->b),
            d = (b - a) / (vec3<>)n;
        initializeFieldFuncs();
        GridParams gp(a, d, n, fE, fB, fJ);
        gr.initialize(gp);
        ft.initialize(gr, gr.sizeReal());
    }

    void MyTestBodyLag() {
        ft.fourierTransform(gr, RtoC);
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                for (int k = 0; k < n / 2 + 1; k++) {
                    vec3<> K = FourierFieldSolver::getFreqVector(vec3<int>(i, j, k), gr.sizeReal(), gr.getStep());
                    gr.EF.x(i, j, k) *= MyComplex::getTrig(1, -c * (K.x + K.y + K.z));
                }
        ft.fourierTransform(gr, CtoR);
    }

};

TEST_F(testLag, lag_is_correct) {
    MyTestBodyLag();

    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            for (int k = 0; k < n; k++) {
                ASSERT_NEAR(f({ i - step, j - step, k - step }), gr.E.x(i, j, k), 1E-7);
            }
}