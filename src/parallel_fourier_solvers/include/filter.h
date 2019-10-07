#pragma once
#include <cmath>
#include <string>
#include "physical_constants.h"
#include "grid3d.h"

class Filter {
public:
    std::string to_string() const {
        switch (state) {
        case on:
            return "on";
        case off:
            return "off";
        default:
            return "off";
        }
    }

    void apply(Grid3d& gr) {
        if (state == on) applyFilter(gr);
    }

    void turnOn() {
        state = on;
    }

    void turnOff() {
        state = off;
    }

    virtual Filter* clone() const = 0;

protected:
    virtual void applyFilter(Grid3d& gr) = 0;

    State state = off;
};


class LowFreqFilter: public Filter {
private:
    vec3<int> maskWidth;
    vec3<int> numZeroFreq;

public:
    LowFreqFilter(vec3<int> _maskWidth, vec3<int> _numZeroFreq) :
        maskWidth(_maskWidth), numZeroFreq(_numZeroFreq) {}
    LowFreqFilter() {}

    void setParams(vec3<int> _maskWidth, vec3<int> _numZeroFreq) {
        maskWidth = _maskWidth;
        numZeroFreq = _numZeroFreq;
    }
    void setWidth(vec3<int> w) {
        maskWidth = w;
    }
    void setNumZeroFreq(vec3<int> nzf) {
        numZeroFreq = nzf;
    }
    vec3<int> getWidth() const {
        return maskWidth;
    }
    vec3<int> getNumZeroFreq() const {
        return numZeroFreq;
    }

    Filter* clone() const override {
        return new LowFreqFilter(*this);
    }

protected:
    void applyFilter(Grid3d& gr) override;
    void filter1d(Grid3d& gr, int maskWidth_2, int numZeroFreq_2, Coordinate coord);
};