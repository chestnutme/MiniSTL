/*
 * Deque design:
 *      1. a deque consists of a vector of fixed-size bufs, called map;
 *      2. each buf store a fixed number of nodes, each nodes store a element;
 * 
 * For any nonsingular iterator i:
 *      i.node is the address of an element in the map array.  The
 *        contents of i.node is a pointer to the beginning of a node.
 *      i.first == *(i.node) 
 *      i.last  == i.first + node_size
 *      i.cur is a pointer in the range [i.first, i.last).  NOTE:
 *        the implication of this is that i.cur is always a dereferenceable
 *        pointer, even if i is a past-the-end iterator.
 *  Start and Finish are always nonsingular iterators.  NOTE: this means
 *      that an empty deque must have one node, and that a deque
 *      with N elements, where N is the buffer size, must have two nodes.
 *  For every node other than start.node and finish.node, every element
 *      in the node is an initialized object.  If start.node == finish.node,
 *      then [start.cur, finish.cur) are initialized objects, and
 *      the elements outside that range are uninitialized storage.  Otherwise,
 *      [start.cur, start.last) and [finish.first, finish.cur) are initialized
 *      objects, and [start.first, start.cur) and [finish.cur, finish.last)
 *      are uninitialized storage.
 *  [map, map + map_size) is a valid, non-empty range.  
 *  [start.node, finish.node] is a valid range contained within 
 *      [map, map + map_size).  
 *  A pointer in the range [map, map + map_size) points to an allocated node
 *      if and only if the pointer is in the range [start.node, finish.node].
 */ 

#pragma once

#include "Allocator/memory.hpp"
#include "Iterator/iterator.hpp"
#include "Algorithms/algobase.hpp"
#include "Traits/type_traits.hpp"

#include <cstddef>
#include <initializer_list>
#include <utility>
#include <exception>

namespace MiniSTL {

// set number of elements in buf
inline size_t deque_buf_size(size_t sz) {
	return sz < 512 ? size_t(512 / sz) : size_t(1);
}

template<class T, class Ref, class Ptr>
struct deque_iterator {
	// alias declarartions
	using iterator = deque_iterator<T, T&, T*>;
	using const_iterator = deque_iterator<T, const T&, const T*>;
	
	using iterator_category = random_access_iterator_tag;
	using value_type = T;
	using pointer = Ptr;
	using reference = Ref;
	using size_type = size_t;
	using difference_type = ptrdiff_t;
	using map_pointer = T**;

    using self = deque_iterator;

    static size_t buffer_size() { return deque_buf_size(sizeof(T)); }

	// data member
	T* cur;
	T* first; 
	T* last;
	map_pointer node;

	// ctor
	deque_iterator() 
        : cur(nullptr), first(nullptr), last(nullptr), node(nullptr) {}
	deque_iterator(pointer x, map_pointer y) 
        : cur(x), first(*y), last(*y + buffer_size()), node(y) {}
	deque_iterator(const iterator& i) 
        : cur(i.cur), first(i.first), last(i.last), node(i.node) {}

	void set_node(map_pointer new_node) {
		node = new_node;
		first = *new_node;
		last = first + static_cast<difference_type>(buffer_size());
	}

	reference operator*() const { return *cur; }
	pointer operator->() const { return &(operator*()); }

	self& operator++() {
		++cur;
		if (cur == last) {
			set_node(node + 1);
			cur = first;
		}
		return *this;
	}

	self operator++(int) {
		self temp = *this;
		++*this;
		return temp;
	}

	self& operator--() {
		if (cur == first) {
			set_node(node - 1);
			cur = last;
		}
		--cur;
		return *this;
	}

	self operator--(int) {
		self temp = *this;
		--*this;
		return temp;
	}

	self& operator+=(difference_type n) {
		difference_type offset = n + (cur - first);
		if (offset >= 0 && offset <static_cast<difference_type>(buffer_size())) {
			cur += n;
		} else {
			difference_type node_offset = offset > 0 ? 
                offset / static_cast<difference_type>(buffer_size()) : 
                -static_cast<difference_type>((-offset - 1) / buffer_size()) - 1;
			set_node(node + node_offset);
			cur = first + (offset - node_offset * static_cast<difference_type>(buffer_size()));
		}
		return *this;
	}

	self operator+(difference_type n) {
		self temp = *this;
		return temp += n;
	}

	self& operator-=(difference_type n) {
		return *this += -n;
	}

	self operator-(difference_type n) {
		self temp = *this;
		return temp -= n;
	}

	reference operator[](difference_type n) {
		return *(*this + n);
	}

    bool operator==(const self& x) const { return cur == x.cur; }
    bool operator!=(const self& x) const { return cur != x.cur; }
    bool operator<(const self& x) const 
        { return node == x.node ? cur < x.cur : node < x.node; }
    bool operator>(const self& x) const { return x < *this; }
    bool operator<=(const self& x) const { return !(x < *this); }
    bool operator>=(const self& x) const { return !(*this < x); }
};

template<class T, class Ref, class Ptr>
inline deque_iterator<T, Ref, Ptr> operator+(ptrdiff_t n, const deque_iterator<T, Ref, Ptr>& x){
	return x + n;
}

// distance between x, y, that is x - y
template<class T, class Ref, class Ptr>
inline typename deque_iterator<T, Ref, Ptr>::difference_type
operator-(const deque_iterator<T, Ref, Ptr>& x, const deque_iterator<T, Ref, Ptr>& y){
	return typename deque_iterator<T, Ref, Ptr>::difference_type(
        deque_iterator<T, Ref, Ptr>::buffer_size() * (x.node - y.node -1) +
		(x.cur - x.first) + (y.last - y.cur));
}




template <class T, class Allocator = simple_alloc<T> >
class deque {
public:
    // types:
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
    using reverse_iterator	= __reverse_iterator<iterator>
    using const_reverse_iterator = __reverse_iterator<const_iterator>
 
    // construct/copy/destroy:
    explicit deque(const Allocator& = Allocator());
    explicit deque(size_type n);
    deque(size_type n, const T& value,const Allocator& = Allocator());
    template <class InputIterator>
        deque(InputIterator first, InputIterator last,const Allocator& = Allocator());
    deque(const deque<T,Allocator>& x);
    deque(deque&&);
    deque(const deque&, const Allocator&);
    deque(deque&&, const Allocator&);
    deque(initializer_list<T>, const Allocator& = Allocator());
 
    ~deque();
    deque<T,Allocator>& operator=(const deque<T,Allocator>& x);
    deque<T,Allocator>& operator=(deque<T,Allocator>&& x);
    deque& operator=(initializer_list<T>);
    template <class InputIterator>
    void assign(InputIterator first, InputIterator last);
    void assign(size_type n, const T& t);
    void assign(initializer_list<T>);
    allocator_type get_allocator() const noexcept;
 
    // iterators:
    iterator                begin() noexcept;
    const_iterator          begin() const noexcept;
    iterator                end() noexcept;
    const_iterator          end() const noexcept;
 
    reverse_iterator        rbegin() noexcept;
    const_reverse_iterator  rbegin() const noexcept;
    reverse_iterator        rend() noexcept;
    const_reverse_iterator  rend() const noexcept;
 
    const_iterator          cbegin() noexcept;
    const_iterator          cend() noexcept;
    const_reverse_iterator  crbegin() const noexcept;
    const_reverse_iterator  crend() const noexcept;
 
    // capacity:
    size_type size() const noexcept;
    size_type max_size() const noexcept;
    void      resize(size_type sz);
    void      resize(size_type sz, const T& c);
    void      shrink_to_fit();
    bool      empty() const noexcept;
 
    // element access:
    reference       operator[](size_type n);
    const_reference operator[](size_type n) const;
    reference       at(size_type n);
    const_reference at(size_type n) const;
    reference       front();
    const_reference front() const;
    reference       back();
    const_reference back() const;
 
    // modifiers:
    template <class... Args> void emplace_front(Args&&... args);
    template <class... Args> void emplace_back(Args&&... args);
    template <class... Args> iterator emplace(const_iterator position, Args&&... args);
 
    void push_front(const T& x);
    void push_front(T&& x);
    void push_back(const T& x);
    void push_back(T&& x);
 
    iterator insert(const_iterator position, const T& x);
    iterator insert(const_iterator position, T&& x);
    iterator insert(const_iterator position, size_type n, const T& x);
    template <class InputIterator>
        iterator insert (const_iterator position, InputIterator first, 
                         InputIterator last);
    iterator insert(const_iterator position, initializer_list<T>);
 
    void pop_front();
    void pop_back();
 
    iterator erase(const_iterator position);
    iterator erase(const_iterator first, const_iterator last);
    void     swap(deque<T,Allocator>&);
    void     clear() noexcept;
};


template <class T, class Allocator = allocator<T> > class deque;

template <class T, class Allocator>
    bool operator==(const deque<T,Allocator>& x, const deque<T,Allocator>& y);
template <class T, class Allocator>
    bool operator<(const deque<T,Allocator>& x, const deque<T,Allocator>& y);
template <class T, class Allocator>
    bool operator!=(const deque<T,Allocator>& x, const deque<T,Allocator>& y);
template <class T, class Allocator>
    bool operator>(const deque<T,Allocator>& x, const deque<T,Allocator>& y);
template <class T, class Allocator>
    bool operator>=(const deque<T,Allocator>& x, const deque<T,Allocator>& y);
template <class T, class Allocator>
    bool operator<=(const deque<T,Allocator>& x, const deque<T,Allocator>& y);

template <class T, class Allocator>
    void swap(deque<T,Allocator>& x, deque<T,Allocator>& y);

} // MiniSTL