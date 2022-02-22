#ifndef ALGO_H
#define ALGO_H

#include <ctime>
#include <cstddef>



namespace MYSTL
{

//all_of
template <typename InputIterator, typename UnaryPredicate>
bool all_of(InputIterator first, InputIterator last, 
            UnaryPredicate unary_pred) {
    for (; first != last; ++first) {
        if (!unary_pred(*first))
            return false;
    }
  return true;
}

//any_of
template <typename InputIterator, typename UnaryPredicate>
bool any_of(InputIterator first, InputIterator last,
            UnaryPredicate unary_pred) {
    for (; first != last; ++first) {
        if(unary_pred(*first))
            return true;
    }
    return false;
}

//none_of
template <typename InputIterator, typename UnaryPredicate>
bool none_of(InputIterator first, InputIterator last, 
            UnaryPredicate unary_pred) {
    for (; first != last; ++first) {
        if (unary_pred(*first))
            return false;
    }
    return true
}

template <typename InputIterator,typename T>
size_t count(InputIterator first, InputIterator last, const T& value) {
    size_t result = 0;
    for (; first != last; ++first) {
        if(*first == value)
            ++result;
    }
    return result;
}

template <typename InputIterator, typename UnaryPredicate>
size_t count_if(InputIterator first, InputIterator last, 
                UnaryPredicate unary_pred) {
    size_t result = 0;
    for (; first != last; ++first)
    {
        if (unary_pred(*first))
            ++result;
    }
    return result;
}
template <typename InputIterator, typename T>
InputIterator
find(InputIterator first, InputIterator last, const T& value) {
    while (first != last && *first != value)
        ++first;
    return first;
}


template <typename InputIterator, typename UnaryPredicate>
InputIterator
find_if(InputIterator first, InputIterator last, UnaryPredicate unary_pred) {
    while (first != last && !unary_pred(*first))
        ++first;
    return first;
}

template <typename InputIterator, typename UnaryPredicate>
InputIterator
find_if_not(InputIterator first, InputIterator last, UnaryPredicate unary_pred) {
    while (first != last && unary_pred(*first))
        ++first;
    return first;
}

//search
template <typename ForwardIterator1, typename ForwardIterator2>
ForwardIterator1
search(ForwardIterator1 first1, ForwardIterator1 last1,
       ForwardIterator2 first2, ForwardIterator2 last2)
{
    auto dis1 = MYSTL::distance(first1, last1);
    auto dis2 = MYSTL::distance(first2, last2);
    if (dis1 < dis2)  return last1;
    auto current1 = first1, current2 = first2;

    while (current2 != last2) {
        if (*current1 == *current2) {
            ++current1;
            ++current2;
        }
        else {
            if (dis1 == dis2) {
                return last1;
            }
            else {
                current1 = ++first1;
                current2 = first2;
                --dis1;
            }
        }
    }
    return first1;
}


template <typename ForwardIterator1, typename ForwardIterator2, 
                                     typename Compared>
ForwardIterator1
search(ForwardIterator1 first1, ForwardIterator1 last1,
       ForwardIterator2 first2, ForwardIterator2 last2, Compared comp)
{
    auto dis1 = MYSTL::distance(first1, last1);
    auto dis2 = MYSTL::distance(first2, last2);
    if (dis1 < dis2)   return last1;
    auto current1 = first1;
    auto current2 = first2;
    while (current2 != last2) {
        if (comp(*current1, *current2)) {
            ++current1;
            ++current2;
        }
        else {
            if (dis1 == dis2) {
                return last1; 
            }
            else {
                current1 = ++first1;
                current2 = first2;
                --dis1;
            }
        }
    }
    return first1;
}

} // namespace MYSTL


#endif