#pragma once
#include <cmath>
#include <iostream>

class MyComplex {
    double real;
    double imag;
public:
    MyComplex() {
        real = 0;
        imag = 0;
    }
    MyComplex(double real, double imag) {
        real = real;
        imag = imag;
    }
    MyComplex(double a) {
        real = a;
        imag = 0;
    }
    static MyComplex getTrig(double r, double fi) {
        MyComplex z;
        z.real = r * cos(fi);
        z.imag = r * sin(fi);
        return z;
    }

    void setReal(double real) {
        real = real;
    }
    void setImag(double imag) {
        imag = imag;
    }
    double getReal() const {
        return real;
    }
    double getImag() const {
        return imag;
    }

    double getAbs() const {
        return sqrt(real * real + imag * imag);
    }
    friend MyComplex operator*(const MyComplex& z, const double a) {
        return MyComplex(z.real * a, z.imag * a);
    }
    friend MyComplex operator*(const double a, const MyComplex& z) {
        return MyComplex(z.real * a, z.imag * a);
    }
    friend MyComplex operator*(const MyComplex& z1, const MyComplex& z2) {
        return MyComplex(z1.real * z2.real - z1.imag * z2.imag,
            z1.real * z2.imag + z2.real * z1.imag);
    }
    friend MyComplex operator/(const MyComplex& z, const double a) {
        return MyComplex(z.real / a, z.imag / a);
    }
    friend MyComplex operator+(const MyComplex& z1, const MyComplex& z2) {
        return MyComplex(z1.real + z2.real, z1.imag + z2.imag);
    }
    MyComplex& operator+=(const MyComplex& z2) {
        real += z2.real;
        imag += z2.imag;
        return *this;
    }
    MyComplex& operator*=(const MyComplex& z2) {
        (*this) = (*this)*z2;
        return *this;
    }
    MyComplex& operator-=(const MyComplex& z2) {
        real -= z2.real;
        imag -= z2.imag;
        return *this;
    }
    friend MyComplex operator-(const MyComplex& z1, const MyComplex& z2) {
        return z1 + (-1)*z2;
    }
    friend bool operator==(const MyComplex& z1, const MyComplex& z2) {
        return (z1.real == z2.real && z1.imag == z2.imag);
    }
    friend bool operator!=(const MyComplex& z1, const MyComplex& z2) {
        return !(z1 == z2);
    }
    MyComplex conjugate() const {
        return MyComplex(real, -imag);
    }
    operator double() { return real; }
	
	friend std::ostream& operator<<(std::ostream& ost, const MyComplex& c) {
        ost << "(" << c.real << "," << c.imag << ")";
        return ost;
    }
	
	friend double abs(const MyComplex& a) {
        return sqrt(a.getReal()*a.getReal() + a.getImag()*a.getImag());
    }

    static MyComplex i() {
        return MyComplex(0, 1);
    }

};