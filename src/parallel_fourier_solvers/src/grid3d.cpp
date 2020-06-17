#include "grid3d.h"

Grid3d::Grid3d() :E(), B(), J(), EF(), BF(), JF() {}

Grid3d::Grid3d(const Grid3d& gr) {
    create(gr.gridParams, gr.allocLocal);
    setFields();
}

Grid3d::Grid3d(const GridParams& gridParams) {
    create(gridParams);
}

Grid3d::Grid3d(const GridParams& gridParams, size_t allocLocal) {
    create(gridParams, allocLocal);
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

void Grid3d::create(const GridParams& gridParams) {
    vec3<int> n = gridParams.n;
    create(gridParams, n.x*n.y*(n.z / 2 + 1));
}

void Grid3d::create(const GridParams& gridParams, size_t allocLocal) {
    clearGrid();

    this->gridParams = gridParams;
    this->allocLocal = allocLocal;

    vec3<int> n = gridParams.n;

    E.initialize({ n.x, n.y, 2 * (n.z / 2 + 1) }, 2 * this->allocLocal);
    B.initialize({ n.x, n.y, 2 * (n.z / 2 + 1) }, 2 * this->allocLocal);
    J.initialize({ n.x, n.y, 2 * (n.z / 2 + 1) }, 2 * this->allocLocal);
    // complex fields have the same memory
    EF.initialize(E, sizeComplex());
    BF.initialize(B, sizeComplex());
    JF.initialize(J, sizeComplex());
}

void Grid3d::setFields() {
    for (int i = 0; i < gridParams.n.x; i++)
        for (int j = 0; j < gridParams.n.y; j++)
            for (int k = 0; k < gridParams.n.z; k++) {
                vec3<> cEx = gridParams.getCoord({ i,j,k }, Field::E, Coordinate::x);
                vec3<> cEy = gridParams.getCoord({ i,j,k }, Field::E, Coordinate::y);
                vec3<> cEz = gridParams.getCoord({ i,j,k }, Field::E, Coordinate::z);
                E.write(i, j, k, { gridParams.fE(cEx, 0.0).x,
                    gridParams.fE(cEy, gridParams.startTime).y,
                    gridParams.fE(cEz, gridParams.startTime).z });

                vec3<> cBx = gridParams.getCoord({ i,j,k }, Field::B, Coordinate::x);
                vec3<> cBy = gridParams.getCoord({ i,j,k }, Field::B, Coordinate::y);
                vec3<> cBz = gridParams.getCoord({ i,j,k }, Field::B, Coordinate::z);
                B.write(i, j, k, { gridParams.fB(cBx, gridParams.startTime).x,
                    gridParams.fB(cBy, gridParams.startTime).y,
                    gridParams.fB(cBz, gridParams.startTime).z });

                vec3<> cJx = gridParams.getCoord({ i,j,k }, Field::J, Coordinate::x);
                vec3<> cJy = gridParams.getCoord({ i,j,k }, Field::J, Coordinate::y);
                vec3<> cJz = gridParams.getCoord({ i,j,k }, Field::J, Coordinate::z);
                J.write(i, j, k, { gridParams.fJ(cJx, gridParams.startTime).x,
                    gridParams.fJ(cJy, gridParams.startTime).y,
                    gridParams.fJ(cJz, gridParams.startTime).z });
            }
}

void Grid3d::setJ(int iter) {
    for (int i = 0; i < sizeReal().x; i++)
        for (int j = 0; j < sizeReal().y; j++)
            for (int k = 0; k < sizeReal().z; k++) {
                vec3<> cJx = gridParams.getCoord({ i,j,k }, Field::J, Coordinate::x);
                vec3<> cJy = gridParams.getCoord({ i,j,k }, Field::J, Coordinate::y);
                vec3<> cJz = gridParams.getCoord({ i,j,k }, Field::J, Coordinate::z);
                double tx = gridParams.getTime(iter, Field::J, Coordinate::x);
                double ty = gridParams.getTime(iter, Field::J, Coordinate::y);
                double tz = gridParams.getTime(iter, Field::J, Coordinate::z);
                J.write(i, j, k, { gridParams.fJ(cJx, tx).x,
                    gridParams.fJ(cJy, ty).y,
                    gridParams.fJ(cJz, tz).z });
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
    if (this != &gr) {
        create(gr.gridParams, gr.allocLocal);
        setFields();
    }
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
