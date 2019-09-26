#pragma once
#include <cmath>
#include <string>
#include "physical_constants.h"
#include "grid3d.h"

class Filter {
public:
    enum State {
        on,
        off
    };

    vec3<int> maskWidth;
    vec3<int> numZeroFreq;

    Filter(vec3<int> _maskWidth, vec3<int> _numZeroFreq) :maskWidth(_maskWidth), numZeroFreq(_numZeroFreq) {}
    Filter() {}

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

    State state = off;
    void operator() (Grid3d& gr) {
        if (state == on) lowFrequencyFilter(gr);
    }
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

private:
    void lowFrequencyFilter(Grid3d& gr);
    void filter1d(Grid3d& gr, int maskWidth_2, int numZeroFreq_2, Coordinate coord);
};