#pragma once
#include <vector>
#include "vector3d.h"

template <class T>
class Array3d {
    vec3<int> n;
	int n1d;
    std::vector<T> data;
    void allocMem();

public:
    void initialize(vec3<int> n);
	void initialize(int n1d, vec3<int> n);
    Array3d();
    Array3d(const Array3d& arr);
    Array3d(vec3<int> n);
    ~Array3d() { clear(); }

    friend int operator==(const Array3d<T>& arr1, const Array3d<T>& arr2) {
        if (arr1.n.x != arr2.n.x || arr1.n.y != arr2.n.y || arr1.n.z != arr2.n.z) return 0;
        for (int i = 0; i < arr1.n.x; i++)
            for (int j = 0; j < arr1.n.y; j++)
                for (int k = 0; k < arr1.n.z; k++)
                    if (arr1.data[arr1.getIndex(i, j, k)] != arr2.data[arr2.getIndex(i, j, k)]) return 0;
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

    int getIndex(int i, int j, int k) const;

    Array3d& operator=(const Array3d& arr);

    T* getArray1d();

    vec3<int> size() const { return n; };
    int size1d() const { return n1d; };

    void clear();
    void setToZeros();
};

template<class T>
inline void Array3d<T>::allocMem()
{
    data.resize(n1d);
}

template<class T>
inline void Array3d<T>::initialize(vec3<int> _n) {
    clear();
    n = _n;
	n1d = n.x * n.y * n.z;
    allocMem();
    setToZeros();
}

template<class T>
inline void Array3d<T>::initialize(int _n1d, vec3<int> _n) {
	clear();
	n = _n;
	n1d = _n1d;
	allocMem();
	setToZeros();
}

template<class T>
inline Array3d<T>::Array3d() {
}

template<class T>
inline Array3d<T>::Array3d(const Array3d & arr) {
    n = arr.n;
	n1d = arr.n1d;

    allocMem();

    for (int i = 0; i < n.x; i++)
        for (int j = 0; j < n.y; j++)
            for (int k = 0; k < n.z; k++)
                (*this)(i, j, k) = arr.data[getIndex(i, j, k)];
}

template<class T>
inline Array3d<T>::Array3d(vec3<int> _n) {
    initialize(_n);
}

template<class T>
inline T & Array3d<T>::operator()(int i, int j, int k) {
    return data[getIndex(i, j, k)];
}

template<class T>
inline T & Array3d<T>::operator()(vec3<int> index) {
    return data[getIndex(index.x, index.y, index.z)];
}

template<class T>
inline T & Array3d<T>::get(int i, int j, int k) {
    return data[getIndex(i, j, k)];
}

template<class T>
inline T & Array3d<T>::operator[](int index) {
    return data[index];
}

template<class T>
inline T & Array3d<T>::operator()(int index) {
    return data[index];
}

template<class T>
inline T Array3d<T>::operator()(int i, int j, int k) const {
    return data[getIndex(i, j, k)];
}

template<class T>
inline T Array3d<T>::operator()(vec3<int> index) const {
    return data[getIndex(index.x, index.y, index.z)];
}

template<class T>
inline T Array3d<T>::get(int i, int j, int k) const {
    return data[getIndex(i, j, k)];
}

template<class T>
inline T Array3d<T>::operator[](int index) const {
    return data[index];
}

template<class T>
inline T Array3d<T>::operator()(int index) const {
    return data[index];
}

template<class T>
inline int Array3d<T>::getIndex(int i, int j, int k) const
{
    return (i*n.y + j)*n.z + k;
}

template<class T>
inline Array3d<T> & Array3d<T>::operator=(const Array3d & arr) {
    initialize(arr.n);

    for (int i = 0; i < n.x; i++)
        for (int j = 0; j < n.y; j++)
            for (int k = 0; k < n.z; k++)
                (*this)(i, j, k) = arr.data[getIndex(i, j, k)];

    return *this;
}

template<class T>
inline T * Array3d<T>::getArray1d() {
    return data;
}

template<class T>
inline void Array3d<T>::clear() {
    data.resize(0);
}

template<class T>
inline void Array3d<T>::setToZeros() {
    for (int i = 0; i < n1d; i++)
        data[i] = 0;
}
