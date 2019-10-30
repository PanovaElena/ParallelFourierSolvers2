#include "field_solver.h"      


void FieldSolverPSTD::refreshE(double dt) {
    int nx = grid->sizeComplex().x, ny = grid->sizeComplex().y, nz = grid->sizeComplex().z;
#pragma omp parallel for
    for (int i = 0; i < nx; i++)
        for (int j = 0; j < ny; j++)
#pragma ivdep
            for (int k = 0; k < nz; k++) {

                vec3<MyComplex> K = getFreqVector(vec3<int>(i, j, k), *grid);

                grid->EF.write(i, j, k, grid->EF(i, j, k) + constants::c * dt * MyComplex::i() *
                    vec3<MyComplex>::cross(K, grid->BF(i, j, k)) - 4 * constants::pi * grid->JF(i, j, k) * dt);
            }
}

void FieldSolverPSTD::refreshB(double dt) {
    int nx = grid->sizeComplex().x, ny = grid->sizeComplex().y, nz = grid->sizeComplex().z;
#pragma omp parallel for
    for (int i = 0; i < nx; i++)
        for (int j = 0; j < ny; j++)
#pragma ivdep
            for (int k = 0; k < nz; k++) {

                vec3<MyComplex> K = getFreqVector(vec3<int>(i, j, k), *grid);

                grid->BF.write(i, j, k, grid->BF(i, j, k) - constants::c * dt * MyComplex::i() *
                    vec3<MyComplex>::cross(K, grid->EF(i, j, k)));
            }
}

void FieldSolverPSTD::operator()(double dt)
{
    refreshE(dt);
    refreshB(dt);
}
