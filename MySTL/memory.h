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



}


#endif