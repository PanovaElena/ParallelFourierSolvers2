#include "grid3d.h"

Grid3d::Grid3d() :E(), B(), J(), EF(), BF(), JF() {}

Grid3d::Grid3d(const Grid3d& gr) {
    initialize(gridParams);
}

Grid3d::Grid3d(const GridParams& gridParams) {
    initialize(gridParams);
}

void Grid3d::clearGrid() {
    E.clear();
    B.clear();
    J.clear();
    EF.clear();
    BF.clear();
    JF.clear();
}

Grid3d::~Grid3d() {
    clearGrid();
}

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

    for (int i = 0; i < n.x; i++)
        for (int j = 0; j < n.y; j++)
            for (int k = 0; i < n.z; k++) {
                E.write(i, j, k, gridParams.fE({ i,j,k }, 0.0));
                B.write(i, j, k, gridParams.fB({ i,j,k }, 0.0));
                J.write(i, j, k, gridParams.fJ({ i,j,k }, 0.0));
            }

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

Grid3d& Grid3d::operator=(const Grid3d& gr) {
    initialize(gr.gridParams);
    E = gr.E;
    B = gr.B;
    J = gr.J;
    EF = gr.EF;
    BF = gr.BF;
    JF = gr.JF;
    return *this;
}

vec3<int> Grid3d::sizeReal() const {
    return gridParams.n;
}

vec3<int> Grid3d::sizeComplex() const {
    return { gridParams.n.x, gridParams.n.y,
        gridParams.n.z / 2 + 1 };
}

vec3<double> Grid3d::getStep() const {
    return gridParams.d;
}

vec3<double> Grid3d::getStart() const {
    return gridParams.a;
}

vec3<double> Grid3d::getEnd() const {
    return gridParams.b();
}
