#ifndef UNINTIALIZED_H
#define UNINITALIZED_H

#include "algobase.h"
#include "construct.h"
#include "iterator.h"
#include "type_traits.h"
#include "utility.h"


namespace MYSTL
{



template <typename InputIterator, typename ForwardItererator>
inline ForwardItererator 
__uninitialized_copy_aux(InputIterator first, InputIterator last, 
                          ForwardItererator dest, std::true_type) {
  return MYSTL::copy(first, last, dest);
}

template <typename InputIterator, typename ForwardItererator>
inline ForwardItererator
__uninitialized_copy_aux(InputIterator first, InputIterator last, 
                          ForwardItererator dest, std::false_type) {
    auto cur = dest;
    try {
        for (; first != last; ++first, ++cur) {
            MYSTL::construct(&*cur, *first);
        }
    }
    catch (...) {
        for (; dest != cur; --cur)
            MYSTL::destroy(&*cur);
    }
    return cur;
}


template <typename InputIterator, typename ForwardItererator>
inline ForwardItererator
uninitialized_copy(InputIterator first, InputIterator last, ForwardItererator dest) {
    using value_type = typename iterator_traits<ForwardItererator>::value_type;
    using trivially_copy_assignable = std::is_trivially_copy_assignable<value_type>;
    return MYSTL::__uninitialized_copy_aux(first, last, dest,
                                           trivially_copy_assignable{});
}

// uninitialized_copy_n

template <typename InputIterator, typename Size, typename ForwardItererator>
inline ForwardItererator 
__uninitialized_copy_aux_n(InputIterator first, Size n, 
                            ForwardItererator result, std::true_type) {
    return MYSTL::copy_n(first, n, result).second;
}

template <typename InputIterator, typename Size, typename ForwardItererator>
inline ForwardItererator
__uninitialized_copy_aux_n(InputIterator first, Size n, 
                            ForwardItererator result, std::false_type) {
    auto cur = result;
    try {
        for (; n > 0; --n, ++cur, ++first) {
            MYSTL::construct(&*cur, *first);
        }
    }
    catch (...) {
        for (; result != cur; --cur)
            MYSTL::destroy(&*cur);
    }
    return cur;
}

template <typename InputIterator, typename Size, typename ForwardItererator>
inline ForwardItererator 
uninitialized_copy_n(InputIterator first, Size n, ForwardItererator result) {
    using value_type = typename iterator_traits<InputIterator>::value_type;
    using trivially_copy_assignable = std::is_trivially_copy_assignable<value_type>;
    return MYSTL::__uninitialized_copy_aux_n(first, n, result,
                                             trivially_copy_assignable{});
}


// uninitialized_fill
template <typename ForwardItererator, typename T>
inline void 
__uninitialized_fill_aux(ForwardItererator first, ForwardItererator last, 
                         const T& value, std::true_type) {
    MYSTL::fill(first, last, value);
}

template <typename ForwardItererator, typename T>
inline void 
__uninitialized_fill_aux(ForwardItererator first, ForwardItererator last, 
                         const T& value, std::false_type) {
    auto cur = first;
    try {
        for (; cur != last; ++cur) {
            MYSTL::construct(&*cur, value);
        }
    }
    catch (...) {
        for (; first != cur; ++first)
            MYSTL::destroy(&*first);
    }
}

template <typename ForwardItererator, typename T>
inline void  
uninitialized_fill(ForwardItererator first, ForwardItererator last, const T& value) {
    using value_type = typename iterator_traits<ForwardItererator>::value_type;
    using trivially_copy_assignable = typename std::is_trivially_copy_assignable<value_type>;
    MYSTL::__uninitialized_fill_aux(first, last, value,
                                    trivially_copy_assignable{});
}


// uninitialized_fill_n
template <typename ForwardItererator, typename Size, typename T>
inline ForwardItererator 
__uninitialized_fill_aux_n(ForwardItererator first, Size n, const T& value, std::true_type) {
    return MYSTL::fill_n(first, n, value);
}

template <typename ForwardItererator, typename Size, typename T>
inline ForwardItererator 
__uninitialized_fill_aux_n(ForwardItererator first, Size n, const T& value, std::false_type) {
    auto cur = first;
    try {
        for (; n > 0; --n, ++cur) {
            MYSTL::construct(&*cur, value);
        }
    }
    catch (...) {
        for (; first != cur; ++first)
            MYSTL::destroy(&*first);
    }
    return cur;
}

template <typename ForwardItererator, typename Size, typename T>
inline ForwardItererator
uninitialized_fill_n(ForwardItererator first, Size n, const T& value) {
    using value_type = typename iterator_traits<ForwardItererator>::value_type;
    using trivially_copy_assignable = typename std::is_trivially_copy_assignable<value_type>;
    return MYSTL::__uninitialized_fill_aux_n(first, n, value,
                                             trivially_copy_assignable{});
}



/*****************************************************************************************/
// uninitialized_move
// 把[first, last)上的内容移动到以 result 为起始处的空间，返回移动结束的位置
/*****************************************************************************************/
template <typename InputIterator, typename ForwardItererator>
inline ForwardItererator 
__uninitialized_move_aux(InputIterator first, InputIterator last, ForwardItererator result, std::true_type) {
    return MYSTL::move(first, last, result);
}

template <typename InputIterator, typename ForwardItererator>
inline ForwardItererator 
__uninitialized_move_aux(InputIterator first, InputIterator last, ForwardItererator result, std::false_type) {
    auto cur = result;
    try {
        for (; first != last; ++first, ++cur) {
            MYSTL::construct(&*cur, MYSTL::move(*first));
        }
    }
    catch (...) {
        MYSTL::destroy(result, cur);
    }
    return cur;
}

template <typename InputIterator, typename ForwardItererator>
inline ForwardItererator 
uninitialized_move(InputIterator first, InputIterator last, ForwardItererator result) {
    using value_type = typename iterator_traits<InputIterator>::value_type;
    using trivially_move_assignable = typename std::is_trivially_move_assignable<value_type>;
    return MYSTL::__uninitialized_move_aux(first, last, result,
                                        trivially_move_assignable{});
}


// uninitialized_move_n
template <typename InputIterator, typename Size, typename ForwardItererator>
inline ForwardItererator 
__uninitialized_move_aux_n(InputIterator first, Size n, ForwardItererator result, std::true_type) {
    return MYSTL::move(first, first + n, result);
}

template <typename InputIterator, typename Size, typename ForwardItererator>
inline ForwardItererator
__uninitialized_move_aux_n(InputIterator first, Size n, ForwardItererator result, std::false_type) {
    auto cur = result;
    try {
        for (; n > 0; --n, ++first, ++cur) {
            MYSTL::construct(&*cur, MYSTL::move(*first));
        }
    }
    catch (...) {
        for (; result != cur; ++result)
            MYSTL::destroy(&*result);
        throw;
    }
    return cur;
}

template <typename InputIterator, typename Size, typename ForwardItererator>
inline ForwardItererator 
uninitialized_move_n(InputIterator first, Size n, ForwardItererator result) {
    using value_type = typename iterator_traits<InputIterator>::value_type;
    using trivially_move_assignable = typename std::is_trivially_move_assignable<value_type>;
    return MYSTL::__uninitialized_move_aux_n(first, n, result,
                                             trivially_move_assignable{});
}


} // end of namespace MYSTL


#endif