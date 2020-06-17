#pragma once
#include <cmath>
#include <string>
#include "physical_constants.h"
#include "grid3d.h"

class Filter {
protected:
    vec3<int> maskWidth;
    vec3<int> numZeroFreq;

public:
    Filter() {}
    Filter(vec3<int> _maskWidth, vec3<int> _numZeroFreq) :
        maskWidth(_maskWidth), numZeroFreq(_numZeroFreq) {}

    void apply(Grid3d* gr) {
        if (state == on) applyFilter(gr);
    }

    void setState(State state) {
        this->state = state;
    }

    State getState() {
        return state;
    }

    void turnOff() {
        state = off;
    }

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

    virtual Filter* clone() const = 0;

    virtual std::string to_string() = 0;

protected:
    virtual void applyFilter(Grid3d* gr) = 0;

    State state = off;
};


class LowFreqFilter: public Filter {
public:
    LowFreqFilter(vec3<int> _maskWidth, vec3<int> _numZeroFreq) :
        Filter(_maskWidth, _numZeroFreq) {}
    LowFreqFilter() {}

    Filter* clone() const override {
        return new LowFreqFilter(*this);
    }

    std::string to_string() override {
        std::string str = "LowFreqFilter, state is ";
        switch (state) {
        case on:
            return str + "on";
        default:
            return str + "off";
        }
    }

protected:
    void applyFilter(Grid3d* gr) override;
    void filter1d(Grid3d* gr, int maskWidth_2, int numZeroFreq_2, Coordinate coord);
};