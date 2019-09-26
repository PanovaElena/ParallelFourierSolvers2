#include "grid3d.h"

Grid3d::Grid3d() :E(), B(), J(), EF(), BF(), JF() {}

Grid3d::Grid3d(const Grid3d& gr) {
    initialize(gr.n, gr.a, gr.b);

    E = gr.E;
    B = gr.B;
    J = gr.J;
    EF = gr.EF;
    BF = gr.BF;
    JF = gr.JF;
}

Grid3d::Grid3d(vec3<int> n, vec3<double> a, vec3<double> b) {
    initialize(n, a, b);
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

void Grid3d::initialize(vec3<int> _n, vec3<double> _a, vec3<double> _b) {
    clearGrid();

    n = _n; a = _a; b = _b;

    d = (b - a) / (vec3<double>)n;

    E.initialize(n);
    B.initialize(n);
    J.initialize(n);
    EF.initialize({ n.x, n.y, n.z / 2 + 1 });
    BF.initialize({ n.x, n.y, n.z / 2 + 1 });
    JF.initialize({ n.x, n.y, n.z / 2 + 1 });
}

int Grid3d::operator==(const Grid3d& gr) {
    if (a.x != gr.a.x) return 0;
    if (a.y != gr.a.y) return 0;
    if (a.z != gr.a.z) return 0;
    if (b.x != gr.b.x) return 0;
    if (b.y != gr.b.y) return 0;
    if (b.z != gr.b.z) return 0;

    return (E == gr.E && B == gr.B && J == gr.J);
}

Grid3d& Grid3d::operator=(const Grid3d& gr) {
    initialize(gr.n, gr.a, gr.b);
    E = gr.E;
    B = gr.B;
    J = gr.J;
    EF = gr.EF;
    BF = gr.BF;
    JF = gr.JF;
    return *this;
}

vec3<int> Grid3d::sizeReal() const {
    return n;
}

vec3<int> Grid3d::sizeComplex() const {
    return { n.x, n.y, n.z / 2 + 1 };
}

vec3<double> Grid3d::getStep() const {
    return d;
}

vec3<double> Grid3d::getStart() const {
    return a;
}

vec3<double> Grid3d::getEnd() const {
    return b;
}
