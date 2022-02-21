#ifndef ITERATOR_H
#define ITERATOR_H

#include <cstddef>

namespace MYSTL
{

struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag : public input_iterator_tag {};
struct bidirectional_iterator_tag : public forward_iterator_tag {};
struct random_access_iterator_tag : public bidirectional_iterator_tag {};


template <typename Category, typename T, typename Distance = ptrdiff_t,
          typename Pointer = T*, typename Reference = T&>
struct iterator {
    using value_type            = T;
    using difference_type       = Distance;
    using pointer               = Pointer;
    using reference             = Reference;
    using interator_category    = Category;
};

template <typename Iterator>
struct iterator_traits {
    using difference_type   = typename Iterator::difference_type;
    using value_type        = typename Iterator::value_type;
    using pointer           = typename Iterator::pointer;
    using reference         = typename Iterator::reference;
    using iterator_category = typename Iterator::iterator_category;
};

//对原生指针的偏特化版本
template <typename T>
struct iterator_traits<T*> {
    using difference_type   = ptrdiff_t;
    using value_type        = T;
    using pointer           = T*;
    using reference         = T&;
    using iterator_category = random_access_iterator_tag;
};

template <typename T>
struct iterator_traits<const T*> {
    using difference_type   = ptrdiff_t;
    using value_type        = T;
    using pointer           = const T*;
    using reference         = const T&;
    using iterator_category = random_access_iterator_tag;
};

//function to trait the category
template <typename Iterator>
typename iterator_traits<Iterator>::iterator_category
iterator_category(const Iterator&) {
    using category = typename iterator_traits<Iterator>::iterator_category;
    return category{};
}


//traits the difference_type
template <typename Iterator>
inline typename iterator_traits<Iterator>::difference_type*
distance_type(const Iterator&) {
    return static_cast<typename iterator_traits<Iterator>::difference_type*>(0);
}

//traits the value_type
template <typename Iterator>
inline typename iterator_traits<Iterator>::value_type*
value_type(const Iterator&) {
    return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
}

//distances:
template <typename InputIterator>
inline typename iterator_traits<InputIterator>::difference_type
_distance(InputIterator first, InputIterator last,
          input_iterator_tag) {
    typename iterator_traits<InputIterator>::difference_type n = 0;
    while(first != last) { ++first;  ++n; }
    return n;
}

template <typename RandomAccessIterator>
inline typename iterator_traits<RandomAccessIterator>::difference_type 
_distance(RandomAccessIterator first, RandomAccessIterator last, 
          random_access_iterator_tag) {
  return last - first;
}

template <typename InputIterator>
inline typename iterator_traits<InputIterator>::difference_type
distance(InputIterator first, InputIterator last) {
    return _distance(first, last, iterator_category(first));
}


//advances:
template <typename InputIterator, typename Distance>
inline void _advance(InputIterator& i, Distance n, input_iterator_tag) {
    while (n--)  ++i;
}

template <typename BidirectionIterator, typename Distance>
inline void _advance(BidirectionIterator& i, Distance n, 
                      bidirectional_iterator_tag) {
    if(n >= 0) 
        while(n --)  ++i;
    else 
        while(n ++)  --i;
}

template <typename RandomAccessIterator, typename Distance>
inline void _advance(RandomAccessIterator& i, Distance n, 
                      random_access_iterator_tag) {
    i += n;
}

template <typename InputIterator, typename Distance>
inline void advance(InputIterator& i, Distance n) {
    _advance(i, n, iterator_catrgory(i));
}



//reverse_iterator
template <typename Iterator>
class reverse_iterator
{
private:
  Iterator current;  // 记录对应的正向迭代器

public:
  // 反向迭代器的五种相应型别

    using value_type            = typename iterator_traits<Iterator>::value_type ;
    using difference_type       = typename iterator_traits<Iterator>::difference_type;
    using pointer               = typename iterator_traits<Iterator>::pointer;
    using reference             = typename iterator_traits<Iterator>::reference;
    using interator_category    = typename iterator_traits<Iterator>::iterator_category;

    using iterator_type         = Iterator;
    using self                  = reverse_iterator<Iterator>;

public:
    // ctors
    reverse_iterator() {};
    explicit reverse_iterator(iterator_type i) : current(i) {}
    reverse_iterator(const self &rhs) : current(rhs.current) {}

    reverse_iterator& operator=(const reverse_iterator&) = default;

public:

    iterator_type base() const
    { return current; }

  // overloads

    
    reference operator*() const 
    {  auto tmp = current;  return *--tmp; }

    pointer operator->() const 
    {  return &(operator*()); }

    self& operator++() 
    {  --current;  return *this; }
    self operator++(int)
    {  self tmp = *this;  --current; return tmp; }

    self &operator--()
    {  ++current;  return *this; }
    self operator--(int)
    {  self tmp = *this; ++current;  return tmp; }

    self &operator+=(difference_type n) 
    {  current -= n;  return *this; }
    self operator+(difference_type n) const 
    {   return self(current - n); }

    self &operator-=(difference_type n)
    {  current += n;  return *this;}
    self operator-(difference_type n) const 
    {  return self(current + n);   }

    reference operator[](difference_type n) const
    {  return *(*this + n); }
};


// overloads
template <typename Iterator>
inline typename reverse_iterator<Iterator>::difference_type
operator-(const reverse_iterator<Iterator>& lhs,
          const reverse_iterator<Iterator>& rhs) 
{
    return rhs.base() - lhs.base();
}

// 重载比较操作符
template <typename Iterator>
inline bool 
operator==(const reverse_iterator<Iterator>& lhs,
           const reverse_iterator<Iterator>& rhs)
{
    return lhs.base() == rhs.base();
}

template <typename Iterator>
bool operator<(const reverse_iterator<Iterator>& lhs,
               const reverse_iterator<Iterator>& rhs)
{
  return rhs.base() < lhs.base();
}

template <typename Iterator>
inline bool 
operator!=(const reverse_iterator<Iterator>& lhs,
           const reverse_iterator<Iterator>& rhs)
{
    return !(lhs == rhs);
}

template <typename Iterator>
inline bool 
operator>(const reverse_iterator<Iterator>& lhs,
          const reverse_iterator<Iterator>& rhs)
{
    return rhs < lhs;
}

template <typename Iterator>
inline bool 
operator<=(const reverse_iterator<Iterator>& lhs,
           const reverse_iterator<Iterator>& rhs)
{
    return !(rhs < lhs);
}

template <typename Iterator>
inline bool 
operator>=(const reverse_iterator<Iterator>& lhs,
           const reverse_iterator<Iterator>& rhs)
{
    return !(lhs < rhs);
}


} //end of namespace MYSTL



#endif