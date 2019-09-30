#include <cmath>
#include "physical_constants.h"
#include "mask.h"
#include "class_member_ptr.h"
#include "array3d.h"

void Mask::apply(Grid3d& grid, Field f, Coordinate c)
{
    Array3d<double>& field = (grid.*getMemberPtrField<double>(f)).*getMemberPtrFieldCoord<double>(c);
#pragma omp parallel for
    for (int i = 0; i < grid.sizeReal().x; i++)
        for (int j = 0; j < grid.sizeReal().y; j++)
            for (int k = 0; k < grid.sizeReal().z; k++)
                field(i, j, k) *= compMult({ i, j, k });
}

void Mask::apply(Grid3d& grid)
{
    apply(grid, E, x);
    apply(grid, E, y);
    apply(grid, E, z);
    apply(grid, B, x);
    apply(grid, B, y);
    apply(grid, B, z);
    apply(grid, J, x);
    apply(grid, J, y);
    apply(grid, J, z);
}

double SimpleMask::compMult(vec3<int> ind)
{
    if (ind.x < guardSize.x || ind.x >= domainSize.x + guardSize.x ||
        ind.y < guardSize.y || ind.y >= domainSize.y + guardSize.y ||
        ind.z < guardSize.z || ind.z >= domainSize.z + guardSize.z) return 0;
    return 1;
}

double SmoothMask::compMult(vec3<int> ind)
{
    double res = 1;
    if (maskWidth.x > 0) res *= f(ind.x, domainSize.x, guardSize.x, maskWidth.x);
    if (maskWidth.y > 0) res *= f(ind.y, domainSize.y, guardSize.y, maskWidth.y);
    if (maskWidth.z > 0) res *= f(ind.z, domainSize.z, guardSize.z, maskWidth.z);
    return res;
}

double SmoothMask::f(int i, int domainSize, int guardSize, int maskWidth) {
    if (i < guardSize - maskWidth / 2 || i > guardSize + domainSize + maskWidth / 2)
        return 0;
    if (i <= guardSize + maskWidth / 2 && i >= guardSize - maskWidth / 2) {
        i -= guardSize - maskWidth / 2;
        return sin(i*constants::pi / (2 * maskWidth))*sin(i*constants::pi / (2 * maskWidth));
    }
    if (i <= guardSize + domainSize + maskWidth / 2 && i >= guardSize + domainSize - maskWidth / 2) {
        i -= guardSize + domainSize - maskWidth / 2;
        return cos(i*constants::pi / (2 * maskWidth))*cos(i*constants::pi / (2 * maskWidth));
    }
    return 1;
}
