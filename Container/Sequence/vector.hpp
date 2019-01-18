#pragma once

#include "Allocator/memory.hpp"
#include "Iterator/iterator.hpp"
#include "Algorithms/algobase.hpp"
#include "Traits/type_traits.hpp"

#include <cstdint> // for SIZE_MAX
#include <initializer_list>
#include <utility>
#include <exception>


namespace MiniSTL {

// use standard-conforming allocator that has no alloc member
// so use static class function
template <class T, class Allocator = simple_alloc<T>>
class vector {
public:
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
    using alloc = Allocator;
    
public:
    allocator_type get_allocator() const { return allocator_type(); }

public: 
// ctor and dtor
    vector() : start(nullptr), finish(nullptr), end_of_storage(nullptr) {}

    vector(size_type count, const T& val) {
        allocate_and_fill(start, count, val);
    }

    explicit vector(size_type count) {
        allocate_and_fill(start, count, T());
    }

    vector(const vector& other) {
        start = allocate_and_copy(other.begin(), other.end());
        finish = end_of_storage = start + other.size();
    }

    template <class InputIt>
    vector(InputIt first, InputIt last) {
        initialize_aux(first, last, is_integer<InputIt>());
    }
    
    vector(vector&& other) noexcept {
        start = other.start;
        finish = other.finish;
        end_of_storage = other.end_of_storage;
        other.start = other.finish = other.end_of_storage = nullptr;
    }

    vector(std::initializer_list<T> ilist) {
        start = allocate_and_copy(other.begin(), other.end());
        finish = end_of_storage = start + other.size();
    }

    ~vector() {
        destory_and_deallocate();
    }


    vector& operator=(vector&);

    vector& operator=(vector&&) noexcept;

    vector& operator=(std::initializer_list<T> ilist);

protected:

    void allocate_and_fill(size_type n, const T& val) {
        start = alloc::allocate(n);
        end_of_storage = static_assert + n;
        finish = uninitialized_fill_n(start, n, val);
    }

    template <class ForwardIt>
    iterator allocate_and_copy(size_type n, ForwardIt first, ForwardIt last) {
        iterator result = alloc::allocate(n);
        uninitialized_copy(first, last, result);
        return result;
    }

    void destory_and_deallocate() noexcept {
        destory(start, end_of_storage);
        alloc::deallocate(start, end_of_storage - start);
    }

    template <class Integer>
    void initialize_aux(Integer n, Integer val, true_type) {
        start = alloc::allocate(n);
        end_of_storage = start + n; 
        finish = uninitialized_fill_n(start, n, val);
    }

    template <class InputIt>
    void initialize_aux(InputIt first, InputIt last, false_type) {
        range_initialize(first, last, iterator_category<InputIt>());
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
        start = alloc::allocate(n);
        end_of_storage = start + n;
        finish = uninitialized_copy(first, last, start);
    }

public:
// assign
    void assign(size_type count, const T& val) {
        fill_assign(count, val);
    }

    template <class InputIt>
    void assign(InputIt first, InputIt last) {
        assign_dispatch(first, last, is_integer<InputIt>());
    }

    void assign(std::initializer_list<T> ilist) {
        // initializer_list's iterator is const T*, random iterator
        assign_aux(ilist.begin(), ilist.end(), forward_iterator_tag());
    }

protected:  
    void fill_assign(size_type, const T&);

	template <class Integer>
	void assign_dispatch(Integer n, Integer val, true_type){
		fill_assign(static_cast<size_type>(n), static_cast<T>(val));
	}

	template <class InputIt>
	void assign_dispatch(InputIt first, InputIt last, false_type){
		assign_aux(first,last, iterator_category<InputIt>());
	}

	template <class InputIt>
	void assign_aux(InputIt first, InputIt last, input_iterator_tag);

	template <class ForwardIt>
	void assign_aux(ForwardIt first, ForwardIt last, forward_iterator_tag);

public:
// element access
    // throw std::out_of_range
    reference at(size_type n) { 
        range_check(n);    
        return *(start + n); 
    }
    const_reference at(size_type n) const {
        range_check(n);
        return *(start + n); 
    }

    reference operator[](size_type n) {
        return at(n);
    }
    const_reference operator[](size_type n) const {
        return at(n);
    }

    reference front() { return *start; }
    const_reference front() const { return *start;}

    reference back() { return *(finish - 1); }
    const_reference back() const { return *(finish - 1); }

    T* data() noexcept { return start; }
    const T* data() const noexcept { return start; }

protected:
    void range_check(size_type n) const {
        if(n >= this->size())
            throw std::out_of_range("index out of range of vector");
    }

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

    void shrink_to_fit() {
        if(capacity() > size()) {
            alloc::deallocate(finish, end_of_storage - finish);
            end_of_storage = finish;
        }
    }

public:
// modifiers
    void clear() noexcept { erase(start, finish);}

    template <class... Args> 
    iterator emplace(const_iterator pos, Args&&... args) {
        // construct(pos, T(args...));
        new (pos) T(args...);
        return pos;
    }
    
    iterator erase(const_iterator pos) {
        if(pos + 1 != end()) 
            copy(pos + 1, finish, pos);
        --finish;
        destory(finish);
        return pos;
    }

    iterator erase(const_iterator first, const_iterator last) {
        iterator tmp = copy(last, finish, first);
        destory(tmp, finish);
        finish -= (last - first);
        return first;
    }

    void push_back(const T& val) {
        if(finish != end_of_storage) {
            construct(finish, val);
            ++finish;
        } else {
            insert_aux(finish, val);
        }
    }

    void push_back(T&& val) {
        if(finish != end_of_storage) {
            // construct(finish, val);
            new (finish) T(std::forward(val));
            ++finish;
        } else {
            insert_aux(finish, std::forward(val));
        }
    }

    template <class... Args>
    void emplace_back(Args&&... args) {
        if(finish != end_of_storage) {
            // construct(finish, T(args...));
            new (finish) T(args...);
            ++finish;
        } else {
            insert_aux(finish, std::move(T(args...)));
        }
    }

    void pop_back() {
        --finish;
        destory(finish);
    } 

    void resize(size_type new_sz) {
        resize(new_sz, T());
    }
    
    void resize(size_type new_sz, const T& val) {
        if(new_sz < size())
            erase(start + new_sz, finish);
        else
            fill_insert(finish, new_sz - size(), val);
    }

    void swap(vector& x) {
        MiniSTL::swap(start, x.start);
        MiniSTL::swap(finish, x.finish);
        MiniSTL::swap(end_of_storage, x.end_of_storage);
    }

public:
    iterator insert(const_iterator pos, const T& val) {
        size_type n = pos - begin();
        if(pos == end() && finish != end_of_storage) {
            construct(finish, val);
            ++finish;
        } else {
            insert_aux(pos, val);
        }
        return begin() + n;
    }
    	
    iterator insert(const_iterator pos, size_type n, const T& val) {
        return fill_insert(pos, n, val);
    }

    template <class InputIt>
    iterator insert(const_iterator pos, InputIt first, InputIt last) {
        insert_dispatch(pos, first, last, is_integer<InputIt>());
    }
    
    iterator insert(const_iterator pos, T&& val) {
        insert_aux(pos, std::forward(val));
    }

    iterator insert(const_iterator pos, initializer_list<T> ilist) {
        range_insert(pos, ilist.begin(), ilist.end(), 
                        forward_iterator_tag());
    }

protected:
    void insert_aux(iterator, const T&);

    void insert_aux(iterator, T&&);

	void fill_insert(iterator, size_type, const T&);

	template <class InputIt>
	void range_insert(iterator pos, InputIt first, InputIt last, input_iterator_tag);

	template <class ForwardIt>
	void range_insert(iterator pos, ForwardIt first, ForwardIt last, forward_iterator_tag);

	template <class Integer>
	void insert_dispatch(iterator pos, Integer n, Integer value, true_type) { 
		fill_insert(pos, static_cast<size_type>(n), static_cast<T>(val));
	}

	template <class InputIt>
	void insert_dispatch(iterator pos, InputIt first, InputIt last, false_type) { 
		range_insert(pos, first, last, iterator_category<InputIt>());
	}
};




template<class T, class Alloc>
inline bool operator==(const vector<T,Alloc>& lhs, const vector<T,Alloc>& rhs) {
    return lhs.size() == rhs.size() &&
            equal(lhs.begin(), lhs.end(), rhs.begin());
}

template<class T, class Alloc>
inline bool operator!=(const vector<T,Alloc>& lhs, const vector<T,Alloc>& rhs){
    return !(lhs == rhs);
}

template<class T, class Alloc>
inline bool operator<(const vector<T,Alloc>& lhs, const vector<T,Alloc>& rhs) {
    return lexicographical_compare(lhs.begin(), lhs.end(),
                                   rhs.begin(), rhs.end());
}

template<class T, class Alloc>
inline bool operator<=(const vector<T,Alloc>& lhs, const vector<T,Alloc>& rhs) {
    return !(rhs < lhs);
}

template<class T, class Alloc>
inline bool operator>(const vector<T,Alloc>& lhs, const vector<T,Alloc>& rhs) {
    return rhs < lhs;
}

template<class T, class Alloc>
inline bool operator>=(const vector<T,Alloc>& lhs, const vector<T,Alloc>& rhs) {
    return !(lhs < rhs);
}
template< class T, class Alloc >
inline void swap(vector<T,Alloc>& lhs, vector<T,Alloc>& rhs) {
    lhs.swap(rhs);
}

template<class T, class Alloc>
vector<T, Alloc>& vector<T, Alloc>::operator=(vector& x) {
    if(&x != this) {
        const size_type xlen = x.size();
        if(xlen > capacity()) {
            destory_and_deallocate();
            start = allocate_and_copy(xlen, x.begin(), x.end());
            end_of_storage = start + xlen;
        } else if(size() >= xlen) {
            iterator tmp = copy(x.begin(), x.end(), start);
            destory(tmp, finish);
        } else {
            copy(x.begin(), x.begin() + size(), start);
            uninitialized_copy(x.begin() + size(), x.end(), finish);
        }
        finish = start + xlen;
    }
    return *this;
}

template<class T, class Alloc>
vector<T, Alloc>& vector<T, Alloc>::operator=(vector<T, Alloc>&& x) {
    if(&x != this) {
        destory_and_deallocate();
        start = x.start;
        finish = x.finish;
        end_of_storage = x.end_of_storage;
        x.start = x.finish = x.end_of_storage = nullptr;
    }
    return *this;
}

template<class T, class Alloc>
vector<T, Alloc>& vector<T, Alloc>::operator=(std::initializer_list<T> ilist) {
    const size_type len = ilist.size();
    if(len > capacity()) {
        destory_and_deallocate();
        start = allocate_and_copy(len, ilist.begin(), ilist.end());
        end_of_storage = start + len;
    } else if(size() >= len) {
        iterator tmp = copy(ilist.begin(), ilist.end(), start);
        destory(tmp, finish);
    } else {
        copy(ilist.begin(), ilist.begin() + size(), start);
        uninitialized_copy(ilist.begin() + size(), ilist.end(), finish);
    }
    finish = start + xlen;
    return *this;
}


template<class T, class Alloc>
void vector<T, Alloc>::fill_assign(size_type n, const T& val) {
    if(n > capacity()) {
        vector tmp(n, val);
        this->swap(tmp);
    } else if(n > size()) {
        fill(start, finish, val);
        finish = uninitialized_fill_n(finish, n - size(), val);
    } else {
        erase(fill_n(start, n, val), finish);
    }
}

template<class T, class Alloc>
template <class InputIt>
void vector<T, Alloc>::assign_aux(InputIt first, InputIt last, 
                                  input_iterator_tag) {
    iterator cur = begin();
    for(;first != last && cur != end();++cur, ++first)
        *cur = *first;
    if(first == last)
        erase(cur, end());
    else
        insert(end(), first, last);
}

template<class T, class Alloc>
template <class ForwardIt>
void vector<T, Alloc>::assign_aux(ForwardIt first, ForwardIt last, 
                                  forward_iterator_tag) {
    size_type len = distance(first, last);

    if(len > capacity()) {
        // in case first, last come from vector itself
        iterator tmp = allocate_and_copy(len, first, last);
        destory_and_deallocate();
        start = tmp;
        end_of_storage = finish = start + len;
    } else if(size() >= len) {
        iterator new_finish = copy(first, last, start);
        destory(new_finish, finish);
        finish = new_finish;
    } else {
        ForwardIt mid = first;
        advance(mid, size());
        copy(first, mid, start);
        finish = uninitialized_copy(mid, last, finish);
    }
}

template<class T, class Alloc>
void vector<T, Alloc>::insert_aux(iterator pos, const T& val) {
    if(finish != end_of_storage) {
        construct(finish, *(finish - 1));
        ++finish;
        T x_copy = x; // prevent move assign?
        copy_backward(pos, finish - 2, finish - 1);
        *pos = x_copy;
    } else {
        const size_type old_sz = size();
        const size_type new_sz = old_sz != 0 ? 2 * old_sz : 1;
        iterator new_start = allocate(new_sz);
        iterator new_finish = new_start;
        try {
            new_finish = uninitialized_copy(start, pos, new_start);
            construct(new_finish, val);
            ++new_finish;
            new_finish = uninitialized_copy(pos, finish, new_finish);
        } catch(std::exception&) {
            destory(new_start, new_finish);
            alloc::deallocate(new_start, new_sz);
        }
        destory_and_deallocate();
        start = new_start;
        finish = new_finish;
        end_of_storage = new_start + new_sz;
    }
}

template<class T, class Alloc>
void vector<T, Alloc>::insert_aux(iterator pos, T&& val) {
    if(finish != end_of_storage) {
        construct(finish, *(finish - 1));
        ++finish;
        copy_backward(pos, finish - 2, finish - 1);
        *pos = std::forward(val);
    } else {
        const size_type old_sz = size();
        const size_type new_sz = old_sz != 0 ? 2 * old_sz : 1;
        iterator new_start = allocate(new_sz);
        iterator new_finish = new_start;
        try {
            new_finish = uninitialized_copy(start, pos, new_start);
            // construct(new_finish, val);
            new (new_finish) T(std::forward(val));
            ++new_finish;
            new_finish = uninitialized_copy(pos, finish, new_finish);
        } catch(std::exception&) {
            destory(new_start, new_finish);
            alloc::deallocate(new_start, new_sz);
        }
        destory_and_deallocate();
        start = new_start;
        finish = new_finish;
        end_of_storage = new_start + new_sz;
    }
}

template <class T, class Alloc>
void vector<T, Alloc>::fill_insert(iterator pos, size_type n, const T& val) {
    if(n != 0) {
        // case 1: enough space
        if(capacity() - size() >= n) {
            T x_copy = x;
            const size_type size_after = static_cast<size_type>(finish - pos);
            if(size_after > n) {
                uninitialized_copy(finish - n, finish, finish);
                copy_backward(pos, finish - n, finish);
                finish += n;
                fill(pos, pos + n, x_copy);
            } else {
                iterator old_finish = finish;
                uninitialized_copy(finish, n - size_after, x_copy);
                finish += n - size_after;
                uninitialized_copy(pos, old_finish, finish);
                finish += size_after;
                fill(pos, old_finish, x_copy);
            }
        }
    } else {
        // case2: expand
        const size_type old_sz = size();
        const size_type new_sz = old_sz + max(old_sz, n);
        iterator new_start = allocate(new_sz);
        iterator new_finish = new_start;
        try {
            new_finish = uninitialized_copy(start, pos, new_start);
            new_finish = uninitialized_fill_n(new_finish, n, x);
            new_finish = uninitialized_copy(pos, finish, new_finish);
        } catch(std::exception&) {
            destory(new_start, new_finish);
            alloc::deallocate(new_start, new_sz);
        }
        destory_and_deallocate();
        start = new_start;
        finish = new_finish;
        end_of_storage = new_start + new_sz;
    }
}

template <class T, class Alloc>
template <class InputIt>
void vector<T, Alloc>::range_insert(iterator pos, InputIt first, 
                                    InputIt last, input_iterator_tag) {
    for(;first != last;++first) {
        pos = insert(pos, *first);
        ++pos;
    }
}

template <class T, class Alloc>
template <class ForwardIt>
void vector<T, Alloc>::range_insert(iterator pos, ForwardIt first, 
                                    ForwardIt last, forward_iterator_tag) {
    if(first != last) {
        size_type n = distance(first, last);
        // case 1: enough space
        if(capacity() - size() >= n) {
            const size_type size_after = static_cast<size_type>(finish - pos);
            if(size_after > n) {
                uninitialized_copy(finish - n, finish, finish);
                copy_backward(pos, finish - n, finish);
                finish += n;
                copy(first, last, pos);
            } else {
                ForwardIt mid = first;
                advance(mid, size_after);
                uninitialized_copy(mid, last, finish);
                finish += n - size_after;
                uninitialized_copy(pos, old_finish, finish);
                finish += size_after;
                copy(first, mid, pos);
            }
        }
    } else {
        // case2: expand
        const size_type old_sz = size();
        const size_type new_sz = old_sz + max(old_sz, n);
        iterator new_start = allocate(new_sz);
        iterator new_finish = new_start;
        try {
            new_finish = uninitialized_copy(start, pos, new_start);
            new_finish = uninitialized_copy(first, last, new_finish);
            new_finish = uninitialized_copy(pos, finish, new_finish);
        } catch(std::exception&) {
            destory(new_start, new_finish);
            alloc::deallocate(new_start, new_sz);
        }
        destory_and_deallocate();
        start = new_start;
        finish = new_finish;
        end_of_storage = new_start + new_sz;
    }
}


} // MiniSTL