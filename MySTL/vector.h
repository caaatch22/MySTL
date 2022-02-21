#ifndef VECTOR_H_
#define VECTOR_H_

#include "memory.h"
#include "utility.h"
#include "iterator.h"
#include "allocator.h"

namespace MYSTL
{


template <typename T, typename Alloc = MYSTL::allocator<T>>
class vector {
public:
    using data_allocator         = MYSTL::allocator<T>;
    using iterator_category      = MYSTL::random_access_iterator_tag;

    using value_type             = T;
    using iterator               = value_type*;
    using const_iterator         = const value_type*;
    using pointer                = value_type*;
    using const_pointer          = const value_type*;
    using reference              = value_type&;
    using const_reference        = const value_type&;
    using size_type              = size_t;
    using difference_type        = ptrdiff_t;

    using reverse_iterator       = MYSTL::reverse_iterator<iterator>;
    using const_reverse_iterator = MYSTL::reverse_iterator<const_iterator>;
    

protected:
    iterator start;
    iterator finish;
    iterator end_of_storage;


protected:

    void allocate(size_type init_size) {
        try {
            start = data_allocator::allocate(init_size);
            end_of_storage = finish = start + init_size;
        }
        catch(...) {
            start = finish = end_of_storage = nullptr;
            throw;
        }
    }
    void deallocate_all() {
        if(start) data_allocator::deallocate(start, end_of_storage - start);
    }

    void fill_initialize(size_type n, const value_type& value);

    template <typename Iterator>
    void range_initialize(Iterator first, Iterator last);

    void insert_aux(iterator position, const T& value);
    template <typename InputIterator>
    void assign_aux(InputIterator first, InputIterator last, MYSTL::input_iterator_tag);
    template <typename ForwardIterator>
    void assign_aux(ForwardIterator first, ForwardIterator last, MYSTL::forward_iterator_tag);

public:
    
    //ctors:
    vector() : start(0), finish(0), end_of_storage(0) {}
    vector(size_type n, const value_type& value) { fill_initialize(n, value); }
    vector(const vector& rhs) { range_initialize(rhs.begin(), rhs.end()); }
    template <typename InputIterator>
    vector(InputIterator first, InputIterator last) { range_initialize(first, last); }
    vector& operator=(const vector& rhs);
    vector(vector&& rhs) noexcept;
    vector& operator=(vector&& rhs) noexcept;

    ~vector() {
        data_allocator::destroy(start, finish);
        deallocate_all();
    }

    //iterators
    iterator        begin()       { return start; }
    const_iterator  begin() const { return start; }
    iterator        end()         { return finish; }
    const_iterator  end()   const { return finish; }

    reverse_iterator       rbegin()        { return reverse_iterator(end()); }
    const_reverse_iterator rbegin()  const { return const_reverse_iterator(end()); }
    reverse_iterator       rend()          { return reverse_iterator(begin()); }
    const_reverse_iterator rend()    const { return const_reverse_iterator(begin()); }

    const_iterator         cbegin()  const { return begin(); }
    const_iterator         cend()    const { return end(); }
    const_reverse_iterator crbegin() const { return rbegin(); }
    const_reverse_iterator crend()   const { return rend(); }

    //capacity
    size_type       size() const
    { return static_cast<size_type>(end() - begin()); }
    size_type       capacity() const 
    {  return static_cast<size_type>(end_of_storage - begin()); }
    bool            empty() const { return begin() == end(); }

    //element access:
    reference       at(size_type n)               { return (*this)[n]; }
    const_reference at(size_type n)         const { return (*this)[n]; }
    reference       front()                       { return *begin(); }
    const_reference front()                 const { return *begin(); }
    reference       back()                        { return *(end() - 1); }
    const_reference back()                  const { return *(end() - 1); }
    reference       operator[](size_type n)       { return *(begin() + n); }
    const_reference operator[](size_type n) const { return *(begin() + n); }
    pointer         data()                        { return start; }
    const_pointer   data()                  const { return start; }

    //modifiers:
    template <typename Iterator>
    void assign(Iterator first, Iterator last);
    void assign(size_type n, const value_type &value);

    template <typename... Args>
    iterator emplace(const_iterator position, Args&&... args);

    template <typename... Args>
    void emplace_back(Args&&... args);

    void push_back(const value_type& value);
    void push_back(value_type&& value);
    void pop_back();

    iterator insert(const_iterator position, const T& x);
    iterator insert(const_iterator position) { return insert(position, T()); }
    iterator insert(const_iterator position, size_type n, value_type& value);
    template <typename InputIterator>
    void     insert(const_pointer position, InputIterator first, InputIterator last);

    iterator erase(const_iterator position) {
        if(position + 1 != end())
            MYSTL::copy(position + 1, finish, position);
        --finish;
        destroy(finish);
        return position;
    }
    iterator erase(const_iterator first, const_iterator last);
    iterator erase(iterator first, iterator last);
    void clear() { erase(begin(), end()); }

    //resize
    void resize(size_type new_size) { resize(new_size, value_type{}); }
    void resize(size_type new_size, const value_type& value);

    void swap(vector& rhs) noexcept {
        if(this != &rhs) {
            MYSTL::swap(start, rhs.start);
            MYSTL::swap(finish, rhs.finish);
            MYSTL::swap(end_of_storage, rhs.end_of_storage);
        }
    }
};

/***********************************************************************/
//heplers

template <typename T, typename Alloc>
void vector<T, Alloc>::fill_initialize(size_type n, const value_type& value) {
    allocate(n);
    MYSTL::uninitialized_fill_n(begin(), n, value);
}

template <typename T, typename Alloc> template <typename Iterator>
void vector<T, Alloc>::range_initialize(Iterator first, Iterator last) {
    const size_type init_size = static_cast<size_type>(last - first);
    allocate(init_size);
    MYSTL::uninitialized_copy(first, last, start);
}

template <typename T, typename Alloc>
void vector<T, Alloc>::insert_aux(iterator position, const value_type& value) {
    if (finish != end_of_storage) {
        construct(finish, *(finish - 1));
        ++finish;
        T value_copy = value;
        copy_backward(position, finish - 2, finish - 1);
        *position = value_copy;
    }
    else {
        const size_type old_size = size();
        const size_type len = old_size != 0 ? 2 * old_size : 1;
        iterator new_start = data_allocator::allocate(len);
        iterator new_finish = new_start;
        
        try {
            new_finish = uninitialized_copy(start, position, new_start);
            construct(new_finish, value);
            ++new_finish;
            new_finish = uninitialized_copy(position, finish, new_finish);
        }
        catch(...) {
            data_allocator::destroy(new_start, new_finish);
            data_allocator::deallocate(new_start, len);
            throw;
        }

        data_allocator::destroy(begin(), end());
        data_allocator::deallocate(begin(), size());
        start = new_start;
        finish = new_finish;
        end_of_storage = new_start + len;
    }
}


template <typename T, typename Alloc>
template <typename InputIterator>
void vector<T, Alloc>::assign_aux(InputIterator first, InputIterator last, MYSTL::input_iterator_tag) {
    auto cur = start;
    while(first != last && cur != finish)
        *cur++ = *first++;
    if (first == last)
        erase(cur, finish);
    else
        insert(finish, first, last);
}

template <typename T, typename Alloc>
template <typename ForwardIterator>
void vector<T, Alloc>::assign_aux(ForwardIterator first, ForwardIterator last, MYSTL::forward_iterator_tag) {
    const size_type len = MYSTL::distance(first, last);
    if(len > capacity()) {
        vector tmp(first, last);
        swap(tmp);
    }
    else if(len > size()) {
        auto mid = first;
        MYSTL::advance(mid, size());
        MYSTL::copy(first, mid, start);
        auto new_finish = MYSTL::uninitialized_copy(mid, last, finish);
        finish = new_finish;
    }
    else {
        auto new_finish = MYSTL::copy(first, last, start);
        data_allocator::destroy(new_finish, finish);
        finish = new_finish;
    }
}

/******************************************************************************/

//modifiers:
template <typename T, typename Alloc>
template <typename Iterator>
void vector<T, Alloc>::
assign(Iterator first, Iterator last) {
    assign_aux(first, last, MYSTL::iterator_category(first));
}

template <typename T, typename Alloc>
void vector<T, Alloc>::
assign(size_type n, const value_type& value) {
    if(n > capacity()) {
        vector tmp(n, value);
        swap(tmp);
    }
    else if(n > size()) {
        MYSTL::fill(begin(), end(), value);
        finish = MYSTL::uninitialized_fill_n(end(), n - size(), value);
    }
    else {
        erase(MYSTL::fill_n(begin(), n, value), end());
    }
}

    


template <typename T, typename Alloc>
template <typename... Args>
typename vector<T, Alloc>::iterator 
vector<T, Alloc>::emplace(const_iterator position, Args&&... args) {
    auto xpos = const_cast<iterator>(position);
    const auto n = xpos - cbegin();

    if(finish != end_of_storage) {
        if(xpos == cend()) {
            data_allocator::construct(MYSTL::addressof(*finish), forward<Args>(args)...);
            ++finish;
        }
        else {
            auto new_finish = finish;
            data_allocator::construct(MYSTL::addressof(*finish));
            ++new_finish;
            MYSTL::copy_backward(xpos, finish, new_finish);
            *xpos = value_type(MYSTL::forward<Args>(args)...);
        }
    }
    else {

    }

}

template <typename T, typename Alloc>
template <typename... Args>
void vector<T, Alloc>::emplace_back(Args&&... args) {
    if(finish < end_of_storage) {
        data_allocator::construct(MYSTL::addressof(*finish), MYSTL::forward<Args>(args)...);
        ++finish;
    }
    else {
        
    }
}


template <typename T, typename Alloc>
void vector<T, Alloc>::push_back(const value_type& value) {
    if(finish != end_of_storage) {
        construct(finish, value);
        ++finish;
    }
    else
        insert_aux(end(), value);
}

template <typename T, typename Alloc>
void vector<T, Alloc>::push_back(value_type&& value) {
    emplace_back(MYSTL::move(value));
}




/*********************************************************************/
// ctors:
template <typename T, typename Alloc >
vector<T, Alloc>& vector<T, Alloc>::operator=(const vector& rhs) {
    if(this != &rhs)
        assign(rhs.begin(), rhs.end());
    return *this;
}


template <typename T, typename Alloc >
vector<T, Alloc>& vector<T, Alloc>::operator=(vector&& rhs) noexcept
{
    //to impl...
    //(start, finish, end_of_storage - start);
    start = rhs.start;
    finish = rhs.finish;
    end_of_storage = rhs.end_of_storage;
    rhs.start = nullptr;
    rhs.finish = nullptr;
    rhs.end_of_storage = nullptr;
    return *this;
}


} //end of namespace MYSTL

#endif