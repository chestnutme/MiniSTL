/*
 * Deque design:
 *      1. a deque consists of a array of fixed-size nodes, called map;
 *      2. each node store a fixed number of elements;
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
#include <cstdint>
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

    static size_t buf_size() { return deque_buf_size(sizeof(T)); }

	// data member
	T* cur; // current element in buf
	T* first; // first element in buf
	T* last;  // behind the last element in buf
	map_pointer node;

	// ctor
	deque_iterator() 
        : cur(nullptr), first(nullptr), last(nullptr), node(nullptr) {}
	deque_iterator(pointer x, map_pointer y) 
        : cur(x), first(*y), last(*y + buf_size()), node(y) {}
    // ! ctor a const_iterator from iterator
	deque_iterator(const iterator& i) 
        : cur(i.cur), first(i.first), last(i.last), node(i.node) {}

	void set_node(map_pointer new_node) {
		node = new_node;
		first = *new_node;
		last = first + static_cast<difference_type>(buf_size());
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
		if (offset >= 0 && offset <static_cast<difference_type>(buf_size())) {
			cur += n;
		} else {
			difference_type node_offset = offset > 0 ? 
                offset / static_cast<difference_type>(buf_size()) : 
                -static_cast<difference_type>((-offset - 1) / buf_size()) - 1;
			set_node(node + node_offset);
			cur = first + (offset - node_offset * static_cast<difference_type>(buf_size()));
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
inline deque_iterator<T, Ref, Ptr> 
operator+(ptrdiff_t n, const deque_iterator<T, Ref, Ptr>& x) {
	return x + n;
}

// distance between x, y, that is x - y
template<class T, class Ref, class Ptr>
inline typename deque_iterator<T, Ref, Ptr>::difference_type
operator-(const deque_iterator<T, Ref, Ptr>& x, 
          const deque_iterator<T, Ref, Ptr>& y){
	return typename deque_iterator<T, Ref, Ptr>::difference_type(
        deque_iterator<T, Ref, Ptr>::buf_size() * (x.node - y.node -1) 
        + (x.cur - x.first) + (y.last - y.cur));
}




template <class T, class Allocator = simple_alloc<T> >
class deque {
public:
    // types:
    using value_type = T;
    using allocator_type = Allocator;
    using size_type	= size_t;
    using difference_type = ptrdiff_t;
    using reference	= T&;
    using const_reference = const T&;
    using pointer = T*;
    using const_pointer = const T*;
    using iterator = deque_iterator<T, T&, T*>;
    using const_iterator = deque_iterator<T, const T&, const T*>;
    using reverse_iterator	= __reverse_iterator<iterator>;
    using const_reverse_iterator = __reverse_iterator<const_iterator>;

protected:
    using map_pointer = T**;
    using node_alloc = simple_alloc<T>;
    using map_alloc = simple_alloc<T*>;

    // data member:
    iterator start;
    iterator finish;
    map_pointer map;
    size_type map_size;

protected:
    enum { INTIALIZE_MAP_SIZE = 8U };

    static size_type buf_size() { return iterator::buf_size(); }

    allocator_type get_allocator() const noexcept 
        { return node_alloc(); }
 
    // node related dynamic alloc
    T* allocate_node() { return node_alloc::allocate(buf_size()); }
    void deallocate_node(T* p) { return node_alloc::deallocate(p, buf_size()); }
    void create_nodes(T** nstart, T** nfinish);
    void destroy_nodes(T** nstart, T** nfinish) {
        for(T** cur = nstart;cur != nfinish;cur++)
            deallocate_node(*cur);
    }

    // map related dynamic alloc
    T** allocate_map(size_type n) { return map_alloc::allocate(n); }
    void deallocate_map(T** p, size_type n) { return map_alloc::deallocate(p, n); }
    void initialize_map(size_type n);
    void reallocate_map(size_type n, bool add_at_front);
    void reserve_map_at_front(size_type n = 1) {
        if(n > static_cast<size_type>(start.node - map))
            reallocate_map(n, true);
    }
    void reserve_map_at_back(size_type n = 1) {
        if(n > map_size - (finish.node - map) - 1)
            reallocate_map(n, false);
    }
    
    // element related dynamic alloc
    iterator reserve_element_at_front(size_type n);
    iterator reserve_element_at_back(size_type n);
    void new_elements_at_front(size_type n);
    void new_elements_at_back(size_type n);

public:
    // construct/copy/destroy:
    explicit deque() 
        : start(), finish(), map(nullptr), map_size(0)
        { initialize_map(0);}
    explicit deque(size_type n) 
        : start(), finish(), map(nullptr), map_size(0)
        { initialize_map(n); }
    deque(size_type n, const T& val) 
        : start(), finish(), map(nullptr), map_size(0) {
        initialize_map(n);
        fill_initialize(val);
    }

    // we cannot get distance [first, last) now, so initialized 
    // map until know the category of iterator
    template <class InputIt>
    deque(InputIt first, InputIt last)
        : start(), finish(), map(nullptr), map_size(0)
        { initialize_dispatch(first, last, integral<InputIt>());}
    deque(const deque& x) 
        : start(), finish(), map(nullptr), map_size(0) { 
        initialize_map(x.size());
        uninitialized_copy(x.begin(), x.end(), start);
    }

    // ? move semantics:
    deque(deque&& x)
        : start(x.start), finish(x.finish), map(x.map), map_size(x.map_size) {
        x.start = iterator();
        x.finish = iterator();
        x.map = nullptr;
        x.map_size = static_cast<size_type>(0);
        // initialize_map(0);
        // swap(x);
    }
    deque(std::initializer_list<T> ilist) 
        : start(), finish(), map(nullptr), map_size(0) {
        range_initialize(ilist.begin(), ilist.end(),
                         forward_iterator_tag()); 
    }
 
    ~deque() {
        destroy(start, finish);
        if(map) {
            destroy_nodes(start.node, finish.node + 1);
            deallocate_map(map, map_size);
        }
    }

    deque& operator=(const deque& x);
    deque& operator=(deque&& x) {
        if(&x != this) {
            clear();
            swap(x);
        }
        return *this;
    }
    deque& operator=(std::initializer_list<T> ilist) {
        assign_aux(ilist.begin(), ilist.end(), forward_iterator_tag());
        return *this;
    }

protected:
    // precondition: must initialize map first
    void fill_initialize(const T& val);
    template <class Integer>
    void initialize_dispatch(Integer n, Integer val, true_type) {
        initialize_map(static_cast<size_type>(n));
        fill_intialize(static_cast<T>(val));
    }

    template <class InputIt>
    void initialize_dispatch(InputIt first, InputIt last, false_type) {
        range_initialize(first, last,                           
                         iterator_category_t<InputIt>());
    }

    template <class InputIt>
    void range_initialize(InputIt first, InputIt last, input_iterator_tag);

    template <class ForwardIt>
    void range_initialize(ForwardIt first, ForwardIt last, forward_iterator_tag);

public:
    template <class InputIt>
    void assign(InputIt first, InputIt last) {
        assign_dispatch(first, last, integral<InputIt>());
    }
    void assign(size_type n, const T& val) {
        fill_assign(n, val);
    }
    void assign(std::initializer_list<T> ilist) {
        assign_aux(ilist.begin(), ilist.end(), forward_iterator_tag());
    }
    
protected:
    void fill_assign(size_type n, const T& val);

	template <class Integer>
	void assign_dispatch(Integer n, Integer val, true_type){
		fill_assign(static_cast<size_type>(n), static_cast<T>(val));
	}

	template <class InputIt>
	void assign_dispatch(InputIt first, InputIt last, false_type){
		assign_aux(first,last, iterator_category_t<InputIt>());
	}

	template <class InputIt>
	void assign_aux(InputIt first, InputIt last, input_iterator_tag);

	template <class ForwardIt>
	void assign_aux(ForwardIt first, ForwardIt last, forward_iterator_tag);

public:
    // iterators:
    iterator                begin() noexcept { return start; }
    const_iterator          begin() const noexcept { return start; }
    iterator                end() noexcept { return finish; }
    const_iterator          end() const noexcept { return finish; }
 
    reverse_iterator        rbegin() noexcept 
        { return reverse_iterator(finish); }
    const_reverse_iterator  rbegin() const noexcept 
        { return const_reverse_iterator(finish); }
    reverse_iterator        rend() noexcept
        { return reverse_iterator(start); }
    const_reverse_iterator  rend() const noexcept
        { return const_reverse_iterator(start); }
 
    const_iterator          cbegin() noexcept { return start; }
    const_iterator          cend() noexcept { return finish; }
    const_reverse_iterator  crbegin() const noexcept 
        { return const_reverse_iterator(finish); }
    const_reverse_iterator  crend() const noexcept
        { return const_reverse_iterator(start); }
 
    // capacity:
    size_type size() const noexcept { return finish - start; }
    size_type max_size() const noexcept 
        { return SIZE_MAX / sizeof(T); }
    
    void resize(size_type sz) { resize(sz, T()); }
    void resize(size_type sz, const T& val);
    void shrink_to_fit();
    bool empty() const noexcept { return start == finish; }

protected:
    void range_check(size_type n) {
        if(n > size())
            throw std::out_of_range("deque index out of range");
    }

public:
    // element access:
    // [] with no bound check, at check
    reference       operator[](size_type n)
        { return start[static_cast<difference_type>(n)]; }
    const_reference operator[](size_type n) const
        { return start[static_cast<difference_type>(n)]; }
    reference       at(size_type n)
        { range_check(n); return (*this)[n]; }
    const_reference at(size_type n) const
         { range_check(n); return (*this)[n]; }
    reference       front() { return *start; }
    const_reference front() const { return *start; }
    reference       back() {
        iterator tmp = finish;
        --tmp;
        return *tmp;
    }
    const_reference back() const {
        iterator tmp = finish;
        --tmp;
        return *tmp;
    }

public:
    // modifiers:
    template <class... Args> 
    void emplace_front(Args&&... args) {
        if(start.cur != start.first) {
            new (start.cur - 1) T(args...);
            --start.cur;
        } else 
            push_front_aux(std::move(T(args...)));
    }
    template <class... Args> 
    void emplace_back(Args&&... args) {
        if(finish.cur != finish.last - 1) {
            new (finish.cur) T(args...);
            ++finish.cur;
        } else 
            push_back_aux(std::move(T(args...)));
    }
    template <class... Args> 
    iterator emplace(const_iterator pos, Args&&... args) {
        insert(pos, std::move(T(args...)));
    }
 
    void push_front(const T& val) {
        if(start.cur != start.first) {
            construct(start.cur - 1, val);
            --start.cur;
        } else 
            push_front_aux(val);
    }
    void push_front(T&& val) {
        if(start.cur != start.first) {
            construct(start.cur - 1, std::forward(val));
            --start.cur;
        } else 
            push_front_aux(std::forward(val));
    }
    void push_back(const T& val) {
        if(finish.cur != finish.last - 1) {
            construct(finish.cur, val);
            ++finish.cur;
        } else 
            push_back_aux(val);
    }
    void push_back(T&& val) {
        if(finish.cur != finish.last - 1) {
            construct(finish.cur, std::forward(val));
            ++finish.cur;
        } else 
            push_back_aux(std::forward(val));
    }
    void pop_front() {
        if(start.cur != start.last - 1) {
            destroy(start.cur);
            ++start.cur;
        } else
            pop_front_aux();
    }
    void pop_back() {
        if(finish.cur != finish.start) {
            --finish.cur;
            destroy(finish.cur);
        } else
            pop_back_aux();
    }

protected:
    void push_front_aux(const T& val);
    void push_front_aux(T&& val);
    void push_back_aux(const T& val);
    void push_back_aux(T&& val);
    void pop_front_aux();
	void pop_back_aux();
	
public:
    iterator insert(const_iterator pos, const T& val);
    iterator insert(const_iterator pos, T&& val);
    iterator insert(const_iterator pos, size_type n, const T& val) {
        return fill_insert(pos, n, val);
    }
    template <class InputIt>
    iterator insert(const_iterator pos, InputIt first, InputIt last) {
        return insert_dispatch(pos, first, last, integral<InputIt>());
    }
    iterator insert(const_iterator pos, 
                    std::initializer_list<T> ilist) {
        return range_insert(pos, ilist.begin(), ilist.end(),
                            forward_iterator_tag());
    }

protected:
    iterator insert_aux(const_iterator pos, const T& val);
    iterator insert_aux(const_iterator pos, T&& val);
    
	iterator fill_insert(const_iterator pos, size_type n, const T& val);
    iterator fill_insert_aux(const_iterator pos, size_type n, const T& val);

    template <class Integer>
	iterator insert_dispatch(const_iterator pos, Integer n, 
                             Integer val, true_type) { 
		return fill_insert(pos, static_cast<size_type>(n), static_cast<T>(val));
	}

	template <class InputIt>
	iterator insert_dispatch(const_iterator pos, InputIt first, 
                             InputIt last, false_type) { 
		return range_insert(pos, first, last, iterator_category_t<InputIt>());
	}

    template <class ForwardIt>
    iterator range_insert_aux(const_iterator pos, ForwardIt first, 
                              ForwardIt last, size_type n);
	template <class InputIt>
	iterator range_insert(const_iterator pos, InputIt first, 
                          InputIt last, input_iterator_tag);

	template <class ForwardIt>
	iterator range_insert(const_iterator pos, ForwardIt first, 
                          ForwardIt last, forward_iterator_tag);

public: 
    iterator erase(const_iterator pos);
    iterator erase(const_iterator first, const_iterator last);

    void     swap(deque& x) {
        MiniSTL::swap(start, x.start);
        MiniSTL::swap(finish, x.finish);
        MiniSTL::swap(map, x.map);
        MiniSTL::swap(map_size, x.map_size);
    }

    void     clear() noexcept;
};


template <class T, class Alloc>
bool operator==(const deque<T,Alloc>& x, const deque<T,Alloc>& y) {
    return x.size() == y.size() && 
        equal(x.begin(), x.end(), y.begin());
}

template <class T, class Alloc>
bool operator<(const deque<T,Alloc>& x, const deque<T,Alloc>& y) {
    return lexicographical_compare(x.begin(), x.end(),
                                   y.being(), y.end());
}

template <class T, class Alloc>
bool operator!=(const deque<T,Alloc>& x, const deque<T,Alloc>& y) {
    return !(x == y);
}

template <class T, class Alloc>
bool operator>(const deque<T,Alloc>& x, const deque<T,Alloc>& y) { 
    return y < x;
}

template <class T, class Alloc>
bool operator>=(const deque<T,Alloc>& x, const deque<T,Alloc>& y) {
    return !(x < y);
}
template <class T, class Alloc>
bool operator<=(const deque<T,Alloc>& x, const deque<T,Alloc>& y) {
    return !(y > x);
}

template <class T, class Alloc>
void swap(deque<T,Alloc>& x, deque<T,Alloc>& y) {
    x.swap(y);
}

template <class T, class Alloc>
void deque<T, Alloc>::create_nodes(T** nstart, T** nfinish) {
    T** cur;
    try {
        for(cur = nstart;cur != nfinish;cur++)
            *cur = allocate_node();
    } catch(std::exception&) {
        destroy_nodes(nstart, nfinish);
        throw;
    }
}

// postcondition: [start, finish) = n
template <class T, class Alloc>
void deque<T, Alloc>::initialize_map(size_type n) {
    size_type num_nodes = n / buf_size() + 1;
    // * 2 : reserve enough map space for future use
    map_size = max(static_cast<size_type>(INTIALIZE_MAP_SIZE), 
                   num_nodes * 2);
    map = allocate_map(map_size);
    T** nstart = map + (map_size - num_nodes) / 2;
    T** nfinish = nstart + num_nodes;

    try {
        create_nodes(nstart, nfinish);
    } catch(std::exception&) {
        deallocate_map(map, map_size);
        map = nullptr;
        map_size = 0;
        throw;
    }
    start.set_node(nstart);
    start.cur = start.first;
    finish.set_node(nfinish - 1);
    finish.cur = finish.first + n % buf_size();
}


// note reallocate_map don't allocate new nodes
template <class T, class Alloc>
void deque<T, Alloc>::reallocate_map(size_type n, bool add_at_front) {
    size_type old_num_nodes = finish.node - start.node + 1;
    size_type new_num_nodes = old_num_nodes + n;

    T** new_start;
    if(map_size > 2 * new_num_nodes) {
        // enough map space, move used nodes to the center of map
        new_start = map + (map_size - new_num_nodes) / 2 +
                    (add_at_front ? n : 0);
        if(new_start < start.node)
            copy(start.node, finish.node + 1, new_start);
        else 
            copy_backward(start.node, finish.node + 1,
                          new_start + new_num_nodes);
    } else {
        // not enough space, reallocate map
        size_type new_map_size = map_size + max(map_size, n) + 2;
        map_pointer new_map = allocate_map(new_map_size);
        new_start = new_map_size + 
                    (new_map_size - new_num_nodes) / 2 +
                    (add_at_front ? n : 0);
        copy(start.node, finish.node + 1, new_start);
        deallocate_map(map, map_size);
        map = new_map;
        map_size = new_map_size;
    }
    
    start.set_node(new_start);
    finish.set_node(new_start + old_num_nodes - 1);
}


template <class T, class Alloc>
typename deque<T, Alloc>::iterator 
deque<T, Alloc>::reserve_element_at_front(size_type n) {
    size_type vacums = start.cur - start.first;
    if(n > vacums)
        new_elements_at_front(n - vacums);
    return start - static_cast<difference_type>(n);
}

template <class T, class Alloc>
typename deque<T, Alloc>::iterator 
deque<T, Alloc>::reserve_element_at_back(size_type n) {
    size_type vacums = finish.last - finish.cur - 1;
    if(n > vacums)
        new_elements_at_back(n - vacums);
    return finish + static_cast<difference_type>(n);
}

template <class T, class Alloc>
void deque<T, Alloc>::new_elements_at_front(size_type n) {
    size_type new_nodes = (n + buf_size() - 1) / buf_size();
    reserve_map_at_front(new_nodes);
    size_type i;
    try {
        for(i = 1;i <= new_nodes;++i)
            *(start.node - i) = allocate_node();
    } catch(std::exception&) {
        for(size_type j = 1;j < i;++j)
            deallocate_node(*(start - j));
        throw;
    }
}

template <class T, class Alloc>
void deque<T, Alloc>::new_elements_at_back(size_type n) {
    size_type new_nodes = (n + buf_size() - 1) / buf_size();
    reserve_map_at_back(new_nodes);
    size_type i;
    try {
        for(i = 1;i <= new_nodes;++i)
            *(finish.node + i) = allocate_node();
    } catch(std::exception&) {
        for(size_type j = 1;j < i;++j)
            deallocate_node(*(finish.node + j));
        throw;
    }
}

template <class T, class Alloc>
deque<T, Alloc>& deque<T, Alloc>::operator=(const deque& x) {
    if(&x != this) {
        if(size() >= x.size()) {
            erase(copy(x.begin(), x.end(), start), finish);
        } else {
            const_iterator mid = x.begin() +                
                static_cast<difference_type>(size());
            copy(x.begin(), mid, start);
            insert(finish, mid, end());
        }
    }
}

template <class T, class Alloc>
void deque<T, Alloc>::fill_initialize(const T& val) {
    T** cur;
    try {
        // fill every node before finish
        for(cur = start.node;cur != finish.node;++cur)
            uninitialized_fill(*cur, *cur + buf_size(), val);
        // fill finish
        uninitialized_fill(finish.start, finish.cur, val);
    } catch(std::exception&) {
        // not clear(), because clear destroy [start, finish)
        destroy(start, iterator(*cur, cur));
        throw;
    }
}

// for input iterator, we just push back one by one
template <class T, class Alloc>
template <class InputIt>
void deque<T, Alloc>::range_initialize(InputIt first, InputIt last, 
                                       input_iterator_tag) {
    initialize_map(0);
    try {
        for(;first != last;first++)
            push_back(*first);
    } catch(std::exception&) {
        clear();
        throw;
    }
}

// for forward iter, we copy each node one by one
template <class T, class Alloc>
template <class ForwardIt>
void deque<T, Alloc>::range_initialize(ForwardIt first, ForwardIt last, 
                                       forward_iterator_tag) {
    size_type n = distance(first, last);
    initialize_map(n);

    T** cur;
    try {
        for(cur = start.node; cur != finish.node;++cur) {
            ForwardIt mid = first;
            advance(mid, buf_size());
            uninitialized_copy(first, mid, *cur);
            first = mid;
        }
        uninitialized_copy(first, last, finish.first);
    } catch(std::exception&) {
        destroy(start, iterator(*cur, cur));
        throw;
    }
}


template <class T, class Alloc>
void deque<T, Alloc>::fill_assign(size_type n, const T& val) {
    if(n > size()) {
        fill(begin(), end(), val);
        insert(end(), n - size(), val);
    } else {
        erase(begin() + n, end());
        fill(begin(), end(), val);
    }
}

template <class T, class Alloc>
template <class InputIt>
void deque<T, Alloc>::assign_aux(InputIt first, InputIt last, 
                                 input_iterator_tag) {
    iterator cur = begin();
    for(;cur != end() && first != last;++cur, ++first)
        *cur = *first;
    if(first == last)
        erase(cur, end());
    else
        insert(end(), first, last);
}


template <class T, class Alloc>
template <class ForwardIt>
void deque<T, Alloc>::assign_aux(ForwardIt first, ForwardIt last,
                                 forward_iterator_tag) {
    size_type len = distance(first, last);
    if(len > size()) {
        ForwardIt mid = first;
        advance(mid, len);
        copy(first, mid, begin());
        insert(end(), mid, last);
    } else {
        erase(copy(first, last, begin()), end());
    }
}

template <class T, class Alloc>
void deque<T, Alloc>::resize(size_type sz, const T& val) {
    if(sz > size())
        insert(finish, sz - size(), val);
    else 
        erase(start + static_cast<difference_type>(sz), finish);
}

template <class T, class Alloc>
void deque<T, Alloc>::shrink_to_fit() {
    if(empty()) {
        clear();
    } else {
        T** cur;
        for(cur = map;cur != start.node;++cur)
            deallocate_node(*cur);
        for(cur = finish.node + 1;cur != map + map_size;++cur)
            deallocate_node(*cur);
        map = start.node;
        map_size = finish.node - start.node + 1;
    }
}

template <class T, class Alloc>
void deque<T, Alloc>::push_front_aux(const T& val) {
    T x_copy = val;
    reserve_map_at_front(1);
    *(start.node - 1) = allocate_node();
    try {
        start.set_node(start.node - 1);
        start.cur = start.last - 1;
        construct(start.cur, x_copy);
    } catch(std::exception&) {
        ++start;
        deallocate_node(*(start.node - 1));
        throw;
    }
}

template <class T, class Alloc>
void deque<T, Alloc>::push_front_aux(T&& val) {
    reserve_map_at_front(1);
    *(start.node - 1) = allocate_node();
    try {
        start.set_node(start.node - 1);
        start.cur = start.last - 1;
        new (start.cur) T(std::forward(val));
    } catch(std::exception&) {
        ++start;
        deallocate_node(*(start.node - 1));
        throw;
    }
}

template <class T, class Alloc>
void deque<T, Alloc>::push_back_aux(const T& val) {
    T x_copy = val;
    reserve_map_at_back(1);
    *(finish.node + 1) = allocate_node();
    try {
        construct(finish.cur, x_copy);
        finish.set_node(finish + 1);
        finish.cur = finish.first;
    } catch(std::exception&) {
        deallocate_node(*(finish.node - 1));
        // if exception happens, it can only occur in construct,
        // at time ++finish not happened yet, so don't need below
        // --finish;
        throw;
    }
}

template <class T, class Alloc>
void deque<T, Alloc>::push_back_aux(T&& val) {
    reserve_map_at_back(1);
    *(finish.node + 1) = allocate_node();
    try {
        new (finish.cur) T(std::move(val));
        finish.set_node(finish + 1);
        finish.cur = finish.first;
    } catch(std::exception&) {
        deallocate_node(*(finish.node - 1));
        throw;
    }
}

template <class T, class Alloc>
void deque<T, Alloc>::pop_front_aux() {
    destroy(start.cur);
    deallocate_node(*(start.node));
    start.set_node(start.node + 1);
    start.cur = start.first;
}

template <class T, class Alloc>
void deque<T, Alloc>::pop_back_aux() {
    deallocate_node(*(finish.node));
    finish.set_node(finish.node - 1);
    finish.cur = finish.last - 1;
    destroy(finish.cur);
}

template <class T, class Alloc>
typename deque<T, Alloc>::iterator
deque<T, Alloc>::insert(const_iterator pos, const T& val) {
    if(pos.cur == start.cur) {
        push_front(val);
        return start;
    } else if(pos.cur == finish.cur) {
        push_back(val);
        iterator tmp = finish;
        --tmp;
        return tmp;
    } else {
        return insert_aux(pos, val);
    }
}

template <class T, class Alloc>
typename deque<T, Alloc>::iterator
deque<T, Alloc>::insert(const_iterator pos, T&& val) {
    if(pos.cur == start.cur) {
        push_front(std::forward(val));
        return start;
    } else if(pos.cur == finish.cur) {
        push_back(std::forward(val));
        iterator tmp = finish;
        --tmp;
        return tmp;
    } else {
        return insert_aux(pos, std::forward(val));
    }
}

template <class T, class Alloc>
typename deque<T, Alloc>::iterator
deque<T, Alloc>::insert_aux(const_iterator pos, const T& val) {
    difference_type idx = pos - start;
    T x_copy = val;
    if(static_cast<size_type>(idx) < size() / 2) {
        // insert at front half part, copy front
        push_front(front());
        iterator front1 = start;
        ++front1;
        iterator front2 = front2;
        ++front2;
        pos = start + idx;
        iterator pos1 = pos;
        ++pos1;
        copy(front2, pos1, front1);
    } else {
        // insert at back half part, copy back
        push_back(back());
        iterator back1 = finish;
        --back1;
        iterator back2 = back1;
        --back2;
        pos = start + idx;
        copy_backward(pos, back2, back1);
    }
    *pos = x_copy;
    return pos;
}

template <class T, class Alloc>
typename deque<T, Alloc>::iterator
deque<T, Alloc>::insert_aux(const_iterator pos, T&& val) {
    difference_type idx = pos - start;
    if(static_cast<size_type>(idx) < size() / 2) {
        // insert at front half part, copy front
        push_front(front());
        iterator front1 = start;
        ++front1;
        iterator front2 = front2;
        ++front2;
        pos = start + idx;
        iterator pos1 = pos;
        ++pos1;
        copy(front2, pos1, front1);
    } else {
        // insert at back half part, copy back
        push_back(back());
        iterator back1 = finish;
        --back1;
        iterator back2 = back1;
        --back2;
        pos = start + idx;
        copy_backward(pos, back2, back1);
    }
    *pos = std::forward(val); // move assign
    return pos;
}

template <class T, class Alloc>
typename deque<T, Alloc>::iterator 
deque<T, Alloc>::fill_insert(const_iterator pos, size_type n,
                             const T& val) {
    if(n == 0)
        return pos;
    
    iterator res = pos;
    if(pos.cur == start.cur) {
        iterator new_start = reserve_element_at_front(n);
        try {
            uninitialized_fill(new_start, start, val);
            start = new_start;
            res = new_start;
        } catch(std::exception&) {
            destroy_nodes(new_start.node, start.node);
            throw;
        }
    } else if(pos.cur == finish.cur) {
        iterator new_finish = reserve_element_at_back(n);
        try {
            uninitialized_fill(finish, new_finish, val);
            res = finish;
            finish = new_finish;
        } catch(std::exception&) {
            destroy_nodes(finish.node + 1, new_finish.node + 1);
            throw;
        }
    } else {
        res = fill_insert_aux(pos, n, val);
    }
    return res;
}

template <class T, class Alloc>
typename deque<T, Alloc>::iterator 
deque<T, Alloc>::fill_insert_aux(const_iterator pos, size_type n, const T& val) {
    if(n == 0)
        return pos;
    
    iterator res = pos;
    difference_type elems_before = pos - start;
    difference_type n1 = static_cast<difference_type>(n);
    if(elems_before < size() / 2) {
        iterator new_start = reserve_element_at_front(n);
        iterator old_start = start;
        pos = start + elems_before;
        try {
            if(elems_before >= n1) {
                iterator start_n = start + n1;
                uninitialized_copy(start, start_n, new_start);
                start = new_start;
                copy(start_n, pos, old_start);
                fill(pos - n1, pos, val);
            } else {
                uninitialized_copy(start, pos, new_start);
                uninitialized_fill(new_start + elems_before, start, val);
                start = new_start;
                fill(old_start, pos, val);
            }
            res = new_start + elems_before;
        } catch(std::exception&) {
            destroy_nodes(new_start.node, start.node);
        }
    } else {
        iterator new_finish = reserve_element_at_back(n);
        iterator old_finish = finish;
        difference_type elems_after = finish - pos;
        pos = finish - elems_after;
        try {
            if(elems_after > n) {
                iterator finish_n = finish - n1;
                uninitialized_copy(finish_n, finish, finish);
                finish = new_finish;
                copy_backward(pos, finish_n, old_finish);
                fill(pos, finish_n, val);
            } else {
                uninitialized_fill(finish, pos + n1, val);
                uninitialized_copy(pos, finish, pos + n1);
                finish = new_finish;
                fill(pos, old_finish, val);
            }
            res = pos;
        } catch(std::exception&) {
            deallocate_node(finish.node + 1, new_finish.node + 1);
        }
    }
    return res;
}


template <class T, class Alloc>
template <class ForwardIt>
typename deque<T, Alloc>::iterator
deque<T, Alloc>::range_insert_aux(const_iterator pos, ForwardIt first, 
                                  ForwardIt last, size_type n) {
    if(first == last)
        return pos;

    iterator res = pos;
    difference_type elems_before = pos - start;
    difference_type n1 = static_cast<difference_type>(n);
    if(elems_before < size() / 2) {
        iterator new_start = reserve_element_at_front(n);
        iterator old_start = start;
        pos = start + elems_before;
        try {
            if(elems_before >= n1) {
                iterator start_n = start + n1;
                uninitialized_copy(start, start_n, new_start);
                start = new_start;
                copy(start_n, pos, old_start);
                copy(first, last, pos - n1);
            } else {
                ForwardIt mid = first;
                advance(mid, n1 - elems_before);
                uninitialized_copy(start, pos, new_start);
                uninitialized_copy(first, mid, new_start + elems_before);
                start = new_start;
                copy(mid, last, old_start);
            }
            res = new_start + elems_before;
        } catch(std::exception&) {
            destroy_nodes(new_start.node, start.node);
        }
    } else {
        iterator new_finish = reserve_element_at_back(n);
        iterator old_finish = finish;
        difference_type elems_after = finish - pos;
        pos = finish - elems_after;
        try {
            if(elems_after > n1) {
                iterator finish_n = finish - n1;
                uninitialized_copy(finish_n, finish, finish);
                finish = new_finish;
                copy_backward(pos, finish_n, old_finish);
                copy(first, last, pos);
            } else {
                ForwardIt mid = first;
                advance(mid, elems_after);
                uninitialized_copy(mid, last, finish);
                uninitialized_copy(pos, finish, pos + n1);
                finish = new_finish;
                copy(first, mid, pos);
            }
            res = pos;
        } catch(std::exception&) {
            deallocate_node(finish.node + 1, new_finish.node + 1);
        }
    }
    return pos;
}

template <class T, class Alloc>
template <class InputIt>
typename deque<T, Alloc>::iterator
deque<T, Alloc>::range_insert(const_iterator pos, InputIt first, 
                              InputIt last, input_iterator_tag) {
    // use insert iter simple code
    copy(first, last, inserter(*this, pos));
}

template <class T, class Alloc>
template <class ForwardIt>
typename deque<T, Alloc>::iterator
deque<T, Alloc>::range_insert(const_iterator pos, ForwardIt first, 
                              ForwardIt last, forward_iterator_tag) {
    if(first == last)
        return pos;
    
    size_type n = distance(first, last);
    iterator res = pos;
    if(pos.cur == start.cur) {
        iterator new_start = reserve_element_at_front(n);
        try {
            uninitialized_copy(first, last, new_start);
            start = new_start;
            res = new_start;
        } catch(std::exception&) {
            destroy_nodes(new_start.node, start.node);
            throw;
        }
    } else if(pos.cur == finish.cur) {
        iterator new_finish = reserve_element_at_back(n);
        try {
            uninitialized_copy(first, last, finish);
            res = finish;
            finish = new_finish;
        } catch(std::exception&) {
            destroy_nodes(finish.node + 1, new_finish.node + 1);
            throw;
        }
    } else {
        res = range_insert_aux(pos, first, last, n);
    }
    return res;
}


template <class T, class Alloc>
typename deque<T, Alloc>::iterator 
deque<T, Alloc>::erase(const_iterator pos) {
    iterator next = pos;
    ++next;
    difference_type idx = pos - start;
    if(static_cast<size_type>(idx) < size() / 2) {
        copy_backward(start, pos, next);
        pop_front();
    } else {
        copy(next, finish, pos);
        pop_back();
    }
    return start + idx;
}


template <class T, class Alloc>
typename deque<T, Alloc>::iterator 
deque<T, Alloc>::erase(const_iterator first, const_iterator last) {
    if(first == start && first == finish) {
        clear();
        return finish;
    } else {
        difference_type n = last - first;
        difference_type elems_before = first - start;
        if(elems_before < static_cast<difference_type>(size() - n) / 2) {
            copy_backward(start, first, last);
            iterator new_start = start + n;
            destroy(start, new_start);
            destroy_nodes(start.node, new_start.node);
            start = new_start;
        } else {
            copy(last, finish, first);
            iterator new_finish = finish - n;
            destroy(new_finish, finish);
            destroy_nodes(new_finish.node - 1, finish.node - 1);
            finish = new_finish;
        }
        return start + elems_before;
    }
}


// postcondition: deque has one node, no elements
template <class T, class Alloc>
void deque<T, Alloc>::clear() noexcept {
    // destroy nodes [start + 1, finish - 1]
    for(T** cur = start.node + 1;cur != finish.node;++cur) {
        destroy(*cur, *cur + buf_size());
        deallocate_node(*cur);
    }
    if(start.node != finish.node) {
        // start and finish are not same node
        // destroy elements and dealloc finish
        destroy(start.cur, start.last);
        destroy(finish.first, finish.cur);
        deallocate_node(*(finish.node));
        finish = start;
    } else {
        // same node, just destroy elements
        destroy(start.cur, finish.cur);
    }
}

} // MiniSTL

