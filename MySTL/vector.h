#ifndef VECTOR_H_
#define VECTOR_H_

#include <initializer_list>
#include "memory.h"
#include "utility.h"
#include "iterator.h"
#include "allocator.h"
#include <iterator>

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
    vector(size_type n) { fill_initialize(n, value_type()); }
    vector(const vector& rhs) { range_initialize(rhs.begin(), rhs.end()); }

    // template <typename InputIterator, 
    // typename = typename std::enable_if<std::is_convertible<typename
    // iterator_traits<InputIterator>::iterator_category, MYSTL::input_iterator_tag>::value>::type>
    template <typename InputIterator, typename = typename
      std::enable_if<std::is_convertible<typename
		std::iterator_traits<InputIterator>::iterator_category,
			       std::input_iterator_tag>::value>::type>
    vector(InputIterator first, InputIterator last) { range_initialize(first, last); }

    vector& operator=(const vector& rhs);
    vector(vector&& rhs) noexcept;
    vector& operator=(vector&& rhs) noexcept;
    vector(std::initializer_list<value_type> ilist) { range_initialize(ilist.begin(), ilist.end()); }

    ~vector() {
        data_allocator::destroy(start, finish);
        data_allocator::deallocate(start, end_of_storage - start);
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
    size_type   size()     const  { return static_cast<size_type>(end() - begin()); }
    size_type   capacity() const  { return static_cast<size_type>(end_of_storage - begin()); }
    bool        empty()    const  { return begin() == end() && start != nullptr; }
    void        shrink_to_fit() {
        if(size() < capacity()) {
            const auto new_size = size();
            auto new_start = data_allocator::allocate(new_size);
            try {
                MYSTL::uninitialized_move(start, finish, new_start);
            }
            catch(...) {
                data_allocator::deallocate(new_start, new_size);
                throw;
            }
            data_allocator::deallocate(start, end_of_storage - start);
            start = new_start;
            finish = end_of_storage = start + new_size;
        }
    }

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
    template <typename InputIterator, typename = typename
      std::enable_if<std::is_convertible<typename
		std::iterator_traits<InputIterator>::iterator_category,
			       std::input_iterator_tag>::value>::type>
    void assign(InputIterator first, InputIterator last);
    void assign(size_type n, const value_type &value);

    template <typename... Args>
    iterator emplace(const_iterator position, Args&&... args);

    template <typename... Args>
    void emplace_back(Args&&... args);

    void push_back(const value_type& value);
    void push_back(value_type&& value);
    void pop_back();

    iterator insert(const_iterator position, const value_type& value);
    iterator insert(const_iterator position, value_type&& value) { return emplace(position, MYSTL::move(value)); }
    iterator insert(const_iterator position) { return insert(position, T()); }
    iterator insert(const_iterator position, size_type n, const value_type& value);

    template <typename InputIterator, typename = typename
      std::enable_if<std::is_convertible<typename
		std::iterator_traits<InputIterator>::iterator_category,
			       std::input_iterator_tag>::value>::type>
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
    void     clear() { erase(begin(), end()); }

    //resize
    void resize(size_type new_size) { resize(new_size, value_type{}); }
    void resize(size_type new_size, const value_type& value) {
        if(new_size > capacity()) {
            vector tmp(new_size, value);
            swap(tmp);
        }
        else if(new_size > size()) {
            insert(end(), new_size - size(), value);
        }
        else {
            erase(begin() + new_size, end());
        }
    }


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
        const size_type new_size = old_size != 0 ? 2 * old_size : 1;
        iterator new_start = data_allocator::allocate(new_size);
        iterator new_finish = new_start;

        try {
            new_finish = uninitialized_copy(start, position, new_start);
            construct(new_finish, value);
            ++new_finish;
            new_finish = uninitialized_copy(position, finish, new_finish);
        }
        catch(...) {
            data_allocator::destroy(new_start, new_finish);
            data_allocator::deallocate(new_start, new_size);
            throw;
        }

        data_allocator::destroy(begin(), end());
        data_allocator::deallocate(begin(), size());
        start = new_start;
        finish = new_finish;
        end_of_storage = new_start + new_size;
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
template <typename InputIterator, typename>
void vector<T, Alloc>::
assign(InputIterator first, InputIterator last) {
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
    auto const_cast_pos = const_cast<iterator>(position);
    const size_type n = position - cbegin();
    if(finish != end_of_storage) {
        if(const_cast_pos == cend()) {
            data_allocator::construct(MYSTL::addressof(*finish), forward<Args>(args)...);
            ++finish;
        }
        else {
            auto new_finish = finish;
            data_allocator::construct(MYSTL::addressof(*finish));
            ++new_finish;
            MYSTL::copy_backward(const_cast_pos, finish, new_finish);
            *const_cast_pos = value_type(MYSTL::forward<Args>(args)...);
        }
    }
    else { //need to reallocate:
        const size_type old_size = size();
        const size_type new_size = old_size != 0 ? 2 * old_size : 1;

        auto new_start = data_allocator::allocate(new_size);
        auto new_finish = new_start;
        try {
            new_finish = MYSTL::uninitialized_move(start, const_cast_pos, new_start);
            data_allocator::construct(MYSTL::addressof(*new_finish), MYSTL::forward<Args>(args)...);
            ++new_finish;
            new_finish = MYSTL::uninitialized_move(const_cast_pos, finish, new_finish);
        }
        catch(...) {
            data_allocator::deallocate(new_start, new_size);
            throw;
        }
        data_allocator::destroy(start, finish);
        data_allocator::deallocate(start, end_of_storage - start);
        start = new_start;
        finish = new_finish;
        end_of_storage = new_start + new_size;
    }
    return start + n;
}

template <typename T, typename Alloc>
template <typename... Args>
void vector<T, Alloc>::emplace_back(Args&&... args) {
    emplace(end(), (args)...);
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

template <typename T, typename Alloc>
void vector<T, Alloc>::pop_back() {
    if(!empty()) {
        --finish;
        destroy(finish);
    }
}

template <typename T, typename Alloc>
typename vector<T, Alloc>::iterator
vector<T, Alloc>::insert(const_iterator position, const value_type& value) {
    const auto n = position - begin();
    if(finish != end_of_storage && position == end()) {
        construct(finish, value);
        ++finish;
    }
    else
        insert_aux(position, value);
    return begin() + n;
}


template <class T, class Alloc>
typename vector<T, Alloc>::iterator 
vector<T, Alloc>::insert(const_iterator position, size_type n, const value_type& value) {
    if (n != 0) {
        if (static_cast<size_type>(end_of_storage - finish) >= n) {
            T value_copy(value);
            const size_type elems_after = finish - position;
            iterator old_finish = finish;
            if (elems_after > n) {
                finish = uninitialized_copy(finish - n, finish, finish);
                move_backward(position, old_finish - n, old_finish);
                uninitialized_fill_n(position, n, value_copy);
            }
            else {
                finish = uninitialized_fill_n(finish, n - elems_after, value_copy);
                finish = uninitialized_move(position, old_finish, finish);
                uninitialized_fill_n(position, elems_after, value_copy);
            }
        }
        else { //need to reallocate
            const size_type old_size = size();        
            const size_type len = old_size + max(old_size, n);
            auto new_start = data_allocator::allocate(len);
            auto new_finish = new_start;
            try {
                new_finish = uninitialized_move(start, position, new_start);
                new_finish = uninitialized_fill_n(new_finish, n, value);
                new_finish = uninitialized_move(position, finish, new_finish);
            }
            catch(...) {
                destroy(new_start, new_finish);
                data_allocator::deallocate(new_start, len);
                throw;
            }
            data_allocator::destroy(start, finish);
            data_allocator::deallocate(start, end_of_storage - start);
            start = new_start;
            finish= new_finish;
            end_of_storage = new_start + len;
        }
    }

}

template <typename T, typename Alloc>
template <typename InputIterator, typename>
void vector<T, Alloc>::insert(const_pointer position, InputIterator first, InputIterator last) {
    if (first == last)  return;
    auto pos = static_cast<iterator>(position);
    const auto n = MYSTL::distance(first, last);
    if (end_of_storage - finish >= n) { 
        const auto after_elems = finish - position;
        auto old_finish = finish;
        if (after_elems > n) {
            finish = MYSTL::uninitialized_copy(finish - n, finish, finish);
            MYSTL::move_backward(pos, old_finish - n, old_finish);
            MYSTL::uninitialized_copy(first, last, pos);
        }
        else
        {
            auto mid = first;
            MYSTL::advance(mid, after_elems);
            finish = MYSTL::uninitialized_copy(mid, last, finish);
            finish = MYSTL::uninitialized_move(pos, old_finish, finish);
            MYSTL::uninitialized_copy(first, mid, pos);
        }
    }
    else { // need to reallocate
        const size_type old_size = size();  
        const size_type len = old_size + max(old_size, n);
        auto new_start = data_allocator::allocate(len);
        auto new_finish = new_start;
        try {
            new_finish = MYSTL::uninitialized_move(start, pos, new_start);
            new_finish = MYSTL::uninitialized_copy(first, last, new_finish);
            new_finish = MYSTL::uninitialized_move(pos, finish, new_finish);
        }
        catch(...) {
            destroy(new_start, new_finish);
            data_allocator::deallocate(new_start, len);
            throw;
        }
        data_allocator::destroy(start, finish);
        data_allocator::deallocate(start, end_of_storage - start);
        start = new_start;
        finish = new_finish;
        end_of_storage = start + len;
    }
}


/*********************************************************************/
// ctors:
template <typename T, typename Alloc >
vector<T, Alloc>& vector<T, Alloc>::operator=(const vector& rhs) {
    if(this != &rhs)
        assign(rhs.begin(), rhs.end());
    return *this;
}

template <typename T, typename Alloc>
vector<T, Alloc>::vector(vector&& rhs) noexcept
                    :start(rhs.start), 
                    finish(rhs.finish), 
                    end_of_storage(rhs.end_of_storage)
{
    rhs.start = rhs.finish = rhs.end_of_storage = nullptr;
}



template <typename T, typename Alloc >
vector<T, Alloc>& vector<T, Alloc>::operator=(vector&& rhs) noexcept {
    data_allocator::destroy(start, finish);
    data_allocator::deallocate(start, end_of_storage - start);
    start = rhs.start;
    finish = rhs.finish;
    end_of_storage = rhs.end_of_storage;
    rhs.start = rhs.finish = rhs.end_of_storage = nullptr;
    return *this;
}

/******************************************************************************/
template <typename T, typename Alloc>
void swap(vector<T, Alloc>& lhs, vector<T, Alloc>& rhs) noexcept  {
    lhs.swap(rhs);
}

template <typename T, typename Alloc>
bool operator==(const vector<T, Alloc>& lhs, const vector<T, Alloc>& rhs) {
    return lhs.size() == rhs.size() && MYSTL::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename T, typename Alloc>
bool operator!=(const vector<T, Alloc>& lhs, const vector<T, Alloc>& rhs) {
    return !(lhs == rhs);
}

template <typename T, typename Alloc>
bool operator<(const vector<T, Alloc>& lhs, const vector<T, Alloc>& rhs) {
  return MYSTL::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), lhs.end());
}

template <typename T, typename Alloc>
bool operator>(const vector<T, Alloc>& lhs, const vector<T, Alloc>& rhs) {
    return rhs < lhs;
}

template <typename T, typename Alloc>
bool operator<=(const vector<T, Alloc>& lhs, const vector<T, Alloc>& rhs) {
    return !(rhs < lhs);
}

template <typename T, typename Alloc>
bool operator>=(const vector<T, Alloc>& lhs, const vector<T, Alloc>& rhs) {
    return !(lhs < rhs);
}



} //end of namespace MYSTL

#endif
