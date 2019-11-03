#include "field_solver.h"      


void FieldSolverPSTD::refreshE(double dt) {
    int nx = grid->sizeComplex().x, ny = grid->sizeComplex().y, nz = grid->sizeComplex().z;

    // MyComplex *EFx = &(grid->EF.x(0, 0, 0)), *EFy = &(grid->EF.y(0, 0, 0)), *EFz = &(grid->EF.z(0, 0, 0));
    // MyComplex *BFx = &(grid->EF.x(0, 0, 0)), *BFy = &(grid->BF.y(0, 0, 0)), *BFz = &(grid->BF.z(0, 0, 0));
    // MyComplex *JFx = &(grid->EF.x(0, 0, 0)), *JFy = &(grid->JF.y(0, 0, 0)), *JFz = &(grid->JF.z(0, 0, 0));

    double *EFx = &(grid->EF.x(0, 0, 0)).real, *EFy = &(grid->EF.y(0, 0, 0)).real, *EFz = &(grid->EF.z(0, 0, 0)).real;
    double *BFx = &(grid->BF.x(0, 0, 0)).real, *BFy = &(grid->BF.y(0, 0, 0)).real, *BFz = &(grid->BF.z(0, 0, 0)).real;
    double *JFx = &(grid->JF.x(0, 0, 0)).real, *JFy = &(grid->JF.y(0, 0, 0)).real, *JFz = &(grid->JF.z(0, 0, 0)).real;

    Array3d<MyComplex>& a = grid->EF.x;

    const double cx = 2 * constants::pi / (grid->getEnd().x - grid->getStart().x);
    const double cy = 2 * constants::pi / (grid->getEnd().y - grid->getStart().y);
    const double cz = 2 * constants::pi / (grid->getEnd().z - grid->getStart().z);

    const int sizex = grid->sizeReal().x;
    const int sizey = grid->sizeReal().y;
    const int sizez = grid->sizeReal().z;

    const int sizex2 = grid->sizeReal().x / 2;
    const int sizey2 = grid->sizeReal().y / 2;
    const int sizez2 = grid->sizeReal().z / 2;

    //#pragma omp parallel for
    for (int i = 0; i < nx; i++)
        for (int j = 0; j < ny; j++) {

            const int chunkSize = 32;
            const int chunkNum = nz / chunkSize;
            const int lastChunkSize = nz % chunkSize;

            double Kx[chunkSize];
            double Ky[chunkSize];
            double Kz[chunkSize];

            //#pragma novector
#pragma ivdep
            for (int chunk = 0; chunk <= chunkNum; chunk++) {
                const int curChunkSize = chunk != chunkNum ? chunkSize : lastChunkSize;
#pragma ivdep
                for (int chunkIdx = 0; chunkIdx < curChunkSize; chunkIdx++)
                {
                    const int k = chunkSize * chunk + chunkIdx;

                    Kx[chunkIdx] = cx * ((i <= sizex2) ? i : i - sizex);
                    Ky[chunkIdx] = cy * ((j <= sizey2) ? j : j - sizey);
                    Kz[chunkIdx] = cz * ((k <= sizez2) ? k : k - sizez);
                }
#pragma ivdep
                for (int chunkIdx = 0; chunkIdx < curChunkSize; chunkIdx++)
                {
                    const int k = chunkSize * chunk + chunkIdx;

                    // vec3<MyComplex> K = getFreqVector(vec3<int>(i, j, k), *grid);

                    /*grid->EF.write(i, j, k, grid->EF(i, j, k) + constants::c * dt * MyComplex::i() *
                        vec3<MyComplex>::cross(K, grid->BF(i, j, k)) - 4 * constants::pi * grid->JF(i, j, k) * dt);*/

                    int ind = a.getIndex(i, j, k) * 2;

                    //EFx[ind] += constants::c * dt * MyComplex::i() * (K.y * BFz[ind] - K.z * BFy[ind]) - 4 * constants::pi * dt * JFx[ind];
                    //EFy[ind] += constants::c * dt * MyComplex::i() * (K.z * BFx[ind] - K.x * BFz[ind]) - 4 * constants::pi * dt * JFy[ind];
                    //EFz[ind] += constants::c * dt * MyComplex::i() * (K.x * BFy[ind] - K.y * BFx[ind]) - 4 * constants::pi * dt * JFz[ind];

                    //EFx[ind] += constants::c * dt * (Ky * BFz[ind] - Kz * BFy[ind]).multi() - 4 * constants::pi * dt * JFx[ind];
                    //EFy[ind] += constants::c * dt * (Kz * BFx[ind] - Kx * BFz[ind]).multi() - 4 * constants::pi * dt * JFy[ind];
                    //EFz[ind] += constants::c * dt * (Kx * BFy[ind] - Ky * BFx[ind]).multi() - 4 * constants::pi * dt * JFz[ind];

                    EFx[ind] += -constants::c * dt * (Ky[chunkIdx] * BFz[ind + 1] - Kz[chunkIdx] * BFy[ind + 1]) - 4.0 * constants::pi * dt * JFx[ind];  // real
                    EFx[ind + 1] += constants::c * dt * (Ky[chunkIdx] * BFz[ind] - Kz[chunkIdx] * BFy[ind]) - 4.0 * constants::pi * dt * JFx[ind + 1];  // complex

                    EFy[ind] += -constants::c * dt * (Kz[chunkIdx] * BFx[ind + 1] - Kx[chunkIdx] * BFz[ind + 1]) - 4.0 * constants::pi * dt * JFy[ind];  // real
                    EFy[ind + 1] += constants::c * dt * (Kz[chunkIdx] * BFx[ind] - Kx[chunkIdx] * BFz[ind]) - 4.0 * constants::pi * dt * JFy[ind + 1];  // complex

                    EFz[ind] += -constants::c * dt * (Kx[chunkIdx] * BFy[ind + 1] - Ky[chunkIdx] * BFx[ind + 1]) - 4.0 * constants::pi * dt * JFz[ind];  // real
                    EFz[ind + 1] += constants::c * dt * (Kx[chunkIdx] * BFy[ind] - Ky[chunkIdx] * BFx[ind]) - 4.0 * constants::pi * dt * JFz[ind + 1];  // complex
                }
            }
        }
}

void FieldSolverPSTD::refreshB(double dt) {
    int nx = grid->sizeComplex().x, ny = grid->sizeComplex().y, nz = grid->sizeComplex().z;

    // MyComplex *EFx = &(grid->EF.x(0, 0, 0)), *EFy = &(grid->EF.y(0, 0, 0)), *EFz = &(grid->EF.z(0, 0, 0));
    // MyComplex *BFx = &(grid->BF.x(0, 0, 0)), *BFy = &(grid->BF.y(0, 0, 0)), *BFz = &(grid->BF.z(0, 0, 0));
    // MyComplex *JFx = &(grid->JF.x(0, 0, 0)), *JFy = &(grid->JF.y(0, 0, 0)), *JFz = &(grid->JF.z(0, 0, 0));

    double *EFx = &(grid->EF.x(0, 0, 0)).real, *EFy = &(grid->EF.y(0, 0, 0)).real, *EFz = &(grid->EF.z(0, 0, 0)).real;
    double *BFx = &(grid->BF.x(0, 0, 0)).real, *BFy = &(grid->BF.y(0, 0, 0)).real, *BFz = &(grid->BF.z(0, 0, 0)).real;
    double *JFx = &(grid->JF.x(0, 0, 0)).real, *JFy = &(grid->JF.y(0, 0, 0)).real, *JFz = &(grid->JF.z(0, 0, 0)).real;

    Array3d<MyComplex>& a = grid->EF.x;

//#pragma omp parallel for
    for (int i = 0; i < nx; i++)
        for (int j = 0; j < ny; j++)
//#pragma novector
#pragma ivdep
            for (int k = 0; k < nz; k++) {

                // vec3<MyComplex> K = getFreqVector(vec3<int>(i, j, k), *grid);
                double Kx = getFreq(i, grid->sizeReal().x, grid->getStart().x, grid->getEnd().x);
                double Ky = getFreq(j, grid->sizeReal().y, grid->getStart().y, grid->getEnd().y);
                double Kz = getFreq(k, grid->sizeReal().z, grid->getStart().z, grid->getEnd().z);

                /*grid->BF.write(i, j, k, grid->BF(i, j, k) - constants::c * dt * MyComplex::i() *
                    vec3<MyComplex>::cross(K, grid->EF(i, j, k)));*/

                int ind = a.getIndex(i, j, k) * 2;

                //BFx[ind] -= constants::c * dt * MyComplex::i() * (K.y * EFz[ind] - K.z * EFy[ind]);
                //BFy[ind] -= constants::c * dt * MyComplex::i() * (K.z * EFx[ind] - K.x * EFz[ind]);
                //BFz[ind] -= constants::c * dt * MyComplex::i() * (K.x * EFy[ind] - K.y * EFx[ind]);

                //BFx[ind] -= constants::c * dt * (Ky * EFz[ind] - Kz * EFy[ind]).multi();
                //BFy[ind] -= constants::c * dt * (Kz * EFx[ind] - Kx * EFz[ind]).multi();
                //BFz[ind] -= constants::c * dt * (Kx * EFy[ind] - Ky * EFx[ind]).multi();

                BFx[ind] -= -constants::c * dt * (Ky * EFz[ind + 1] - Kz * EFy[ind + 1]);  // real
                BFx[ind + 1] -= constants::c * dt * (Ky * EFz[ind] - Kz * EFy[ind]);  // complex
                
                BFy[ind] -= -constants::c * dt * (Kz * EFx[ind + 1] - Kx * EFz[ind + 1]);  // real
                BFy[ind + 1] -= constants::c * dt * (Kz * EFx[ind] - Kx * EFz[ind]);  // complex
                
                BFz[ind] -= -constants::c * dt * (Kx * EFy[ind + 1] - Ky * EFx[ind + 1]);  // real
                BFz[ind + 1] -= constants::c * dt * (Kx * EFy[ind] - Ky * EFx[ind]);  // complex
            }
}

void FieldSolverPSTD::operator()(double dt)
{
    refreshE(dt);
    refreshB(dt);
}
