#include "field_solver.h"

static int mod(int a, int b) {
    return (a + b) % b;
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