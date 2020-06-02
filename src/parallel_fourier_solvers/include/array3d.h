#pragma once
#include <vector>
#include <omp.h>
#include <type_traits>
#include "vector3d.h"

template <class Data>
class NUMA_Allocator {
public:

    using value_type = Data;
    using propagate_on_container_move_assignment = std::bool_constant<true>;
    using is_always_equal = std::bool_constant<true>;

    constexpr NUMA_Allocator() noexcept {}
    constexpr NUMA_Allocator(const NUMA_Allocator&) noexcept = default;
    template<class OtherData>
    constexpr NUMA_Allocator(const NUMA_Allocator<OtherData>&) noexcept {}

    value_type * allocate(const size_t num)
    {
        const size_t size_vt = sizeof(value_type);
        const size_t len = num * size_vt;
        const size_t num_threads = omp_get_max_threads();
        const size_t block_size = (num + num_threads - 1) / num_threads * size_vt;
        const size_t block_size_rem = len - block_size * (num_threads - 1);
        char * p = reinterpret_cast<char*>(std::malloc(len));
#pragma omp parallel for
        for (int thr = 0; thr < num_threads; thr++) {
            const size_t cur_block_size = thr == num_threads - 1 ? block_size_rem : block_size;
            std::memset(p + thr * block_size, 0, cur_block_size);
        }
        return reinterpret_cast<value_type*>(p);
    }

    void deallocate(value_type * const p, const size_t num)
    {
        std::free(p);
    }

    friend int operator==(const NUMA_Allocator& a1, const NUMA_Allocator& a2) {
        return true;
    }

    friend int operator!=(const NUMA_Allocator& a1, const NUMA_Allocator& a2) {
        return false;
    }

};

template <class T>
class Array3d {
    vec3<int> n;
    std::vector<T, NUMA_Allocator<T>> data;
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
    int size1d() const { return n.x * n.y * n.z; };

    void clear();
    void setToZeros();
};

template<class T>
inline void Array3d<T>::allocMem()
{
    data.resize(n.x*n.y*n.z);
}

template<class T>
inline void Array3d<T>::initialize(vec3<int> _n) {
    clear();
    n = _n;
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
    for (int i = 0; i < n.x*n.y*n.z; i++)
        data[i] = 0;
}
