#ifndef NUMERIC_H
#define NUMERIC_H

#include "iterator.h"
#include "utility.h"


namespace
{


template<typename ForwardIterator, typename T>
constexpr
void
iota(ForwardIterator first, ForwardIterator last, T value) {
    for (; first != last; ++first) {
          *first = value;
          ++value;
    }
}

template <typename InputIterator, typename T>
constexpr
inline T accumulate(InputIterator first, InputIterator last, T init) {
    while(first != last) {
        init = MYSTL::move(init) + *first++;
    }
    return init;
}


template<typename InputIterator, typename T, typename BinaryOperation>
constexpr
inline T accumulate(InputIterator first, InputIterator last, T init,
                    BinaryOperation binary_op) {
    for (; first != last; ++first)
        init = binary_op(init, *first);
      return init;
}
 
template <typename InputIterator, typename OutputIterator>
constexpr
OutputIterator
partial_sum(InputIterator first, InputIterator last, OutputIterator dest_first) {
    if(first == last)  return dest_first;

    typename MYSTL::iterator_traits<InputIterator>::value_type sum = *first;
    *dest_first = sum;

    while(++first != last) {
        sum = MYSTL::move(sum) + *first;
        *++dest_first = sum;
    }
    return ++dest_first;
}


template<typename InputIterator, typename OutputIterator, typename BinaryOperation>
constexpr 
OutputIterator 
partial_sum(InputIterator first, InputIterator last, 
                     OutputIterator dest_first, BinaryOperation op)
{
    if (first == last) return dest_first;
 
    typename MYSTL::iterator_traits<InputIterator>::value_type sum = *first;
    *dest_first = sum;
 
    while (++first != last) {
       sum = op(MYSTL::move(sum), *first); 
       *++dest_first = sum;
    }
    return ++dest_first;
}

template <typename InputIterator1, typename InputIterator2, typename T>
constexpr
inline T
inner_product(InputIterator1 first1, InputIterator1 last1, 
              InputIterator2 first2, T init) {
    for (; first1 != last1; ++first1, ++first2)
        init = MYSTL::move(init) + (*first1 * *first2);
    return init;
}

template <typename InputIterator1, typename InputIterator2, typename T,
          typename BinaryOeration1, typename BinaryOeration2>
constexpr
inline T
inner_product(InputIterator1 first1, InputIterator1 last1, 
              InputIterator2 first2, T init, 
              BinaryOeration1 op1, BinaryOeration2 op2) {
    for (; first1 != last1; ++first1, ++first2)
        init = op1(MYSTL::move(init), op2(*first1, *first2));
    return init;
}

/*
*(dest_first)   = *first;
*(dest_first+1) = *(first+1) - *(first);
*(dest_first+2) = *(first+2) - *(first+1);
*(dest_first+3) = *(first+3) - *(first+2);
*/

template<typename InputIterator, typename OutputIterator>
constexpr 
OutputIterator 
adjacent_difference(InputIterator first, InputIterator last, 
                                    OutputIterator dest_first)
{
    if (first == last)    return dest_first;

    using value_t = typename MYSTL::iterator_traits<InputIterator>::value_type;
    value_t acc = *first;
    *dest_first = acc;
    while (++first != last) {
        value_t val = *first;
        *++dest_first = val - MYSTL::move(acc); 
        acc = MYSTL::move(val);
    }
    return ++dest_first;
}

template<typename InputIterator, typename OutputIterator, typename BinaryOperation>
constexpr 
OutputIterator 
adjacent_difference(InputIterator first, InputIterator last, 
                    OutputIterator dest_first, BinaryOperation op)
{
    if (first == last) return dest_first;
 
    using value_t = typename MYSTL::iterator_traits<InputIterator>::value_type;
    value_t acc = *first;
    *dest_first = acc;
    while (++first != last) {
        value_t val = *first;
        *++dest_first = op(val, MYSTL::move(acc)); 
        acc = MYSTL::move(val);
    }
    return ++dest_first;
}








} // namespace 

#endif