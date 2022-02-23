/*
#ifndef ALLOCATOR_H_
#define ALLOCATOR_H_

#include <new>
#include <cstddef>
#include <climits>
#include <iostream>

namespace MYSTL
{


template <typename T>
inline T* _allocate(ptrdiff_t size, T*) {
    set_new_hander(0);
    pointer tmp = (pointer )(::operator new((size_t)(size * sizeof(T))));
    if(tmp == 0) {
        std::cerr << "out of memory" << std::endl;
        exit(1);
    }
    return tmp;
}

template <typename T>
inline void _deallocate(T* buffer) {
    ::operator delete(buffer);
}

template <typename T1, typename T2>
inline void _constuct(T1* p, const T2& value) {
    new (p) T1(value);
}

template <typename T>
inline void _destroy(T* ptr) {
    ptr->~T();
}

template <typename T>
typename allocator {

public:
    using value_type        = T;
    using pointer           = T*;
    using const_pointer     = const T*;
    using reference         = T&;
    using const_reference   = const T&;
    using size_type         = size_t;
    using difference_type   = ptrdiff_t;

    template <typename U>
    struct rebind {
        typedef allocator<U> other;
    };

    pointer allocate(size_type n, const void* hint = 0) {
        return _allocate((difference_type)n, (pointer)0);
    }

    void deallocate(pointer p, size_type n) { _deallocate(p); }

    void construct(pointer p, const T& value) {
        _construct(p, value);
    }

    void destroy(pointer p) { _destroy(p); }

    pointer address(reference x) { return (pointer)&x; }

    const_pointer const_adress(const_reference x) {
        return (const_pointer)&x;
    }

    size_type max_size() const {
        return size_type(UINT_MAX / sizeof(T));
    }
};

}

#endif
*/

#ifndef ALLOCATOR_H_
#define ALLOCATOR_H_

#include <new>
#include <climits>
#include "construct.h"
#include "utility.h"

namespace MYSTL
{


template <typename T>
class allocator
{
public:

    using value_type        = T;
    using pointer           = T*;
    using const_pointer     = const T*;
    using reference         = T&;
    using const_reference   = const T&;
    using size_type         = size_t;
    using difference_type   = ptrdiff_t;
    using iterator          = T*;
    using const_iterator    = const T*;
    template <typename U>
    struct rebind{
        using other = allocator<U>; 
    };

public:

    //pointer address(reference x) { return &x; }
    //const_pointer address(const_reference x) { return &x; }
    
    static pointer allocate(size_type n, const void* = static_cast<const void*>(0)) {
        return static_cast<T*>(::operator new(n * sizeof(T)));
    }

    static void deallocate(T* ptr, size_type /*size*/) {
        if (ptr == nullptr)  return;
        ::operator delete(ptr);
    }

    // static size_type max_size() const {
    //     return size_type(UINT_MAX / sizeof(T));
    // }

    static void construct(pointer ptr) {
        MYSTL::construct(ptr);
    }
    template <typename... Args>
    static void construct(pointer ptr, Args&&... args) {
        MYSTL::construct(ptr, MYSTL::forward<Args>(args)...);
    }

    static void destroy(pointer ptr) {
        MYSTL::destroy(ptr);
    }

    static void destroy(pointer first, pointer last) {
        MYSTL::destroy(first, last);
    }
    //void destroy(pointer first, pointer last);
};

/*
template <typename T>
void allocator<T>::construct(T* ptr) {
    MYSTL::construct(ptr);
}

template <typename T>
void allocator<T>::construct(T* ptr, const T& value) {
    MYSTL::construct(ptr, value);
}

template <typename T>
void allocator<T>::construct(T* ptr, T&& value) {
    MYSTL::construct(ptr, MYSTL::move(value));
}


template <typename T>
void allocator<T>::destroy(T* first, T* last) {
    MYSTL::destroy(first, last);
}
*/


} // namespace MYSTL



#endif // !MYTINYSTL_ALLOCATOR_H_

