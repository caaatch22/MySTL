#ifndef HEAP_H
#define HEAP_H

#include "iterator.h"

namespace MYSTL
{

template<typename RandomAccessIterator, typename Distance,
        typename Tp, typename Compare>
void adjust_heap(RandomAccessIterator first, Distance hole_index,
                  Distance len, Tp value, Compare comp)
{
    const Distance __topIndex = hole_index;
    Distance secondChild = hole_index;
    while (secondChild < (len - 1) / 2)
    {
        secondChild = 2 * (secondChild + 1);
        if (comp(first + secondChild,
                 first + (secondChild - 1)))
            secondChild--;
          *(first + hole_index) = _GLIBCXX_MOVE(*(first + secondChild));
          hole_index = secondChild;
        }
      if ((len & 1) == 0 && secondChild == (len - 2) / 2)
        {
          secondChild = 2 * (secondChild + 1);
          *(first + hole_index) = _GLIBCXX_MOVE(*(first
                                                     + (secondChild - 1)));
          hole_index = secondChild - 1;
        }

      __decltype(__gnu_cxx::__ops::__iter_comp_val(_GLIBCXX_MOVE(comp)))
        __cmp(_GLIBCXX_MOVE(comp));
      std::__push_heap(first, hole_index, __topIndex,
                       _GLIBCXX_MOVE(value), __cmp);
    }

template <typename RandomAccessIterator, typename Compare>
void make_heap(RandomAccessIterator first, RandomAccessIterator last,
                Compare comp)
{
    using value_type = typename iterator_traits<RandomAccessIterator>::value_type;
    using distance_type = typename iterator_traits<RandomAccessIterator>::difference_type;

    if(last - first < 2) return;
    const auto len = last - first;

    for (distance_type hole = len >> 1; hole > 0;) {
        --hole;
        auto val = MYSTL::move(*(first + hole));
        adjust_heap(first, hole, len, MYSTL::move(val), comp);
    }
}

template <typename RandomAccessIterator>
void make_heap(RandomAccessIterator first, RandomAccessIterator last) {
    
}

}

#endif