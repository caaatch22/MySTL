#ifndef VECTOR_H_
#define VECTOR_H_

#include "memory.h"
#include "utility.h"
#include "iterator.h"
#include "allocator.h"

namespace MYSTL
{

template <typename T, typename Alloc = allocator<T>>
class vector {
public:
    using data_allocator         = allocator<T>;
    using iterator_category      = MYSTL::random_access_iterator_tag;

    using value_type             = T;
    using iterator               = value_type*;
    using const_iterator         = const value_type*;
    using reference              = value_type&;
    using const_reference        = const value_type&;
    using size_type              = size_t;
    using difference_type        = ptrdiff_t;

    using reverse_iterator       = reverse_iterator<iterator>;
    using const_reverse_iterator = reverse_iterator<const_iterator>;
    

protected:
    iterator start;
    iterator finish;
    iterator end_of_storage;

    void insert_aux(iterator position, const T& value);
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
    void deallocate() {
        if(start) data_allocator::deallocate(start, end_of_storage - start);
    }
    void fill_initialize(size_type n, const value_type& value);

    template <typename Iterator>
    void range_initialize(Iterator first, Iterator last);


public:
    
    //ctors:
    vector() : start(0), finish(0), end_of_storage(0) {}
    vector(size_type n, const value_type& value) { fill_initialize(n, value); }
    vector(const vector &rhs) { range_initialize(rhs.begin(), rhs.end()); }
    template <typename InputIterator>
    vector(InputIterator first, InputIterator last) { range_initialize(first, last); }
    vector& operator=(const vector& rhs) {
        if(this != &rhs) {
            assign(rhs.begin(), rhs.end());
        }
        return *this;
    }
    vector(vector &&rhs) noexcept;
    vector& operator=(vector&& rhs) noexcept;

    ~vector();

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
    reference       at(size_type n) { return (*this)[n]; }
    const_reference at(size_type n) const { return (*this)[n]; }
    reference       front() { return *begin(); }
    const_reference front() const { return *begin(); }
    reference       back() { return *(end() - 1); }
    const_reference back() const { return *(end() - 1); }
    reference       operator[](size_type n) { return *(begin() + n); }
    const_reference operator[](size_type n) const { return *(begin() + n); }
    
    //modifiers:
    template <typename Iterator>
    void assign(Iterator first, Iterator last) {
        size_type n = static_cast<size_type>(last - first);

    }

    void assign(size_type n, const value_type& value) {
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
    void resize(size_type new_size) {return resize(new_size, value_type{}; }
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

template <typename T, typename Alloc> template <typename Iterator>
void vector<T, Alloc>::range_initialize(Iterator first, Iterator last) {
    
}


template <typename T, typename Alloc>
void vector<T, Alloc>::insert_aux(iterator position, const value_type& value) {
    if(finish != end_of_storage) {
        construct(finish, *(finish - 1));
        ++finish;
    }
}

template <typename T, typename Alloc>
void vector<T, Alloc>::fill_initialize(size_type n, const value_type& value) {
    allocate(n);
    MYSTL::uninitialized_fill_n(begin(), n, value);
}

/******************************************************************************/

template <typename T, typename Alloc>
template <typename... Args>
typename vector<T, Alloc>::iterator 
vector<T, Alloc>::emplace(const_iterator position, Args&&... args) {
    auto xpos = const_cast<iterator>(position);
    const auto n = xpos - cbegin();
    
    if(finish != end_of_storage) {
        if(xpos == cend()) {
            data_allocator::construct(addressof(finish), forward<Args>(args)...);
            ++finish;
        }
        else {
            data_allocator::construct(addressof())
        }
    }

}

template<typename _Alloc>
    void
    vector<bool, _Alloc>::
    _M_insert_aux(iterator __position, bool __x)
    {
      if (this->_M_impl._M_finish._M_p != this->_M_impl._M_end_addr())
	{
	  std::copy_backward(__position, this->_M_impl._M_finish, 
			     this->_M_impl._M_finish + 1);
	  *__position = __x;
	  ++this->_M_impl._M_finish;
	}
      else
	{
	  const size_type __len =
	    _M_check_len(size_type(1), "vector<bool>::_M_insert_aux");
	  _Bit_pointer __q = this->_M_allocate(__len);
	  iterator __start(std::__addressof(*__q), 0);
	  iterator __i = _M_copy_aligned(begin(), __position, __start);
	  *__i++ = __x;
	  iterator __finish = std::copy(__position, end(), __i);
	  this->_M_deallocate();
	  this->_M_impl._M_end_of_storage = __q + _S_nword(__len);
	  this->_M_impl._M_start = __start;
	  this->_M_impl._M_finish = __finish;
	}
    }


{
	const auto __n = __position - cbegin();
	if (this->_M_impl._M_finish != this->_M_impl._M_end_of_storage)
	  if (__position == cend())
	    {
	      _GLIBCXX_ASAN_ANNOTATE_GROW(1);
	      _Alloc_traits::construct(this->_M_impl, this->_M_impl._M_finish,
				       std::forward<_Args>(__args)...);
	      ++this->_M_impl._M_finish;
	      _GLIBCXX_ASAN_ANNOTATE_GREW(1);
	    }
	  else
	    {
	      // We need to construct a temporary because something in __args...
	      // could alias one of the elements of the container and so we
	      // need to use it before _M_insert_aux moves elements around.
	      _Temporary_value __tmp(this, std::forward<_Args>(__args)...);
	      _M_insert_aux(begin() + __n, std::move(__tmp._M_val()));
	    }
	else
	  _M_realloc_insert(begin() + __n, std::forward<_Args>(__args)...);

	return iterator(this->_M_impl._M_start + __n);
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



/*********************************************************************/
// ctors:
template <typename T, typename Alloc >
vector<T, Alloc>& vector<T, Alloc>::operator=(const vector& rhs) {
    if(this != &rhs) {
        const auto count = rhs.size();
        if(count > capacity()) {
            vector result(rhs.begin(), rhs.end());
            swap(result);
        }
        else if(count <= size()) {
            auto itr = std::copy(rhs.begin(), rhs.end(), begin());
            //to impl...
            finish = start + count;
        }
        else {
            std::copy(rhs.begin(), rhs.begin() + size(), start);
            MYSTL::uninitialized_copy(rhs.begin() + size(), rhs.end(), finish);
            end_of_storage = finish = start + count;
        }
    }
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