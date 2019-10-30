#include "field_solver.h"


void FieldSolverPSTD::refreshE(double dt) {
    int nx = grid->sizeComplex().x, ny = grid->sizeComplex().y, nz = grid->sizeComplex().z;

#pragma omp parallel for
    for (int i = 0; i < nx; i++)
        for (int j = 0; j < ny; j++) {

            const int chunkSize = 32;
            const int chunkNum = nz / chunkSize;
            const int lastChunkSize = nz % chunkSize;

            vec3<MyComplex> K[chunkSize];

            for (int chunk = 0; chunk <= chunkNum; chunk++) {
                const int curChunkSize = chunk != chunkNum ? chunkSize : lastChunkSize;
#pragma ivdep
                for (int chunkIdx = 0; chunkIdx < curChunkSize; chunkIdx++)
                {
                    const int k = chunkSize * chunk + chunkIdx;

                    K[chunkIdx].x = getFreq(i, grid->sizeReal().x, grid->getStart().x, grid->getEnd().x);
                    K[chunkIdx].y = getFreq(j, grid->sizeReal().y, grid->getStart().y, grid->getEnd().y);
                    K[chunkIdx].z = getFreq(k, grid->sizeReal().z, grid->getStart().z, grid->getEnd().z);
                }
#pragma ivdep
                for (int chunkIdx = 0; chunkIdx < curChunkSize; chunkIdx++)
                {
                    const int k = chunkSize * chunk + chunkIdx;

                    grid->EF.write(i, j, k, grid->EF(i, j, k) + constants::c * dt * MyComplex::i() *
                        vec3<MyComplex>::cross(K[chunkIdx], grid->BF(i, j, k)) - 4 * constants::pi * grid->JF(i, j, k) * dt);
                }
            }
        }
}

void FieldSolverPSTD::refreshB(double dt) {
    int nx = grid->sizeComplex().x, ny = grid->sizeComplex().y, nz = grid->sizeComplex().z;

#pragma omp parallel for
    for (int i = 0; i < nx; i++)
        for (int j = 0; j < ny; j++) {

            const int chunkSize = 32;
            const int chunkNum = nz / chunkSize;
            const int lastChunkSize = nz % chunkSize;

            vec3<MyComplex> K[chunkSize];

            for (int chunk = 0; chunk <= chunkNum; chunk++) {
                const int curChunkSize = chunk != chunkNum ? chunkSize : lastChunkSize;
#pragma ivdep
                for (int chunkIdx = 0; chunkIdx < curChunkSize; chunkIdx++)
                {
                    const int k = chunkSize * chunk + chunkIdx;

                    K[chunkIdx].x = getFreq(i, grid->sizeReal().x, grid->getStart().x, grid->getEnd().x);
                    K[chunkIdx].y = getFreq(j, grid->sizeReal().y, grid->getStart().y, grid->getEnd().y);
                    K[chunkIdx].z = getFreq(k, grid->sizeReal().z, grid->getStart().z, grid->getEnd().z);
                }
#pragma ivdep
                for (int chunkIdx = 0; chunkIdx < curChunkSize; chunkIdx++)
                {
                    const int k = chunkSize * chunk + chunkIdx;

                    grid->BF.write(i, j, k, grid->BF(i, j, k) - constants::c * dt * MyComplex::i() *
                        vec3<MyComplex>::cross(K[chunkIdx], grid->EF(i, j, k)));
                }
            }
        }
}


//void FieldSolverPSTD::refreshE(double dt) {
//#pragma omp parallel for
//    for (int i = 0; i < grid->sizeComplex().x; i++)
//        for (int j = 0; j < grid->sizeComplex().y; j++)
//            for (int k = 0; k < grid->sizeComplex().z; k++) {
//
//                vec3<MyComplex> K = getFreqVector(vec3<int>(i, j, k), *grid);
//
//                grid->EF.write(i, j, k, grid->EF(i, j, k) + constants::c * dt * MyComplex::i() *
//                    vec3<MyComplex>::cross(K, grid->BF(i, j, k)) - 4 * constants::pi * grid->JF(i, j, k) * dt);
//            }
//}
//
//void FieldSolverPSTD::refreshB(double dt) {
//#pragma omp parallel for
//    for (int i = 0; i < grid->sizeComplex().x; i++)
//        for (int j = 0; j < grid->sizeComplex().y; j++)
//            for (int k = 0; k < grid->sizeComplex().z; k++) {
//
//                vec3<MyComplex> K = getFreqVector(vec3<int>(i, j, k), *grid);
//
//                grid->BF.write(i, j, k, grid->BF(i, j, k) - constants::c * dt * MyComplex::i() *
//                    vec3<MyComplex>::cross(K, grid->EF(i, j, k)));
//       


//void FieldSolverPSTD::refreshE(double dt) {
//    int nx = grid->sizeComplex().x, ny = grid->sizeComplex().y, nz = grid->sizeComplex().z;
//#pragma omp parallel for
//    for (int i = 0; i < nx; i++)
//        for (int j = 0; j < ny; j++)
//#pragma ivdep
//            for (int k = 0; k < nz; k++) {
//
//                vec3<MyComplex> K = getFreqVector(vec3<int>(i, j, k), *grid);
//
//                grid->EF.write(i, j, k, grid->EF(i, j, k) + constants::c * dt * MyComplex::i() *
//                    vec3<MyComplex>::cross(K, grid->BF(i, j, k)) - 4 * constants::pi * grid->JF(i, j, k) * dt);
//            }
//}
//
//void FieldSolverPSTD::refreshB(double dt) {
//    int nx = grid->sizeComplex().x, ny = grid->sizeComplex().y, nz = grid->sizeComplex().z;
//#pragma omp parallel for
//    for (int i = 0; i < nx; i++)
//        for (int j = 0; j < ny; j++)
//#pragma ivdep
//            for (int k = 0; k < nz; k++) {
//
//                vec3<MyComplex> K = getFreqVector(vec3<int>(i, j, k), *grid);
//
//                grid->BF.write(i, j, k, grid->BF(i, j, k) - constants::c * dt * MyComplex::i() *
//                    vec3<MyComplex>::cross(K, grid->EF(i, j, k)));
//            }
//}

void FieldSolverPSTD::operator()(double dt)
{
    refreshE(dt);
    refreshB(dt);
}
