#pragma once

#include "../Allocator/allocator.hpp"
#include "../Allocator/uninitialized.hpp"
#include "../Iterator/iterator.hpp"

#include <cstdint>

namespace MiniSTL {


template <class T, class Allocator = allocator<T>>
class vector {
public:g
    using value_type = T；
    using allocator_type = Allocator;
    using size_type	= size_t;
    using difference_type = ptrdiff_t;
    using reference	= T&;
    using const_reference = const T&;
    using pointer = T*;
    using const_pointer = const T*;
    using iterator = T*;    
    using const_iterator = const T*;
    using reverse_iterator	= reverse_iterator<iterator>
    using const_reverse_iterator = reverse_iterator<const_iterator>

protected:
    T* start;
    T* finish;
    T* end_of_storage;

protected:
    using alloc = alloctor_type;
    
    template <class Integer>
    void initialize_aux(Integer n, Integer val, true_type) {
        start = alloc::allocate(n);
        end_of_storage = start + n; 
        finish = uninitialized_fill_n(start, n, val);
    }

    template <class InputIt>
    void initialize_aux(InputIt first, InputIt last, false_type) {
        range_initialize(first, last, iterator_category<InputIt>);
    }

    template <class InputIt>
    void range_initialize(InputIt first, InputIt last, input_iterator_tag) {
        for (;first != last;++first)
            push_back(*first);
    }

    template <class ForwardIt>
    void range_initialize(ForwardIt first, ForwardIt last, forward_iterator_tag) {
        size_type n = 0;
        distance(first, last, n);
        start = _M_allocate(n);
        end_of_storage = start + n;
        _M_finish = uninitialized_copy(first, last, start);
    }

    template <class ForwardIt>
    iterator allocate_and_copy(size_type n, ForwardIt first, ForwardIt last) {
        iterator result = alloc::allocate(n);
        uninitialized_copy(first, last, result);
        return result;
    }

public: 
// ctor and dtor
    vector();

    vector(size_type count, const T& val);

    explicit vector(size_type count);

    vector(const vector& other);

    template <class InputIt>
    vector(InputIt first, InputIt last) {
        initialize_aux(first, last, is_iterger<InputIt>());
    }
    
    vector(vector&& other) noexcept;

    vector(std::initializer_list<T> init);

    ~vector() {}


    vector& operator=(vector&& other);

    vector& operator=(vector&&) noexcept;

    vector& operator=(std::initializer_list<T> ilist);

public:
// assign
    void assign(size_type count, const T& val);
    template <class InputIt >
    void assign(InputIt first, InputIt last);
    void assign(std::initializer_list<T> ilist);
 
    allocator_type get_allocator() const { return allocator_type(); }
 
public:
// element access
    // throw std::out_of_range
    reference at(size_type pos);
    const_reference at(size_type pos) const;

    reference operator[](size_type pos);
    const_reference operator[](size_type pos) const;

    reference front();
    const_reference front() const;

    reference back();
    const_reference back() const;

    T* data() noexcept;
    const T* data() const noexcept;

public:
// iterator
    iterator begin() noexcept { return start; }
	iterator end() noexcept { return finish; }
	reverse_iterator rbegin() noexcept { return reverse_iterator(finish); }
	reverse_iterator rend() noexcept { return reverse_iterator(start); }

    const_iterator begin() const noexcept { return start; }
	const_iterator end() const noexcept { return finish; }
    const_reverse_iterator rbegin() const noexcept { return reverse_iterator(finish); }
    const_reverse_iterator rend() const noexcept { return reverse_iterator(begin); }
	const_iterator cbegin() const noexcept { return start; }
	const_iterator cend() const noexcept { return finish; }
	const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(finish); }
	const_reverse_iterator crend() const noexcept { return const_reverse_iterator(start);  }


public:
// capacity
    bool empty() const noexcept { return start == finish; }
    
    size_type size() const noexcept {
        return static_cast<size_type>(finish - start);
    }

    size_type max_size() const noexcept {
        return SIZE_MAX / sizeof(T);
    }

    void reserve(size_type new_cap) {}
    
    size_type capacity() const noexcept {
        return static_cast<size_type>(end_of_storage - start);
    }

    void shrink_to_fit() {}

public:
// modifiers
    void clear() noexcept {}

    template <class... Args> 
    iterator emplace(const_iterator pos, Args&&... args);
    
    iterator erase(const_iterator pos);

    iterator erase(const_iterator first, const_iterator last);

    void push_back(const T& val) {}

    void push_back(T&& val) {}

    template <class... Args>
    void emplace_back(Args&&... args) {}

    void pop_back() {} 

    void resize(size_type count);
    
    void resize(size_type count, const value_type& value);

    void swap(vector& other);

public:
    iterator insert (const_iterator pos, const value_type& val);
    	
    iterator insert (const_iterator pos, size_type n, const value_type& val);
    
    template <class InputIt>
    iterator insert (const_iterator pos, InputIt first, InputIt last);
    
    iterator insert (const_iterator pos, value_type&& val);

    iterator insert (const_iterator pos, initializer_list<value_type> il);
};

    template<class T, class Alloc>
    bool operator==(const vector<T,Alloc>& lhs, const vector<T,Alloc>& rhs);


    template<class T, class Alloc>
    bool operator!=(const vector<T,Alloc>& lhs, const vector<T,Alloc>& rhs);

    template<class T, class Alloc>
    bool operator<(const vector<T,Alloc>& lhs, const vector<T,Alloc>& rhs);

    template<class T, class Alloc>
    bool operator<=(const vector<T,Alloc>& lhs, const vector<T,Alloc>& rhs);

    template<class T, class Alloc>
    bool operator>(const vector<T,Alloc>& lhs, const vector<T,Alloc>& rhs);

    template<class T, class Alloc>
    bool operator>=(const vector<T,Alloc>& lhs, const vector<T,Alloc>& rhs);

    template< class T, class Alloc >
    void swap(vector<T,Alloc>& lhs, vector<T,Alloc>& rhs);


} // MiniSTL













} // MiniSTL