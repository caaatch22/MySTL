#ifndef MEMORY_H
#define MEMORY_H

#include <cstddef>
#include <cstdlib>
#include <climits>

#include "allocator.h"
#include "construct.h"
#include "uninitialized.h"

namespace MYSTL
{

template <class T>
constexpr T* addressof(T& value) noexcept {
    return &value;
}



//my smart pointer

// unique_ptr, without Deleter
template <typename T>
class unique_ptr 
{
    
private:
    T* ptr;

public:
    explicit 
    unique_ptr(T* ptr = nullptr) noexcept 
        :ptr(ptr) {}

    unique_ptr(const unique_ptr&) = delete;
    unique_ptr& opeartor=(const unique_ptr&) = delete;

    unique_ptr(unique_ptr&& rhs) noexcept
        : ptr(MYSTL::move(rhs))  { rhs.ptr = nullptr; }
    unique_ptr& operator=(unique_ptr&& rhs) noexcept {
        reset(rhs.release());
        return *this;
    }

    ~unique_ptr() { delete ptr; }


    T *operator->() const noexcept { return ptr; }
    T &operator*() const noexcept { return *ptr; }
    T *get() const noexcept { return ptr; }
    explicit operator bool() const { return ptr; }

    void swap(unique_ptr& rhs) noexcept {
        using std::swap;
        swap(ptr, rhs.ptr);
    }

    void reset(T* p = nullptr) noexcept {
        const T* old_ptr = ptr;
        ptr = p;
        if(old_ptr)
            delete old_ptr;
    }

    T* release() {
        T *result = nullptr;
        std::swap(result, ptr);
        return result;
    }
};

template <typename T>
void swap(unique_ptr<T>& lhs, unique_ptr<T>& rhs) noexcept { lhs.swap(rhs); }










} //namespace MYSTL


#endif