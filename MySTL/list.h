#ifndef LIST_H
#define LIST_H


#include <initializer_list>
#include "iterator.h"
#include "utility.h"
#include "allocator.h"
#include "memory.h"

namespace LIST_H
{



template <typename T>
struct list_node {
    using node_ptr = list_node<T>*;

    node_ptr prev;
    node_ptr next;
    T data;

public:
    void hook(node_ptr position) {
        next = position;
        prev = position->prev;
        position->prev->next = this;
        position->prev = this;
    }

    void unhook() {
        const node_ptr next_node = next;
        const node_ptr prev_node = prev;
        prev_node->next = next_node;
        next_node->prev = prev_node;
    }
    
    void transfer(node_ptr first, node_ptr last) {
        if (this != last) {
            // Remove [first, last) from its old position.
            last->prev->next = this;
            first->prev->next = last;
            prev->next = first;
            // Splice [first, last) into its new position.
            node_ptr tmp = prev;
            prev = last->prev;
            last->prev = first->prev;
            first->prev = tmp;
        }
    }

};

template <typename T>
struct list_iterator 
    :public MYSTL::iterator<MYSTL::bidirectional_iterator_tag, T>
{
    using iterator_category = MYSTL::bidirectional_iterator_tag;
    using value_type        = T;
    using pointer           = T*;
    using reference         = T&;
    using self              = list_iterator<T>;
    using node_ptr          = list_node<T>*;

    node_ptr node_;

    list_iterator() = default;
    list_iterator(const node_ptr& node) :node_(node) {}
    list_iterator(const list_iterator& rhs):node_(rhs.node_) {}

    reference operator*() const { return node_->data; }
    pointer operator->() const { return &(operator*()); }

    self& operator++() {
        node_ = node_->next;
        return *this;
    }
    self operator++(int) {
        self tmp = *this;
        ++*this;
        return tmp;
    }
    self& operator--() {
        node_ = node_->prev;
        return *this;
    }
    self operator--(int) {
        self tmp = *this;
        --*this;
        return tmp;
    }

    bool operator==(const self& rhs) const { return node_ == rhs.node_; }
    bool operator!=(const self& rhs) const { return node_ != rhs.node_; }
};
 
template <typename T>
struct list_const_iterator 
        :public MYSTL::iterator<MYSTL::bidirectional_iterator_tag, T>
{

    using iterator_category = MYSTL::bidirectional_iterator_tag;
    using value_type        = T;
    using pointer           = const T*;
    using reference         = const T&;
    using self              = list_const_iterator<T>;
    using iterator          = list_iterator<T>;

    using node_ptr          = list_node<T>*;
    node_ptr node_;

    list_const_iterator() = default;
    list_const_iterator(node_ptr node)  : node_(node) {}
    list_const_iterator(const list_iterator<T>& rhs)   : node_(rhs.node_) {}
    list_const_iterator(const list_const_iterator& rhs)  : node_(rhs.node_) {}

    iterator __const_cast() const noexcept {
        return iterator(const_cast<list_node<T>*>(node_));
    }

    reference operator*() const { return node_->data; }
    pointer operator->() const { return &(operator*()); }

    self& operator++() {
        node_ = node_->next;
        return *this;
    }
    self operator++(int) {
        self tmp = *this;
        ++*this;
        return tmp;
    }
    self &operator--() {
        node_ = node_->prev;
        return *this;
    }
    self operator--(int) {
        self tmp = *this;
        --*this;
        return tmp;
    }

    bool operator==(const self& rhs) const { return node_ == rhs.node_; }
    bool operator!=(const self& rhs) const { return node_ != rhs.node_; }
};


template <typename T, typename Alloc = MYSTL::allocator<T>>
class list {
public:
    using allocator_type         = MYSTL::allocator<T>;
    using data_allocator         = MYSTL::allocator<T>;
    using node_allocator         = MYSTL::allocator<list_node<T>>;

    using value_type             = T;
    using reference              = value_type&;
    using const_reference        = const value_type&;
    using size_type              = size_t;
    using difference_type        = ptrdiff_t;
    
    // using value_type             = typename allocator_type::value_type;
    // using reference              = typename allocator_type::reference;
    // using const_reference        = typename allocator_type::const_reference;
    // using size_type              = typename allocator_type::size_type;
    // using difference_type        = typename allocator_type::difference_type;

    using iterator               = list_iterator<value_type>;
    using const_iterator         = list_const_iterator<value_type>;
    using reverse_iterator       = MYSTL::reverse_iterator<iterator>;
    using const_reverse_iterator = MYSTL::reverse_iterator<const_iterator>;

protected:
    using node_ptr               = list_node<T>*;
    node_ptr node_;
    size_type size_;

public:
    list() { empty_initialize(); }
    explicit list(size_type n) { fill_initialize(n, value_type{}); }
    list(size_type n, const T &value) { fill_initialize(n, value); }

    template <typename Iterator>
    list(Iterator first, Iterator last) { range_initialize(first, last); }
    list(const list& rhs) { range_initialize(rhs.begin(), rhs.end()); }
    list(list&& rhs) noexcept : node_(rhs.node), size_(rhs.size_) 
    { rhs.node_ = nullptr;   rhs.size_ = 0; }
    list(const std::initializer_list<T> &ilist) { range_initialize(ilist.begin(), ilist.end()); }

    list &operator=(const list& rhs) {
        if(this != &rhs) {
            assign(rhs.begin(), rhs.end());
        }
        return *this;
    }

    list &operator=(list&& rhs) {
        clear();
        splice(end(), rhs);
        return *this;
    }
    
    list &operator=(std::initializer_list<T> ilist) {
        list tmp(ilist.begin(), ilist.end());
        swap(tmp);
        return *this;
    }

    ~list() {
        if(node_ != nullptr) {
            clear();
            node_allocator::deallocate(node_, 1);
            node_ = nullptr;
            size_ = 0;
        }
    }

public:
    void empty_initialize() {
        node_ = node_allocator::allocate(1);
        node_->next = node_;
        node_->prev = node_;
        size_ = 0;
    }

    void fill_initialize(size_type n, const T& value) {
        empty_initialize();
        insert(begin(), n, value);
    }

    template <typename InputIterator>
    void range_initialize(InputIterator first, InputIterator last) {
        empty_initialize();
        // insert( begin(), first, last); 不可以直接insert, 因为insert要求是list的迭代器
        size_type n = MYSTL::distance(first, last);
        try {
            for (; n > 0; --n, ++first) {
                __insert(end(), *first);
            }
        }
        catch(...) {
            clear();
            node_allocator::deallocate(node_, 1);
            node_ = nullptr;
            throw;
        }
    }

public:
    //iterators
    iterator        begin()       { return node_->next; }
    const_iterator  begin() const { return node_->next; }
    iterator        end()         { return node_; }
    const_iterator  end()   const { return node_; }

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
    { return size_; }
    size_type       max_size() const 
    { return static_cast<size_type>(-1); }
    bool            empty() const { return begin() == end(); }

    //Element access
    reference       front() { return *begin(); }
    const_reference front() const { return *begin(); }

    reference       back() { return *(--end()); }
    const_reference back() const { return *(--end()); }

    //modifiers
    void assign(size_type n, const value_type& value) {
        auto i = begin();
        auto j = end();
        for (; n > 0 && i != j; --n, ++i) {
            *i = value;
        }
        if (n > 0)
            insert(j, n, value);
        else
            erase(i, j);
    }

    template <typename Iterator>
    void assign(Iterator first, Iterator last) {
        auto this_first = begin(), this_last = end();
        for (; this_first != this_last && first != last; ++this_first, ++first) {
            *this_first = *first;
        }
        if(first == last) {
            erase(this_first, this_last);
        }
        else {
            insert(this_first, first, last);
        }
    }
    void assign(std::initializer_list<T> ilist) {
        assign(ilist.begin(), ilist.end()); 
    }

    template <typename... Args>
    void emplace_front(Args&&... args) {
        __insert(begin(), std::forward<Args>(args)...);
        return front();
    }

    void  push_front(const value_type& value) {
        __insert(begin(), value);
    }
    void  push_front(value_type&& value) {
        __insert(begin(), MYSTL::move(value));
    }

    void pop_front() {
        __erase(begin());
    }

    template <typename... Args>
    void emplace_back(Args&&... args) {
        __insert(end(), MYSTL::forward<Args>(args)...);
    }

    void push_back(const value_type& value) {
        __insert(end(), value);
    }
    void push_back(value_type&& value) {
        __insert(end(), MYSTL::move(value));
    }

    void pop_back() {
        __erase(node_->prev);
    }

    template <typename... Args>
    iterator emplace(const_iterator position, Args&&... args) {
        auto tmp = create_node(MYSTL::forward<Args>(args)...);
        tmp->hook(position.node_);
        ++size_;
        return iterator(tmp);
    }

    iterator insert(const_iterator position, const value_type& value) {
        auto tmp = create_node(value);
        tmp->hook(position.node_);
        ++size_;
        return iterator(tmp);
    }
    iterator insert(const_iterator position, value_type&& value) {
        return emplace(position, MYSTL::move(value));
    }
    iterator insert(const_iterator position, size_type n, const value_type& value) {
        if(n) {
            list tmp(n, value);
            iterator it = tmp.begin();
            splice(position, tmp);
            return it;
        }
        return iterator(position);
    }

    template <typename InputIterator>
            //  ,typename = std::_RequireInputIter<InputIterator>
    iterator
    insert(const_iterator position, InputIterator first, InputIterator last) {
        list tmp(first, last);
        if(!tmp.empty()) {
            iterator it = tmp.begin();
            splice(position, tmp);
            return it;
        }
        return position.__const_cast();
    }

    iterator erase(const_iterator position) {
        auto tmp = position.node_->next;
        __erase(position.__const_cast());
        return iterator(tmp);
    }
    iterator erase(const_iterator first, const_iterator last) {
        while(first != last)
            first = erase(first);
        return last.__const_cast();
    }

    void swap(list& rhs) {
        MYSTL::swap(node_, rhs.node_);
        MYSTL::swap(size_, rhs.size_);
    }

    void resize(size_type new_size) {
        resize(new_size, value_type{});
    }
    void resize(size_type new_size, const value_type& value) {
        auto iter = begin();
        size_type len = 0;
        while(iter != end() && len < new_size) {
            ++iter;
            ++len;
        }
        if(len == new_size) {
            erase(iter, end());
        }
        else {
            insert(end(), new_size - len, value);
        }
    }
    
    void clear() {
        erase(begin(), end());
    }

public:
    void splice(const_iterator position, list& other) {
        splice(position, MYSTL::move(other));
    }
    void splice(const_iterator position, list&& other) noexcept {
        if (!other.empty()) {
            transfer(position.__const_cast(), other.begin(), other.end());
            size_ += other.size_;
            other.size_ = 0;
          }
    }

    void splice(const_iterator position, list& other, const_iterator i) {
        splice(position, MYSTL::move(other), i);
    }
    void splice(const_iterator position, list&& other, const_iterator i) {
        iterator j = i.__const_cast();
        ++j;
        if(position == i || position == j)
            return;
        transfer(position.__const_cast(), i.__const_cast(), j);
        ++size_;
        ++other->size_;
    }

    void splice(const_iterator position, list&& other, const_iterator first, const_iterator last) {
        if(first != last && this != &other) {
            auto n = MYSTL::distance(first, last);
            size_ += n;
            other.size_ -= n;
            transfer(position.__const_cast(), 
                    first.__const_cast(), 
                    last.__const_cast());
        }
    }
    void splice(const_iterator position, list& other, const_iterator first, const_iterator last) {
        splice(position, MYSTL::move(other), first, last);
    }

    void remove(const value_type& value) {
        remove_if([&](const value_type &v) { return v == value; });
    }

    template <typename UnaryPredicate>
    void remove_if(UnaryPredicate pred) {
        auto first = begin();
        auto last = end();
        while(first != last) {
            if(pred(*first))
                first = erase(first);
            else
                ++first;
        }
    }

    // void unique();
    // void merge();
    // void sort();
    void reverse() {
        if(size() > 1) {
            auto i = begin();
            auto last = end();
            while(i.node_  != last.node_) {
                MYSTL::swap(i.node_->prev, i.node_->next);
                i.node_ = i.node->prev;
            }
            MYSTL::swap(last.node_->prev, last.node_->next);
        } 
    }

protected:
    //helper functions:
    
    template <typename... Args>
    node_ptr create_node(Args&& ...args) {
        auto p = node_allocator::allocate(1);
        try {
            data_allocator::construct(MYSTL::addressof(p->data), MYSTL::forward<Args>(args)...);
            p->prev = p->next = nullptr;
        }
        catch(...) {
            node_allocator::deallocate(p, 1);
            throw;
        }
        return p;
    }
    void destroy_node(node_ptr p) {
        data_allocator::destroy(MYSTL::addressof(p->data));
        node_allocator::deallocate(p, 1);
    }

    //连接到position的前一个
    template <typename... Args>
    void __insert(iterator position, Args&&... args) {
        node_ptr tmp = create_node(MYSTL::forward<Args>(args)...);
        tmp->hook(position.node_);
        ++size_;
    }

    void __erase(iterator position) noexcept {
        --size_;
        position.node_->unhook();
        destroy_node(position.node_);
    }

    void transfer(iterator position, iterator first, iterator last) {
        position.node_->transfer(first.node_, last.node_);
    }
};




// overload swap over MYSTL::swap
// see <<effective c++>> Item...
template <typename T>
void swap(list<T>& lhs, list<T>& rhs) noexcept {
    lhs.swap(rhs);
}

} // end of namespace list

#endif