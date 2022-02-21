#ifndef ALGOBASE_H
#define ALGOBASE_H

#include <cstring>

#include "iterator.h"
#include "utility.h"

namespace MYSTL
{

#ifdef max
#pragma message("#undefing marco max")
#undef max
#endif // max

#ifdef min
#pragma message("#undefing marco min")
#undef min
#endif // min

// max
template <typename T>
inline const T& max(const T& lhs, const T& rhs) {
    return lhs < rhs ? rhs : lhs;
}

template <typename T, typename Compare> 
inline const T& max(const T& lhs, const T& rhs, Compare comp) {
    return comp(lhs, rhs) ? rhs : lhs;
}   

// min
template <typename T>
inline const T& min(const T& lhs, const T& rhs) {
    return rhs < lhs ? rhs : lhs;
}


template <typename T, typename Compare>
inline const T& min(const T& lhs, const T& rhs, Compare comp) {
    return comp(rhs, lhs) ? rhs : lhs;
}

template <typename ForwardIterator1, typename ForwardIterator2>
void iter_swap(ForwardIterator1 lhs, ForwardIterator2 rhs) {
    MYSTL::swap(*lhs, *rhs);
}

//copy
//input_iterator_tag -version
template <typename InputIterator, typename OutputIterator>
inline OutputIterator 
__copy (InputIterator first, InputIterator last, OutputIterator dest, 
        MYSTL::input_iterator_tag) {
    for (; first != last; ++first, ++dest) {
        *dest = *first;
    }
    return dest;
}

template <typename InputIterator, typename OutputIterator>
inline OutputIterator 
__copy (InputIterator first, InputIterator last, OutputIterator dest, 
        MYSTL::random_access_iterator_tag) {
    for (auto n = last - first; n > 0; --n, ++first, ++dest)  {
        *dest = *first;
    }
    return dest;
}

template <typename InputIterator, typename OutputIterator>
inline OutputIterator 
__copy_d (InputIterator first, InputIterator last, OutputIterator dest) {
    return __copy(first, last, dest, iterator_catrgory(first));
}

// 为 trivially_copy_assignable 类型提供特化版本
template <typename T, typename U>
typename std::enable_if<
  std::is_same<typename std::remove_const<T>::type, U>::value &&
  std::is_trivially_copy_assignable<U>::value,
  U*>::type
__copy_d(T* first, T* last, U* result) {
    const auto n = static_cast<size_t>(last - first);
    if (n != 0)
        std::memmove(result, first, n * sizeof(U));
    return result + n;
}

template <typename InputIterator, typename OutputIterator>
inline OutputIterator 
copy(InputIterator first, InputIterator last, OutputIterator dest) {
    return __copy_d(first, last, dest);
}

////////////////////////////////////////////////////////////////////////
// copy_backward
//[first, last) -> [result - (last - first), result)

// bidirectional_iterator_tag version
template <typename BidirectionalIterator1, typename BidirectionalIterator2>
BidirectionalIterator2 
__copy_backward_aux(BidirectionalIterator1 first, BidirectionalIterator1 last,
                    BidirectionalIterator2 result, MYSTL::bidirectional_iterator_tag)
{
    while (first != last)
        *--result = *--last;
    return result;
}

//random_access_iterator_tag version
template <typename BidirectionalIterator1, typename BidirectionalIterator2>
BidirectionalIterator2 
__copy_backward_aux(BidirectionalIterator1 first, BidirectionalIterator1 last,
                    BidirectionalIterator2 result, MYSTL::random_access_iterator_tag)
{
    auto n = last - first;
    while(n -- )
        *--result = *--last;
    return result;
}

template <typename BidirectionalIterator1, typename BidirectionalIterator2>
BidirectionalIterator2 
__copy_backward(BidirectionalIterator1 first, BidirectionalIterator1 last,
                BidirectionalIterator2 result)
{
  return __copy_backward_aux(first, last, result,
                            iterator_category(first));
}

//  trivially_copy_assignable version
template <typename T, typename U>
typename std::enable_if<
    std::is_same<typename std::remove_const<T>::type, U>::value &&
    std::is_trivially_copy_assignable<U>::value,
    U*>::type
__copy_backward(T* first, T* last, U* result)
{
    const auto n = static_cast<size_t>(last - first);
    if (n != 0) {
        result -= n;
        std::memmove(result, first, n * sizeof(U));
    }
    return result;
}

template <typename BidirectionalIterator1, typename BidirectionalIterator2>
BidirectionalIterator2 
copy_backward(BidirectionalIterator1 first, BidirectionalIterator1 last, 
              BidirectionalIterator2 result)
{
    return __copy_backward(first, last, result);
}

////////////////////////////////////////////////////////////////////////
//copy_n


template <typename InputIterator, typename Size, typename OutputIterator>
MYSTL::pair<InputIterator, OutputIterator>
__copy_n(InputIterator first, Size n, OutputIterator result, MYSTL::input_iterator_tag)
{
    for (; n > 0; --n, ++first, ++result) {
        *result = *first;
    }
    return MYSTL::pair<InputIterator, OutputIterator>(first, result);
}

template <typename RandomIterator, typename Size, typename OutputIterator>
MYSTL::pair<RandomIterator, OutputIterator>
__copy_n(RandomIterator first, Size n, OutputIterator result, 
                 MYSTL::random_access_iterator_tag)
{
    auto last = first + n;
    return MYSTL::pair<RandomIterator, OutputIterator>(last, MYSTL::copy(first, last, result));
}

template <typename InputIterator, typename Size, typename OutputIterator>
MYSTL::pair<InputIterator, OutputIterator> 
copy_n(InputIterator first, Size n, OutputIterator result) {
    return __copy_n(first, n, result, iterator_category(first));
}


/////////////////////////////////////////////////////////////////////////

template <typename OutputIterator, typename Size, typename T>
OutputIterator __fill_n(OutputIterator first, Size n, const T& value)
{
    for (; n > 0; --n, ++first) {
        *first = value;
    }
    return first;
}

// 为 one-byte 类型提供特化版本
template <typename T, typename Size, typename U>
typename std::enable_if<
  std::is_integral<T>::value && sizeof(T) == 1 &&
  !std::is_same<T, bool>::value &&
  std::is_integral<U>::value && sizeof(U) == 1,
  T*>::type
__fill_n(T* first, Size n, U value)
{
    if (n > 0) {
        std::memset(first, (unsigned char)value, (size_t)(n));
    }
    return first + n;
}

template <typename OutputIterator, typename Size, typename T>
OutputIterator fill_n(OutputIterator first, Size n, const T& value) {
    return __fill_n(first, n, value);
}

/////////////////////////////////////////////////////////////////////////////
//fill
template <typename ForwardIterator, typename T>
void __fill(ForwardIterator first, ForwardIterator last, const T& value,
              MYSTL::forward_iterator_tag)
{
    for (; first != last; ++first) {
        *first = value;
    }
}

template <typename RandomIterator, typename T>
void __fill(RandomIterator first, RandomIterator last, const T& value,
              MYSTL::random_access_iterator_tag)
{
    fill_n(first, last - first, value);
}

template <typename ForwardIterator, typename T>
void fill(ForwardIterator first, ForwardIterator last, const T& value)
{
    __fill(first, last, value, iterator_category(first));
}


} // end of namespace MYSTL



#endif