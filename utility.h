#ifndef UTILITY_H
#define UTILITY_H

#include <cstddef>

#include "type_traits.h"

namespace MYSTL 
{

// move
template <typename T>
typename std::remove_reference<T>::type&& 
move(T&& arg) noexcept {
    using return_type = typename std::remove_reference<T>::type&&;
    return static_cast<return_type>(arg);
}

//forward
template <class T>
T&& forward(typename std::remove_reference<T>::type& arg) noexcept {
    return static_cast<T &&>(arg);
}

template <typename T>
T&& forward(typename std::remove_reference<T>::type&& arg) noexcept {
    static_assert(!std::is_lvalue_reference<T>::value, "bad forward");
    return static_cast<T &&>(arg);
}

// swap

template <typename T>
void swap(T& lhs, T& rhs) {
    auto tmp(MYSTL::move(lhs)); 
    lhs = MYSTL::move(rhs);
    rhs = MYSTL::move(tmp);
}


/******************************pair***************************/
template <class T1, class T2>
struct pair
{
	using first_type  = T1;
	using second_type = T2;

	first_type  first;	
	second_type second; 

	// default constructiable
	template <class Other1 = T1, class Other2 = T2,
			  typename = typename std::enable_if<
				  std::is_default_constructible<Other1>::value &&
					  std::is_default_constructible<Other2>::value,
				  void>::type>
	constexpr pair()
		: first(), second()
	{}

  // implicit constructiable for this type
  template <class U1 = T1, class U2 = T2,
    typename std::enable_if<
    std::is_copy_constructible<U1>::value &&
    std::is_copy_constructible<U2>::value &&
    std::is_convertible<const U1&, T1>::value &&
    std::is_convertible<const U2&, T2>::value, int>::type = 0>
    constexpr pair(const T1& a, const T2& b)
    : first(a), second(b)
    {}

  // explicit constructible for this type
  template <class U1 = T1, class U2 = T2,
    typename std::enable_if<
    std::is_copy_constructible<U1>::value &&
    std::is_copy_constructible<U2>::value &&
    (!std::is_convertible<const U1&, T1>::value ||
     !std::is_convertible<const U2&, T2>::value), int>::type = 0>
    explicit constexpr pair(const T1& a, const T2& b)
    : first(a), second(b)
  {
  }

  pair(const pair& rhs) = default;
  pair(pair&& rhs) = default;

  // implicit constructiable for other type
  template <class Other1, class Other2,
    typename std::enable_if<
    std::is_constructible<T1, Other1>::value &&
    std::is_constructible<T2, Other2>::value &&
    std::is_convertible<Other1&&, T1>::value &&
    std::is_convertible<Other2&&, T2>::value, int>::type = 0>
    constexpr pair(Other1&& a, Other2&& b)
    : first(MYSTL::forward<Other1>(a)),
    second(MYSTL::forward<Other2>(b))
  {
  }

  // explicit constructiable for other type
  template <class Other1, class Other2,
    typename std::enable_if<
    std::is_constructible<T1, Other1>::value &&
    std::is_constructible<T2, Other2>::value &&
    (!std::is_convertible<Other1, T1>::value ||
     !std::is_convertible<Other2, T2>::value), int>::type = 0>
    explicit constexpr pair(Other1&& a, Other2&& b)
    : first(MYSTL::forward<Other1>(a)),
    second(MYSTL::forward<Other2>(b))
  {
  }

  // implicit constructiable for other pair
  template <class Other1, class Other2,
    typename std::enable_if<
    std::is_constructible<T1, const Other1&>::value &&
    std::is_constructible<T2, const Other2&>::value &&
    std::is_convertible<const Other1&, T1>::value &&
    std::is_convertible<const Other2&, T2>::value, int>::type = 0>
    constexpr pair(const pair<Other1, Other2>& other)
    : first(other.first),
    second(other.second)
    {
    }

  // explicit constructiable for other pair
  template <class Other1, class Other2,
    typename std::enable_if<
    std::is_constructible<T1, const Other1&>::value &&
    std::is_constructible<T2, const Other2&>::value &&
    (!std::is_convertible<const Other1&, T1>::value ||
     !std::is_convertible<const Other2&, T2>::value), int>::type = 0>
    explicit constexpr pair(const pair<Other1, Other2>& other)
    : first(other.first),
    second(other.second)
    {
    }

  // implicit constructiable for other pair
  template <class Other1, class Other2,
    typename std::enable_if<
    std::is_constructible<T1, Other1>::value &&
    std::is_constructible<T2, Other2>::value &&
    std::is_convertible<Other1, T1>::value &&
    std::is_convertible<Other2, T2>::value, int>::type = 0>
    constexpr pair(pair<Other1, Other2>&& other)
    : first(MYSTL::forward<Other1>(other.first)),
    second(MYSTL::forward<Other2>(other.second))
    {
    }

  // explicit constructiable for other pair
  template <class Other1, class Other2,
    typename std::enable_if<
    std::is_constructible<T1, Other1>::value &&
    std::is_constructible<T2, Other2>::value &&
    (!std::is_convertible<Other1, T1>::value ||
     !std::is_convertible<Other2, T2>::value), int>::type = 0>
    explicit constexpr pair(pair<Other1, Other2>&& other)
    : first(MYSTL::forward<Other1>(other.first)),
    second(MYSTL::forward<Other2>(other.second))
  {
  }

  // copy assign for this pair
	pair &operator=(const pair &rhs) {
    	if (this != &rhs) {
			first = rhs.first;
			second = rhs.second;
    	}
		return *this;
  	}

  // move assign for this pair
  	pair& operator=(pair&& rhs) {
    	if (this != &rhs) {
			first = MYSTL::move(rhs.first);
			second = MYSTL::move(rhs.second);
    	}
    	return *this;
  	}

  // copy assign for other pair
  	template <class Other1, class Other2>
	pair& operator=(const pair<Other1, Other2>& other) {
		first = other.first;
		second = other.second;
		return *this;
  	}

  // move assign for other pair
  	template <class Other1, class Other2>
  	pair& operator=(pair<Other1, Other2>&& other) {
		first = MYSTL::forward<Other1>(other.first);
		second = MYSTL::forward<Other2>(other.second);
		return *this;
  	}

  	~pair() = default;

  	void swap(pair& other) {
    	if (this != &other) {
			MYSTL::swap(first, other.first);
			MYSTL::swap(second, other.second);
    	}
  	}

};

template <class T1, class T2>
bool operator==(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs) {
	return lhs.first == rhs.first && lhs.second == rhs.second;
}

template <class T1, class T2>
bool operator<(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs) {
	return lhs.first < rhs.first || (lhs.first == rhs.first && lhs.second < rhs.second);
}

template <class T1, class T2>
bool operator!=(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs) {
	return !(lhs == rhs);
}

template <class T1, class T2>
bool operator>(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs) {
	return rhs < lhs;
}

template <class T1, class T2>
bool operator<=(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs) {
	return !(rhs < lhs);
}

template <class T1, class T2>
bool operator>=(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs) {
	return !(lhs < rhs);
}

// 重载 MYSTL 的 swap
template <class T1, class T2>
void swap(pair<T1, T2>& lhs, pair<T1, T2>& rhs) {
	lhs.swap(rhs);
}

// 全局函数，让两个数据成为一个 pair
template <class T1, class T2>
pair<T1, T2> make_pair(T1&& first, T2&& second) {
	return pair<T1, T2>(MYSTL::forward<T1>(first), MYSTL::forward<T2>(second));
}



}


#endif