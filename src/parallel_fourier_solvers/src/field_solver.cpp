#include "field_solver.h"

static int mod(int a, int b) {
    return (a + b) % b;
}

void FieldSolverPSATD::operator()(double dt)
{
#pragma omp parallel for
    for (int i = 0; i < grid->sizeComplex().x; i++)
        for (int j = 0; j < grid->sizeComplex().y; j++)
            for (int k = 0; k < grid->sizeComplex().z; k++) {

                vec3<MyComplex> K = getFreqVector(vec3<int>(i, j, k), grid->getStep());
                double normK = K.getNorm();
                K = K.normilize();

                double C = cos(normK*constants::c*dt);
                double S = sin(normK*constants::c*dt);
                vec3<MyComplex> E = grid->EF(i, j, k), B = grid->BF(i, j, k),
                    J = 4 * constants::pi * grid->JF(i, j, k);
                vec3<MyComplex> Jl = K * vec3<MyComplex>::dot(K, J);
                vec3<MyComplex> El = K * vec3<MyComplex>::dot(K, E);

                if (normK == 0) {
                    grid->EF.write(i, j, k, E - J);
                    continue;
                }

                grid->EF.write(i, j, k, C*E + MyComplex::i() * S*vec3<MyComplex>::cross(K, B) -
                    S / (normK*constants::c)*J + (1 - C)*El + Jl * (S / (normK*constants::c) - dt));
                grid->BF.write(i, j, k, C*B - MyComplex::i() * S*vec3<MyComplex>::cross(K, E) +
                    MyComplex::i() * ((1 - C) / (normK*constants::c))*vec3<MyComplex>::cross(K, J));
            }
}


void FieldSolverPSTD::refreshE(double dt) {
#pragma omp parallel for
    for (int i = 0; i < grid->sizeComplex().x; i++)
        for (int j = 0; j < grid->sizeComplex().y; j++)
            for (int k = 0; k < grid->sizeComplex().z; k++) {

                vec3<MyComplex> K = getFreqVector(vec3<int>(i, j, k), grid->getStep());

                grid->EF.write(i, j, k, grid->EF(i, j, k) + MyComplex::i() * constants::c*dt*
                    vec3<MyComplex>::cross(K, grid->BF(i, j, k)) - 4 * constants::pi * grid->JF(i, j, k) * dt);
            }
}

void FieldSolverPSTD::refreshB(double dt) {
#pragma omp parallel for
    for (int i = 0; i < grid->sizeComplex().x; i++)
        for (int j = 0; j < grid->sizeComplex().y; j++)
            for (int k = 0; k < grid->sizeComplex().z; k++) {

                vec3<MyComplex> K = getFreqVector(vec3<int>(i, j, k), grid->getStep());

                grid->BF.write(i, j, k, grid->BF(i, j, k) - MyComplex::i() * constants::c*dt*
                    vec3<MyComplex>::cross(K, grid->EF(i, j, k)));
            }
}

void FieldSolverPSTD::operator()(double dt)
{
    refreshE(dt);
    refreshB(dt);
}


void FieldSolverFDTD::refreshE(double dt) {
    double _x, _y, _z;
#pragma omp parallel for
    for (int i = 0; i < grid->sizeReal().x; i++)
        for (int j = 0; j < grid->sizeReal().y; j++)
            for (int k = 0; k < grid->sizeReal().z; k++) {

                int prevI = mod(i - 1, grid->sizeReal().x);
                int prevJ = mod(j - 1, grid->sizeReal().y);
                int prevK = mod(k - 1, grid->sizeReal().z);

                _x = grid->E(i, j, k).x + constants::c*dt*
                    ((grid->B(i, j, k).z - grid->B(i, prevJ, k).z) / (grid->getStep().y) -
                    (grid->B(i, j, k).y - grid->B(i, j, prevK).y) / (grid->getStep().z));
                _y = grid->E(i, j, k).y - constants::c*dt*
                    ((grid->B(i, j, k).z - grid->B(prevI, j, k).z) / (grid->getStep().x) -
                    (grid->B(i, j, k).x - grid->B(i, j, prevK).x) / (grid->getStep().z));
                _z = grid->E(i, j, k).z + constants::c*dt*
                    ((grid->B(i, j, k).y - grid->B(prevI, j, k).y) / (grid->getStep().x) -
                    (grid->B(i, j, k).x - grid->B(i, prevJ, k).x) / (grid->getStep().y));

                vec3<double> res(_x, _y, _z);
                grid->E.write(i, j, k, res - 4 * constants::pi * grid->J(i, j, k) * dt);
            }
}

void FieldSolverFDTD::refreshB(double dt) {
    double _x, _y, _z;
#pragma omp parallel for
    for (int i = 0; i < grid->sizeReal().x; i++)
        for (int j = 0; j < grid->sizeReal().y; j++)
            for (int k = 0; k < grid->sizeReal().z; k++) {

                int nextI = mod(i + 1, grid->sizeReal().x);
                int nextJ = mod(j + 1, grid->sizeReal().y);
                int nextK = mod(k + 1, grid->sizeReal().z);

                _x = grid->B(i, j, k).x - constants::c*dt*
                    ((grid->E(i, nextJ, k).z - grid->E(i, j, k).z) / (grid->getStep().y) -
                    (grid->E(i, j, nextK).y - grid->E(i, j, k).y) / (grid->getStep().z));
                _y = grid->B(i, j, k).y + constants::c*dt*
                    ((grid->E(nextI, j, k).z - grid->E(i, j, k).z) / (grid->getStep().x) -
                    (grid->E(i, j, nextK).x - grid->E(i, j, k).x) / (grid->getStep().z));
                _z = grid->B(i, j, k).z - constants::c*dt*
                    ((grid->E(nextI, j, k).y - grid->E(i, j, k).y) / (grid->getStep().x) -
                    (grid->E(i, nextJ, k).x - grid->E(i, j, k).x) / (grid->getStep().y));

                vec3<double> res(_x, _y, _z);
                grid->B.write(i, j, k, res);
            }
}

void FieldSolverFDTD::operator()(double dt)
{
    refreshE(dt);
    refreshB(dt);
}
