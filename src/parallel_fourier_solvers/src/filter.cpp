#include "filter.h"
#include "simple_types.h"
#include "class_member_ptr.h"
#include "vector3d.h"

void LowFreqFilter::filter1d(Grid3d& gr, int maskWidth_2, int numZeroFreq_2, Coordinate coord) {

    int a1 = gr.sizeComplex().*getMemberPtrCoord<int>(coord) / 2 
        - maskWidth_2 - numZeroFreq_2;
    int b1 = gr.sizeComplex().*getMemberPtrCoord<int>(coord) / 2 
        - numZeroFreq_2;
    int a2 = gr.sizeComplex().*getMemberPtrCoord<int>(coord) / 2 
        + maskWidth_2 + numZeroFreq_2;
    int b2 = gr.sizeComplex().*getMemberPtrCoord<int>(coord) / 2 
        + numZeroFreq_2;

    vec3<int> ind1 = vec3<int>::getVecIfCoord(coord, vec3<int>(a1), vec3<int>(0));
    vec3<int> ind2 = vec3<int>::getVecIfCoord(coord, vec3<int>(b1), gr.sizeComplex());

    for (int i = ind1.x; i < ind2.x; i++)
        for (int j = ind1.y; j < ind2.y; j++)
            for (int k = ind1.z; k < ind2.z; k++) {
                gr.EF.write(i, j, k, gr.EF(i, j, k) *
                    MyComplex(cos((constants::pi*(i - a1)) / (b1 - a1) * 0.5)*
                    cos((constants::pi*(i - a1)) / (b1 - a1) * 0.5)));
                gr.BF.write(i, j, k, gr.BF(i, j, k) *
                    MyComplex(cos((constants::pi*(i - a1)) / (b1 - a1) * 0.5)*
                    cos((constants::pi*(i - a1)) / (b1 - a1) * 0.5)));
            }

    ind1 = vec3<int>::getVecIfCoord(coord, vec3<int>(b2), vec3<int>(0));
    ind2 = vec3<int>::getVecIfCoord(coord, vec3<int>(a2), gr.sizeComplex());

    if (coord != z) {
        for (int i = ind1.x; i < ind2.x; i++)
            for (int j = ind1.y; j < ind2.y; j++)
                for (int k = ind1.z; k < ind2.z; k++) {
                    gr.EF.write(i, j, k, gr.EF(i, j, k) *
                        MyComplex(cos((constants::pi*(a2 - i)) / (a2 - b2) * 0.5)
                        * cos((constants::pi*(a2 - i)) / (a2 - b2) * 0.5)));
                    gr.BF.write(i, j, k, gr.BF(i, j, k) *
                        MyComplex(cos((constants::pi*(a2 - i)) / (a2 - b2) * 0.5)
                        * cos((constants::pi*(a2 - i)) / (a2 - b2) * 0.5)));
                }
    }

    ind1 = vec3<int>::getVecIfCoord(coord, vec3<int>(b1), vec3<int>(0));
    ind2 = vec3<int>::getVecIfCoord(coord, vec3<int>(b2), gr.sizeComplex());

    for (int i = ind1.x; i < ind2.x; i++)
        for (int j = ind1.y; j < ind2.y; j++)
            for (int k = ind1.z; k < fmin(ind2.z, gr.sizeComplex().z); k++) {
                gr.EF.write(i, j, k, vec3<MyComplex>(0));
                gr.BF.write(i, j, k, vec3<MyComplex>(0));
            }
}

void LowFreqFilter::applyFilter(Grid3d & gr)
{
    if (maskWidth.x != 0) filter1d(gr, (maskWidth.x - numZeroFreq.x) / 2,
        numZeroFreq.x / 2, x);
    if (maskWidth.y != 0) filter1d(gr, (maskWidth.y - numZeroFreq.y) / 2,
        numZeroFreq.y / 2, y);
    if (maskWidth.z != 0) filter1d(gr, (maskWidth.z - numZeroFreq.z) / 2,
        numZeroFreq.z / 2, z);
}