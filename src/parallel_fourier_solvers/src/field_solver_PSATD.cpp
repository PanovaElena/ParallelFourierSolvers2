#include "field_solver.h"


__forceinline void compField(Grid3d* grid, int i, int j, int k, vec3<MyComplex>& K, double normK,
    const vec3<MyComplex>& El, const vec3<MyComplex>& Jl, double dt) {

    const double C = cos(normK * constants::c * dt);
    const double S = sin(normK * constants::c * dt);
    vec3<MyComplex> E = grid->EF(i, j, k), B = grid->BF(i, j, k),
        J = 4 * constants::pi * grid->JF(i, j, k);

    K = K / normK;

    grid->EF.write(i, j, k, C * E + MyComplex::i() * S * vec3<MyComplex>::cross(K, B) -
        S / (normK*constants::c) * J + (1 - C) * El + Jl * (S / (normK*constants::c) - dt));
    grid->BF.write(i, j, k, C * B - MyComplex::i() * S * vec3<MyComplex>::cross(K, E) +
        MyComplex::i() * ((1 - C) / (normK*constants::c))*vec3<MyComplex>::cross(K, J));
}


void FieldSolverPSATD::operator()(double dt)
{
    int nx = grid->sizeComplex().x, ny = grid->sizeComplex().y, nz = grid->sizeComplex().z;

#pragma omp parallel for
    for (int i = 0; i < nx; i++) {
        for (int j = 0; j < ny; j++) {

            const int chunkSize = 32;
            const int chunkNum = nz / chunkSize;
            const int lastChunkSize = nz % chunkSize;

            vec3<MyComplex> K[chunkSize];
            vec3<MyComplex> Jl[chunkSize];
            vec3<MyComplex> El[chunkSize];
            double normK[chunkSize];

            for (int chunk = 0; chunk <= chunkNum; chunk++) {
                const int curChunkSize = chunk != chunkNum ? chunkSize : lastChunkSize;
#pragma ivdep
                for (int chunkIdx = 0; chunkIdx < curChunkSize; chunkIdx++)
                {
                    const int k = chunkSize * chunk + chunkIdx;

                    K[chunkIdx].x = getFreq(i, grid->sizeReal().x, grid->getStart().x, grid->getEnd().x);
                    K[chunkIdx].y = getFreq(j, grid->sizeReal().y, grid->getStart().y, grid->getEnd().y);
                    K[chunkIdx].z = getFreq(k, grid->sizeReal().z, grid->getStart().z, grid->getEnd().z);

                    normK[chunkIdx] = K[chunkIdx].getNorm();

                    Jl[chunkIdx] = K[chunkIdx] * vec3<MyComplex>::dot(K[chunkIdx], 4 * constants::pi * grid->JF(i, j, k));
                    El[chunkIdx] = K[chunkIdx] * vec3<MyComplex>::dot(K[chunkIdx], grid->EF(i, j, k));
                }

                // chunkIdx == 0
                if (i == 0 && j == 0 && chunk == 0) {
                    grid->EF.write(0, 0, 0, grid->EF(0, 0, 0) - 4 * constants::pi * grid->JF(0, 0, 0));
                }
                else {
                    const int k = chunkSize * chunk;
                    compField(grid, i, j, k, K[0], normK[0], El[0], Jl[0], dt);
                }

                // chunkIdx != 0
#pragma ivdep
                for (int chunkIdx = 1; chunkIdx < curChunkSize; chunkIdx++)
                {
                    const int k = chunkSize * chunk + chunkIdx;
                    compField(grid, i, j, k, K[chunkIdx], normK[chunkIdx], El[chunkIdx], Jl[chunkIdx], dt);
                }
            }
        }
    }
}


//void FieldSolverPSATD::operator()(double dt)
//{
////#pragma omp parallel for
//    for (int i = 0; i < grid->sizeComplex().x; i++)
//        for (int j = 0; j < grid->sizeComplex().y; j++)
//            for (int k = 0; k < grid->sizeComplex().z; k++) {
//
//                vec3<MyComplex> K = getFreqVector(vec3<int>(i, j, k), *grid);
//                double normK = K.getNorm();
//                K.normilize();
//
//                double C = cos(normK*constants::c*dt);
//                double S = sin(normK*constants::c*dt);
//                vec3<MyComplex> E = grid->EF(i, j, k), B = grid->BF(i, j, k),
//                    J = 4 * constants::pi * grid->JF(i, j, k);
//                vec3<MyComplex> Jl = K * vec3<MyComplex>::dot(K, J);
//                vec3<MyComplex> El = K * vec3<MyComplex>::dot(K, E);
//
//                if (normK == 0) {
//                    grid->EF.write(i, j, k, E - J);
//                    continue;
//                }
//
//                grid->EF.write(i, j, k, C*E + MyComplex::i() * S*vec3<MyComplex>::cross(K, B) -
//                    S / (normK*constants::c)*J + (1 - C)*El + Jl * (S / (normK*constants::c) - dt));
//                grid->BF.write(i, j, k, C*B - MyComplex::i() * S*vec3<MyComplex>::cross(K, E) +
//                    MyComplex::i() * ((1 - C) / (normK*constants::c))*vec3<MyComplex>::cross(K, J));
//            }
//}
