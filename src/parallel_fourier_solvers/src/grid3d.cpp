#include "grid3d.h"

Grid3d::Grid3d() :E(), B(), J(), EF(), BF(), JF() {}

Grid3d::Grid3d(const Grid3d& gr) {
    initialize(gr.gridParams);
}

Grid3d::Grid3d(const GridParams& gridParams, bool ifMpiFFT, int allocLocal) {
    initialize(gridParams, ifMpiFFT, allocLocal);
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

void Grid3d::initialize(const GridParams& gridParams, bool ifMpiFFT, int allocLocal) {
    clearGrid();

    this->gridParams = gridParams;

    vec3<int> n = gridParams.n;

	if (ifMpiFFT) {
		if (allocLocal < n.x * n.y * (n.z / 2 + 1))
			allocLocal = n.x * n.y * (n.z / 2 + 1);
		E.initialize(2 * allocLocal, { n.x, n.y, 2 * (n.z / 2 + 1) });
		B.initialize(2 * allocLocal, { n.x, n.y, 2 * (n.z / 2 + 1) });
		J.initialize(2 * allocLocal, { n.x, n.y, 2 * (n.z / 2 + 1) });
		EF.initialize(allocLocal, { n.x, n.y, n.z / 2 + 1 });
		BF.initialize(allocLocal, { n.x, n.y, n.z / 2 + 1 });
		JF.initialize(allocLocal, { n.x, n.y, n.z / 2 + 1 });
	}
	else {
		E.initialize(n);
		B.initialize(n);
		J.initialize(n);
		EF.initialize({ n.x, n.y, n.z / 2 + 1 });
		BF.initialize({ n.x, n.y, n.z / 2 + 1 });
		JF.initialize({ n.x, n.y, n.z / 2 + 1 });
	}

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

Grid3d& Grid3d::operator=(const Grid3d& gr) {
    if (this != &gr)
        initialize(gr.gridParams);
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
