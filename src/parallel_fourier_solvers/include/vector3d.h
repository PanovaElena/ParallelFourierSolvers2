#pragma once
#include <cmath>
#include <iostream>
#include <string>
#include <initializer_list>
#include "my_complex.h"
#include "simple_types.h"


inline double absSquare(double a) {
    return a * a;
}
inline double absSquare(MyComplex a) {
    return a.getReal()*a.getReal() + a.getImag()*a.getImag();
}

template <class T = double>
class vec3
{
public:
    T x;
    T y;
    T z;

    vec3() {}

    vec3(T a, T b, T c) : x(a), y(b), z(c) {
    };

    vec3(std::initializer_list<T> l) {
        if (l.size() < 3) return;
        x = *(l.begin());
        y = *(l.begin() + 1);
        z = *(l.begin() + 2);
    };

    explicit vec3(T a) : x(a), y(a), z(a) {
    };

    static vec3<int> getBaseVector(int index) {
        vec3<int> res(0);
        res[index] = 1;
        return res;
    }

    vec3& operator=(const vec3& v) {
        x = v.x;
        y = v.y;
        z = v.z;
        return *this;
    }

    T& operator[](int i) {
        switch (i) {
        case 0: return x;
        case 1: return y;
        case 2: return z;
        default: return x;
        }
    };

    T operator[](int i) const {
        switch (i) {
        case 0: return x;
        case 1: return y;
        case 2: return z;
        default: return x;
        }
    };

    vec3(const vec3<T>& c) : x(c.x), y(c.y), z(c.z) {
    };

    template<class T>
    operator vec3<T>() const {
        return vec3<T>(x, y, z);
    }


    friend vec3 operator+(const vec3& v1, const vec3& v2) {
        return vec3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
    };
    friend vec3 operator-(const vec3& v1, const vec3& v2) {
        return vec3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
    };


    friend vec3 operator* (T b, const vec3& v) {
        return v * b;
    };
    friend vec3 operator* (const vec3& v, T b) {
        return vec3(v.x*b, v.y*b, v.z*b);
    };
    //by components
    friend vec3 operator*(const vec3& v1, const vec3& v2) {
        return vec3(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
    };


    friend vec3 operator/(const vec3& v, T b) {
        return vec3(v.x / b, v.y / b, v.z / b);
    };
    //by components
    friend vec3 operator/(const vec3& v1, const vec3& v2) {
        return vec3(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z);
    };


    friend vec3 operator%(const vec3& v, T b) {
        return vec3(v.x % b, v.y % b, v.z % b);
    };
    //by components
    friend vec3 operator%(const vec3& v1, const vec3& v2) {
        return vec3(v1.x % v2.x, v1.y % v2.y, v1.z % v2.z);
    };


    friend bool operator==(const vec3& a, const vec3& b)
    {
        return ((a.x == b.x) && (a.y == b.y) && (a.z == b.z));
    };

    friend bool operator!=(const vec3& a, const vec3& b) {
        return !(a == b);
    };

    friend bool operator<(const vec3& a, const vec3& b) {
        return ((a.x < b.x) && (a.y < b.y) && (a.z < b.z));
    }

    friend bool operator<=(const vec3& a, const vec3& b) {
        return ((a.x <= b.x) && (a.y <= b.y) && (a.z <= b.z));
    }

    double getNorm() const
    {
        return sqrt(absSquare(x) + absSquare(y) + absSquare(z));
    };
    vec3 normilize() {
        if (getNorm() == 0) return *this;
        (*this) = (*this)*(1.0 / getNorm());
        return *this;
    };

    static T dot(const vec3& a, const vec3& b) {
        return (a.x * b.x + a.y * b.y + a.z * b.z);
    };
    static vec3 cross(const vec3& a, const vec3& b) {
        T c1, c2, c3;
        c1 = a.y * b.z - a.z * b.y;
        c2 = a.z * b.x - a.x * b.z;
        c3 = a.x * b.y - a.y * b.x;
        return vec3(c1, c2, c3);
    };

    friend std::ostream& operator<<(std::ostream& ost, const vec3<T>& vec) {
        ost << "(" << vec.x << "," << vec.y << "," << vec.z << ")";;
        return ost;
    }

    friend std::string to_string(const vec3<T>& vec) {
        std::string str = "(" + std::to_string((long long)vec.x) + "," +
            std::to_string((long long)vec.y) + "," +
            std::to_string((long long)vec.z) + ")";
        return str;
    }


    template<class T>
    static vec3<T> getVecIfCoord(Coordinate coord, const vec3<T>& valCoord, const vec3<T>& valOther) {
        return vec3<T>(coord == Coordinate::x ? valCoord.x : valOther.x,
            coord == Coordinate::y ? valCoord.y : valOther.y,
            coord == Coordinate::z ? valCoord.z : valOther.z);
    }
};