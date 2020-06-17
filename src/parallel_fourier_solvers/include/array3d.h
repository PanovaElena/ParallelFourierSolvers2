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
        if (num_threads > num) {
            value_type* p = reinterpret_cast<value_type*>(std::malloc(len));
            std::memset(p, 0, len);
            return p;
        }
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
public:
    vec3<int> n;
	size_t alloc;
    std::vector<T, NUMA_Allocator<T>> data;
    T* row = 0;
    bool ifStorage;

    void allocMem();

    Array3d() {}
    Array3d(const Array3d& arr) {
        if (arr.ifStorage) createCopy(arr);
        else createShallowCopy(arr);
    }
    Array3d(vec3<int> n) {
        create(n);
    }
    Array3d(vec3<int> n, int alloc) {
        create(n, alloc);
    }
    ~Array3d() {
        clear();
    }

    void create(vec3<int> n);
    void create(vec3<int> n, int alloc);

    void createCopy(const Array3d& arr);
    void createShallowCopy(const Array3d& arr);
    template <class U>
    void createShallowCopy(Array3d<U>& arr, vec3<int> n);

    friend bool operator==(const Array3d<T>& arr1, const Array3d<T>& arr2) {
        if (arr1.n.x != arr2.n.x || arr1.n.y != arr2.n.y || arr1.n.z != arr2.n.z)
            return false;
        for (int i = 0; i < arr1.n.x; i++)
            for (int j = 0; j < arr1.n.y; j++)
                for (int k = 0; k < arr1.n.z; k++)
                    if (arr1.data[arr1.getIndex(i, j, k)] != arr2.data[arr2.getIndex(i, j, k)])
                        return false;
        return true;
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

    size_t getIndex(int i, int j, int k) const;

    Array3d& operator=(const Array3d& arr);

    T* getArray1d();

    vec3<int> size() const { return n; };
    size_t size1d() const { return alloc; };

    void clear();
};

template<class T>
inline void Array3d<T>::allocMem()
{
    data.resize(alloc);
}

template<class T>
inline void Array3d<T>::create(vec3<int> n) {
    create(n, n.x * n.y * n.z);
}

template<class T>
inline void Array3d<T>::create(vec3<int> n, int alloc) {
	clear();
	this->n = n;
	this->alloc = alloc;
    this->ifStorage = true;
	allocMem();
    row = data.data();
}

template<class T>
inline void Array3d<T>::createCopy(const Array3d & arr) {
    create(arr.n, arr.alloc);
    for (int i = 0; i < n.x; i++)
        for (int j = 0; j < n.y; j++)
            for (int k = 0; k < n.z; k++)
                (*this)(i, j, k) = arr(i, j, k);
}

template<class T>
inline void Array3d<T>::createShallowCopy(const Array3d<T>& arr) {
    clear();
    this->n = arr.n;
    this->alloc = arr.alloc;
    this->ifStorage = false;
    row = arr.row;
}

template<class T>
template<class U>
inline void Array3d<T>::createShallowCopy(Array3d<U>& arr, vec3<int> n) {
    clear();
    this->n = n;
    this->alloc = arr.alloc;
    this->ifStorage = false;
    row = reinterpret_cast<T*>(arr.row);
}

template<class T>
__forceinline T & Array3d<T>::operator()(int i, int j, int k) {
    return row[getIndex(i, j, k)];
}

template<class T>
__forceinline T & Array3d<T>::operator()(vec3<int> index) {
    return row[getIndex(index.x, index.y, index.z)];
}

template<class T>
__forceinline T & Array3d<T>::get(int i, int j, int k) {
    return row[getIndex(i, j, k)];
}

template<class T>
__forceinline T & Array3d<T>::operator[](int index) {
    return row[index];
}

template<class T>
__forceinline T & Array3d<T>::operator()(int index) {
    return row[index];
}

template<class T>
__forceinline T Array3d<T>::operator()(int i, int j, int k) const {
    return row[getIndex(i, j, k)];
}

template<class T>
__forceinline T Array3d<T>::operator()(vec3<int> index) const {
    return row[getIndex(index.x, index.y, index.z)];
}

template<class T>
__forceinline T Array3d<T>::get(int i, int j, int k) const {
    return row[getIndex(i, j, k)];
}

template<class T>
__forceinline T Array3d<T>::operator[](int index) const {
    return row[index];
}

template<class T>
__forceinline T Array3d<T>::operator()(int index) const {
    return row[index];
}

template<class T>
__forceinline size_t Array3d<T>::getIndex(int i, int j, int k) const
{
    return (i*n.y + j)*n.z + k;
}

template<class T>
inline Array3d<T> & Array3d<T>::operator=(const Array3d & arr) {
    if (this != &arr) {
        if (arr.ifStorage) createCopy(arr);
        else createShallowCopy(arr);
    }
    return *this;
}

template<class T>
__forceinline T * Array3d<T>::getArray1d() {
    return row;
}

template<class T>
inline void Array3d<T>::clear() {
    data.clear();
    row = 0;
}
