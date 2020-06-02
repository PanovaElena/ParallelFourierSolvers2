#include "field_solver.h"

static int mod(int a, int b) {
    return (a + b) % b;
}

void FieldSolverPSATD::operator()(const double dt)
{
    const MyComplex complex_i = MyComplex::i();
    const int nx = grid->sizeComplex().x, ny = grid->sizeComplex().y, nz = grid->sizeComplex().z;
    const vec3<double> d = grid->getStep();
    const vec3<int> nReal = grid->sizeReal();
    const int chunkSize = 32;
    const int nChunks = nz / chunkSize;
    const int chunkRem = nz % chunkSize;

#pragma omp parallel for collapse(2)
    for (int i = 0; i < nx; i++)
        for (int j = 0; j < ny; j++)
            for (int chunk = 0; chunk < nChunks + 1; chunk++) {
                int kLast = chunk == nChunks ? chunkRem : chunkSize;

                vec3<> K[chunkSize];
                double normK[chunkSize];
#pragma ivdep
                for (int k = 0; k < kLast; k++) {
                    K[k] = getFreqVector(vec3<int>(i, j, k + chunk * chunkSize), nReal, d);
                    normK[k] = K[k].getNorm();
                    if (normK[k] != 0.0)
                        K[k] = K[k] / normK[k];
                }

                double C[chunkSize], S[chunkSize];
#pragma ivdep
                for (int k = 0; k < kLast; k++) {
                    C[k] = cos(normK[k] * constants::c*dt);
                    S[k] = sin(normK[k] * constants::c*dt);
                }

#pragma ivdep
                for (int k = 0; k < kLast; k++) {
                    int zIndex = k + chunk * chunkSize;
                    vec3<MyComplex> E = grid->EF(i, j, zIndex), B = grid->BF(i, j, zIndex),
                        J = 4 * constants::pi * grid->JF(i, j, zIndex);
                    vec3<MyComplex> Jl = K[k] * vec3<MyComplex>::dot(K[k], J);
                    vec3<MyComplex> El = K[k] * vec3<MyComplex>::dot(K[k], E);

                    if (normK == 0) {
                        grid->EF.write(i, j, zIndex, E - J);
                    }
                    else {
                        grid->EF.write(i, j, zIndex, C[k] * E + complex_i * S[k] * vec3<MyComplex>::cross(K[k], B) -
                            S[k] / (normK[k] * constants::c)*J +
                            (1 - C[k])*El + Jl * (S[k] / (normK[k] * constants::c) - dt));
                        grid->BF.write(i, j, zIndex, C[k] * B - complex_i * S[k] * vec3<MyComplex>::cross(K[k], E) +
                            complex_i * ((1 - C[k]) / (normK[k] * constants::c))*vec3<MyComplex>::cross(K[k], J));
                    }
                }
            }
}


void FieldSolverPSTD::refreshE(const double dt) {
    const MyComplex complex_i = MyComplex::i();
    const int nx = grid->sizeComplex().x, ny = grid->sizeComplex().y, nz = grid->sizeComplex().z;
#pragma omp parallel for collapse(2)
    for (int i = 0; i < nx; i++)
        for (int j = 0; j < ny; j++)
            for (int k = 0; k < nz; k++) {

                vec3<MyComplex> K = getFreqVector(vec3<int>(i, j, k), grid->sizeReal(), grid->getStep());

                grid->EF.write(i, j, k, grid->EF(i, j, k) + complex_i * constants::c*dt*
                    vec3<MyComplex>::cross(K, grid->BF(i, j, k)) - 4 * constants::pi * grid->JF(i, j, k) * dt);
            }
}

void FieldSolverPSTD::refreshB(const double dt) {
    const MyComplex complex_i = MyComplex::i();
    const int nx = grid->sizeComplex().x, ny = grid->sizeComplex().y, nz = grid->sizeComplex().z;
#pragma omp parallel for collapse(2)
    for (int i = 0; i < nx; i++)
        for (int j = 0; j < ny; j++)
            for (int k = 0; k < nz; k++) {

                vec3<MyComplex> K = getFreqVector(vec3<int>(i, j, k), grid->sizeReal(), grid->getStep());

                grid->BF.write(i, j, k, grid->BF(i, j, k) - complex_i * constants::c*dt*
                    vec3<MyComplex>::cross(K, grid->EF(i, j, k)));
            }
}

void FieldSolverPSTD::operator()(const double dt)
{
    refreshE(dt);
    refreshB(dt);
}


void FieldSolverFDTD::refreshE(const double dt) {
    double _x, _y, _z;
    const int nx = grid->sizeReal().x, ny = grid->sizeReal().y, nz = grid->sizeReal().z;
#pragma omp parallel for collapse(2)
    for (int i = 0; i < nx; i++)
        for (int j = 0; j < ny; j++)
            for (int k = 0; k < nz; k++) {

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

void FieldSolverFDTD::refreshB(const double dt) {
    double _x, _y, _z;
    const int nx = grid->sizeReal().x, ny = grid->sizeReal().y, nz = grid->sizeReal().z;
#pragma omp parallel for collapse(2)
    for (int i = 0; i < nx; i++)
        for (int j = 0; j < ny; j++)
            for (int k = 0; k < nz; k++) {

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

void FieldSolverFDTD::operator()(const double dt)
{
    refreshE(dt);
    refreshB(dt);
}
