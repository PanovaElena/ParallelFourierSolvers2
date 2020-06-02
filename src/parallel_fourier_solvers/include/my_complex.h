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
        this->real = real;
        this->imag = imag;
    }
    MyComplex(double a) {
        real = a;
        imag = 0;
    }
    __forceinline static MyComplex getTrig(double r, double fi) {
        MyComplex z;
        z.real = r * cos(fi);
        z.imag = r * sin(fi);
        return z;
    }

    __forceinline void setReal(double real) {
        this->real = real;
    }
    __forceinline void setImag(double imag) {
        this->imag = imag;
    }
    __forceinline double getReal() const {
        return real;
    }
    __forceinline double getImag() const {
        return imag;
    }

    __forceinline double getAbs() const {
        return sqrt(real * real + imag * imag);
    }
    __forceinline friend MyComplex operator*(const MyComplex& z, const double a) {
        return MyComplex(z.real * a, z.imag * a);
    }
    __forceinline friend MyComplex operator*(const double a, const MyComplex& z) {
        return MyComplex(z.real * a, z.imag * a);
    }
    __forceinline friend MyComplex operator*(const MyComplex& z1, const MyComplex& z2) {
        return MyComplex(z1.real * z2.real - z1.imag * z2.imag,
            z1.real * z2.imag + z2.real * z1.imag);
    }
    __forceinline friend MyComplex operator/(const MyComplex& z, const double a) {
        return MyComplex(z.real / a, z.imag / a);
    }
    __forceinline friend MyComplex operator+(const MyComplex& z1, const MyComplex& z2) {
        return MyComplex(z1.real + z2.real, z1.imag + z2.imag);
    }
    __forceinline MyComplex& operator+=(const MyComplex& z2) {
        real += z2.real;
        imag += z2.imag;
        return *this;
    }
    __forceinline MyComplex& operator*=(const MyComplex& z2) {
        (*this) = (*this)*z2;
        return *this;
    }
    __forceinline MyComplex& operator-=(const MyComplex& z2) {
        real -= z2.real;
        imag -= z2.imag;
        return *this;
    }
    __forceinline friend MyComplex operator-(const MyComplex& z1, const MyComplex& z2) {
        return z1 + (-1)*z2;
    }
    __forceinline friend bool operator==(const MyComplex& z1, const MyComplex& z2) {
        return (z1.real == z2.real && z1.imag == z2.imag);
    }
    __forceinline friend bool operator!=(const MyComplex& z1, const MyComplex& z2) {
        return !(z1 == z2);
    }
    __forceinline MyComplex conjugate() const {
        return MyComplex(real, -imag);
    }
    __forceinline operator double() { return real; }
	
    __forceinline friend std::ostream& operator<<(std::ostream& ost, const MyComplex& c) {
        ost << "(" << c.real << "," << c.imag << ")";
        return ost;
    }
	
    __forceinline friend double abs(const MyComplex& a) {
        return sqrt(a.getReal()*a.getReal() + a.getImag()*a.getImag());
    }

    __forceinline static MyComplex i() {
        return MyComplex(0, 1);
    }

};