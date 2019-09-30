#pragma once
#include <map>
#include "vector3d.h"
#include "grid3d.h"
#include "class_member_ptr.h"

class Mask {
protected:
    std::string str;
    vec3<int> maskWidth;
    vec3<int> domainSize;
    vec3<int> guardSize;

public:
    Mask() {}
    Mask(vec3<int> _maskWidth, vec3<int> _domainSize, vec3<int> _guardSize) :
        maskWidth(_maskWidth), domainSize(_domainSize), guardSize(_guardSize) {}

    vec3<int> getMaskWidth() const {
        return maskWidth;
    }
    void setMaskWidth(vec3<int> mw) {
        maskWidth = mw;
    }
    std::string to_string() const {
        return str;
    }
    friend double operator==(const Mask& m1, const Mask& m2) {
        return (m1.func == m2.func);
    }
    friend double operator!=(const Mask& m1, const Mask& m2) {
        return (m1 != m2);
    }
    virtual double compMult(vec3<int> ind) = 0;
    void apply(Grid3d& grid);
    void apply(Grid3d& grid, Field f, Coordinate c);
};

class SimpleMask : public Mask {
public:
    SimpleMask() {}
    SimpleMask(vec3<int> _maskWidth, vec3<int> _domainSize, vec3<int> _guardSize) :
        Mask(_maskWidth, _domainSize, _guardSize) {}

    double compMult(vec3<int> ind) override;
};

class SmoothMask : public Mask {
public:
    SmoothMask() {}
    SmoothMask(vec3<int> _maskWidth, vec3<int> _domainSize, vec3<int> _guardSize) :
        Mask(_maskWidth, _domainSize, _guardSize) {}

    double compMult(vec3<int> ind) override;

private:
    double f(int i, int domainSize, int guardSize, int maskWidth);  // for one dim
};

//const std::map<std::string, Mask> MaskMap =
//{ { "simple", SimpleMask },{ "smooth", SmoothMask } };