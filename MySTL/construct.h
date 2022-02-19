#ifndef CONSTRUCT_H_
#define CONSTRUCT_H_


#include <new>
#include "type_traits.h"
#include "iterator.h"

namespace MYSTL
{


//construct
template <typename Tp>
inline void construct(Tp* ptr) {
    ::new (static_cast<void*>(ptr)) Tp();
}


template <class Ty, class... Args>
inline void construct(Ty* ptr, Args&&... args) {
    ::new (static_cast<void*>(ptr)) Ty(MYSTL::forward<Args>(args)...);
}


//destroy
template <typename Tp>
inline void __destroy_aux(Tp* ptr, std::false_type) {
    if(ptr != nullptr)
        ptr->~Tp();
}

template <typename Tp>
inline void __destroy_aux(Tp* ptr, std::true_type) {}

template <typename ForwardIterator>
inline void __destroy_n_aux(ForwardIterator first, ForwardIterator last, std::true_type) {}


template <typename ForwardIterator>
inline void __destroy_n_aux(ForwardIterator first, ForwardIterator last, std::false_type) {
    for (; first != last; ++first)
        destroy(&*first);
}


template <typename Tp>
inline void destroy(Tp* ptr) {
    __destroy_aux(ptr, std::is_trivially_destructible<Tp>{});
}

template <typename T, typename ForwardIterator>
inline void destroy(ForwardIterator first, ForwardIterator last) {
    using value_type = typename iterator_traits<ForwardIterator>::value_type;
    __destroy_n_aux(first, last, std::is_trivially_destructible<value_type>{});
} 



} //end of namespace MYSTL


#endif