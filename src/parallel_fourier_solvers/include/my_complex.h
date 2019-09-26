#pragma once
#include <cmath>
#include <iostream>

class MyComplex {
    double data[2];

public:
    MyComplex() {
        data[0] = 0;
        data[1] = 0;
    }
    MyComplex(double real, double imag) {
        data[0] = real;
        data[1] = imag;
    }
    MyComplex(double a) {
        data[0] = a;
        data[1] = 0;
    }
    static MyComplex getTrig(double r, double fi) {
        MyComplex z;
        z.data[0] = r * cos(fi);
        z.data[1] = r * sin(fi);
        return z;
    }
    void setReal(double real) {
        data[0] = real;
    }
    void setImag(double imag) {
        data[1] = imag;
    }
    double getReal() const {
        return data[0];
    }
    double getImag() const {
        return data[1];
    }
    double getAbs() const {
        return sqrt(data[0] * data[0] + data[1] * data[1]);
    }
    friend MyComplex operator*(const MyComplex& z, const double a) {
        return MyComplex(z.data[0] * a, z.data[1] * a);
    }
    friend MyComplex operator*(const double a, const MyComplex& z) {
        return MyComplex(z.data[0] * a, z.data[1] * a);
    }
    friend MyComplex operator*(const MyComplex& z1, const MyComplex& z2) {
        return MyComplex(z1.data[0] * z2.data[0] - z1.data[1] * z2.data[1],
            z1.data[0] * z2.data[1] + z2.data[0] * z1.data[1]);
    }
    friend MyComplex operator/(const MyComplex& z, const double a) {
        return MyComplex(z.data[0] / a, z.data[1] / a);
    }
    friend MyComplex operator+(const MyComplex& z1, const MyComplex& z2) {
        return MyComplex(z1.data[0] + z2.data[0], z1.data[1] + z2.data[1]);
    }
    MyComplex& operator+=(const MyComplex& z2) {
        data[0] += z2.data[0];
        data[1] += z2.data[1];
        return *this;
    }
    MyComplex& operator*=(const MyComplex& z2) {
        (*this) = (*this)*z2;
        return *this;
    }
    MyComplex& operator-=(const MyComplex& z2) {
        data[0] -= z2.data[0];
        data[1] -= z2.data[1];
        return *this;
    }
    friend MyComplex operator-(const MyComplex& z1, const MyComplex& z2) {
        return z1 + (-1)*z2;
    }
    friend bool operator==(const MyComplex& z1, const MyComplex& z2) {
        return (z1.data[0] == z2.data[0] && z1.data[1] == z2.data[1]);
    }
    friend bool operator!=(const MyComplex& z1, const MyComplex& z2) {
        return !(z1 == z2);
    }
    MyComplex conjugate() const {
        return MyComplex(data[0], -data[1]);
    }
    operator double() { return data[0]; }
	
	friend std::ostream& operator<<(std::ostream& ost, const MyComplex& c) {
        ost << "(" << c.data[0] << "," << c.data[1] << ")";
        return ost;
    }
	
	friend double abs(const MyComplex& a) {
        return sqrt(a.getReal()*a.getReal() + a.getImag()*a.getImag());
    }

};

const MyComplex complex_i(0, 1);
const MyComplex complex_0(0, 0);