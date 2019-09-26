#pragma once
#include <vector>
#include "vector3d.h"

template <class T>
class Array3d {
    vec3<int> n;

    std::vector<T> tmp1;  // structure for allocation of memory
    std::vector<T*> tmp2;
    std::vector<T**> data;

    void allocMem();

public:
    void initialize(vec3<int> n);
    Array3d();
    Array3d(const Array3d& arr);
    Array3d(vec3<int> n);
    ~Array3d() { clear(); }

    friend int operator==(const Array3d<T>& arr1, const Array3d<T>& arr2) {
        if (arr1.n.x != arr2.n.x || arr1.n.y != arr2.n.y || arr1.n.z != arr2.n.z) return 0;
        for (int i = 0; i < arr1.n.x; i++)
            for (int j = 0; j < arr1.n.y; j++)
                for (int k = 0; k < arr1.n.z; k++)
                    if (arr1.data[i][j][k] != arr2.data[i][j][k]) return 0;
        return 1;
    };
    friend int operator!=(const Array3d<T>& arr1, const Array3d<T>& arr2) {
        return !(arr1 == arr2);
    };
    T& operator()(int i, int j, int k);
    T& operator()(vec3<int> index);
    T& get(int i, int j, int k);
    T& operator[](int index);
    T& operator()(int index);

    T operator()(int i, int j, int k) const;
    T operator()(vec3<int> index) const;
    T get(int i, int j, int k) const;
    T operator[](int index) const;
    T operator()(int index) const;

    Array3d& operator=(const Array3d& arr);

    T* getArray1d();

    vec3<int> size() const { return n; };
    int size1d() const { return n.x * n.y * n.z; };

    void clear();
    void setToZeros();
};

template<class T>
inline void Array3d<T>::allocMem()
{
    tmp1.resize(n.x*n.y*n.z);
    tmp2.resize(n.x*n.y);
    data.resize(n.x);

    for (int i = 0; i < n.x*n.y; i++)
        tmp2[i] = &(tmp1[i*n.z]);
    for (int i = 0; i < n.x; i++)
        data[i] = &(tmp2[i*n.y]);
}

template<class T>
inline void Array3d<T>::initialize(vec3<int> _n) {
    clear();
    n.x = _n.x; n.y = _n.y; n.z = _n.z;
    allocMem();
    setToZeros();
}

template<class T>
inline Array3d<T>::Array3d() {
}

template<class T>
inline Array3d<T>::Array3d(const Array3d & arr) {
    n = arr.n;

    allocMem();

    for (int i = 0; i < n.x; i++)
        for (int j = 0; j < n.y; j++)
            for (int k = 0; k < n.z; k++)
                (*this)(i, j, k) = arr.data[i][j][k];
}

template<class T>
inline Array3d<T>::Array3d(vec3<int> _n) {
    initialize(_n);
}

template<class T>
inline T & Array3d<T>::operator()(int i, int j, int k) {
    return data[i][j][k];
}

template<class T>
inline T & Array3d<T>::operator()(vec3<int> index) {
    return data[index.x][index.y][index.z];
}

template<class T>
inline T & Array3d<T>::get(int i, int j, int k) {
    return data[i][j][k];
}

template<class T>
inline T & Array3d<T>::operator[](int index) {
    return tmp1[index];
}

template<class T>
inline T & Array3d<T>::operator()(int index) {
    return tmp1[index];
}

template<class T>
inline T Array3d<T>::operator()(int i, int j, int k) const {
    return data[i][j][k];
}

template<class T>
inline T Array3d<T>::operator()(vec3<int> index) const {
    return data[index.x][index.y][index.z];
}

template<class T>
inline T Array3d<T>::get(int i, int j, int k) const {
    return data[i][j][k];
}

template<class T>
inline T Array3d<T>::operator[](int index) const {
    return tmp1[index];
}

template<class T>
inline T Array3d<T>::operator()(int index) const {
    return tmp1[index];
}

template<class T>
inline Array3d<T> & Array3d<T>::operator=(const Array3d & arr) {
    initialize(arr.n);

    for (int i = 0; i < n.x; i++)
        for (int j = 0; j < n.y; j++)
            for (int k = 0; k < n.z; k++)
                (*this)(i, j, k) = arr.data[i][j][k];

    return *this;
}

template<class T>
inline T * Array3d<T>::getArray1d() {
    return tmp1;
}

template<class T>
inline void Array3d<T>::clear() {
    data.resize(0);
    tmp2.resize(0);
    tmp1.resize(0);
}

template<class T>
inline void Array3d<T>::setToZeros() {
    for (int i = 0; i < n.x*n.y*n.z; i++)
        tmp1[i] = 0;
}
