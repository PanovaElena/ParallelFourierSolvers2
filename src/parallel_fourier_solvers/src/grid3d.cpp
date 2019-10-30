#include "grid3d.h"

void Grid3d::initialize(const GridParams& gridParams) {
    clearGrid();

    this->gridParams = gridParams;

    vec3<int> n = gridParams.n;
    E.initialize(n);
    B.initialize(n);
    J.initialize(n);
    EF.initialize({ n.x, n.y, n.z / 2 + 1 });
    BF.initialize({ n.x, n.y, n.z / 2 + 1 });
    JF.initialize({ n.x, n.y, n.z / 2 + 1 });

    if (this->gridParams.isFieldFuncsSetted())
        setFields();
}

void Grid3d::setFields() {
    for (int i = 0; i < gridParams.n.x; i++)
        for (int j = 0; j < gridParams.n.y; j++)
            for (int k = 0; k < gridParams.n.z; k++) {
                E.write(i, j, k, gridParams.fE({ i,j,k }, 0.0));
                B.write(i, j, k, gridParams.fB({ i,j,k }, 0.0));
                J.write(i, j, k, gridParams.fJ({ i,j,k }, 0.0));
            }
}

void Grid3d::setJ(int iter) {
    for (int i = 0; i < sizeReal().x; i++)
        for (int j = 0; j < sizeReal().y; j++)
            for (int k = 0; k < sizeReal().z; k++)
                J.write(i, j, k, gridParams.fJ({ i,j,k }, iter));
}

int Grid3d::operator==(const Grid3d& gr) {
    if (gridParams.a != gr.gridParams.a)
        return 0;
    if (gridParams.d != gr.gridParams.d)
        return 0;
    if (gridParams.n != gr.gridParams.n)
        return 0;

    return (E == gr.E && B == gr.B && J == gr.J);
}