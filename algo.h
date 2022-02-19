#ifndef ALGO_H
#define ALGO_H

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




} // namespace MYSTL


#endif