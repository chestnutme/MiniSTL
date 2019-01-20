#pragma once

#include "Iterator/iterator.hpp"
#include "Allocator/memory.hpp"
#include "Traits/type_traits.hpp"
#include "Algorithms/algobase.hpp"
#include <cstddef>
#include <exception>
#include <initializer_list>
#include <cstdint>
#include <utility>

namespace MiniSTL {


template <class T>
struct list_node {
    T data;
    list_node* prev;
    list_node* next;
};

template <class T>
struct list_iterator {
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using iterator_category = bidirectional_iterator_tag;
    using value_type = T;
    using pointer = T*;
    using reference = T&;

    using self = list_iterator<T>;
    using node_t = list_node<T>;

    node_t* node;

    list_iterator() {}
    list_iterator(node_t* x) : node(x) {}

    reference operator*() const { return node->data; }

    pointer operator->() const { return &(operator*()); }

    self& operator++() {
        node = node->next;
        return *this;
    }

    self operator++(int) {
        self tmp = *this;
        node = node->next;
        return tmp;
    }

    self& operator--() {
        node = node->prev;
        return *this;
    }

    self operator--(int) {
        self tmp = *this;
        node = node->prev;
        return *this;
    }

    bool operator==(const self& x) const { return node == x.node; }
    bool operator!=(const self& x) const { return node != x.node; }
};

template <class T>
struct const_list_iterator {
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using iterator_category = bidirectional_iterator_tag;
    using value_type = T;
    using pointer = const T*;
    using reference = const T&;

    using self = const_list_iterator<T>;
    using node_t = list_node<T>;

    node_t* node;

    const_list_iterator() {}
    const_list_iterator(node_t* x) : node(x) {}
    const_list_iterator(const list_iterator<T>& x) : node(x.node) {}

    reference operator*() const { return node->data; }

    pointer operator->() const { return &(operator*()); }

    self& operator++() {
        node = node->next;
        return *this;
    }

    self operator++(int) {
        self tmp = *this;
        node = node->next;
        return tmp;
    }

    self& operator--() {
        node = node->prev;
        return *this;
    }

    self operator--(int) {
        self tmp = *this;
        node = node->prev;
        return *this;
    }

    bool operator==(const self& x) const { return node == x.node; }
    bool operator!=(const self& x) const { return node != x.node; }
};

template <class T, class Allocator = simple_alloc<T>>
class list {
public:
    // types alias
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using value_type = T;
    using reference = T&;
    using pointer = T*;
    using const_reference = const T&;
    using const_pointer = const T*;
    using iterator = list_iterator<T>;
    using const_iterator = const_list_iterator<T>;
    using reverse_iterator = __reverse_iterator<iterator>;
    using const_reverse_iterator = __reverse_iterator<const_iterator>;
    using allocator_type = Allocator;

protected:
    using node_alloc = simple_alloc<list_node<T>>;
    using node_t = list_node<T>;
    node_t* dummy;

    void initialize() {
        dummy = get_node();
        dummy->prev = dummy;
        dummy->next = dummy;
    }

    node_t* get_node() { return node_alloc::allocate(1); }

	void put_node(node_t* p) { node_alloc::deallocate(p); }

	node_t* create_node(const T& val) {
        node_t* p = get_node();
        try {
            construct(&p->data, val);
        } catch(std::exception&) {
            put_node(p);
        }
        return p;
    } 

    node_t* create_node(T&& val)  {
        node_t* p = get_node();
        try {
            new (&p->data) T(std::forward(val));
        } catch(std::exception&) {
            put_node(p);
        }
        return p;
    }
    
public:
    allocator_type get_allocator() const noexcept { return allocator_type(); }

public:
    // construct/copy/destroy:
    list() { initialize(); }

    explicit list(size_type n) {
        initialize();
        insert(begin(), n, T());
    }

    list(size_type n, const T& val) {
        initialize();
        insert(begin(), n, val);
    }

    template <class InputIt>
    list(InputIt first, InputIt last) {
        initialize();
        // no need for dispatch here, insert does all of that anyway
        insert(begin(), first, last);
    }

    list(list&& x) {
        dummy = x.dummy;
        x.dummy = nullptr;
    }

    list(const list& x) {
        initialize();
        insert(begin(), x.begin(), x.end());
    }

    list(std::initializer_list<T> ilist) {
        initialize();
        insert(begin(), ilist.begin(), ilist.end());
    }

    ~list() {
        clear();
        put_node(dummy);
    }

    list& operator=(const list& x);

    list& operator=(list&& x) {
        if(&x != this) {
            clear();
            swap(x);
        }
        return *this;
    }

    list& operator=(std::initializer_list<T> ilist);

public:
    template <class InputIt>
    void assign(InputIt first, InputIt last) {
        assign_dispatch(first, last, integral<InputIt>());
    }

    void assign(size_type n, const T& val) {
        fill_assign(n, val);
    }

    void assign(std::initializer_list<T> ilist){
        assign_dispatch(ilist.begin(), ilist.end(), false_type());
    }

protected:
	void fill_assign(size_type n, const T& val);

	template<class Integer>
	void assign_dispatch(Integer n, Integer val, true_type){
		fill_assign(static_cast<size_type>(n),static_cast<T>(val));
	}

	template<class InputIt>
	void assign_dispatch(InputIt first, InputIt last, false_type);

public:
    // iterators:
    iterator                begin() noexcept { return dummy->next; }
    const_iterator          begin() const noexcept 
        { return const_iterator(dummy->next) ;}
    iterator                end() noexcept { return dummy; }
    const_iterator          end() const noexcept 
        { return const_iterator(dummy); }
 
    reverse_iterator        rbegin() noexcept {
        return reverse_iterator(end());
    }
    const_reverse_iterator  rbegin() const noexcept {
        return const_reverse_iterator(end());
    }
    reverse_iterator        rend() noexcept {
        return reverse_iterator(begin());
    }
    const_reverse_iterator  rend() const noexcept {
        return const_reverse_iterator(begin());
    }
 
    const_iterator          cbegin() const noexcept 
        { return const_iterator(dummy->next) ;}
    const_iterator          cend() const noexcept 
        { return const_iterator(dummy) ;}
    const_reverse_iterator  crbegin() const noexcept 
        { return const_reverse_iterator(end()); }
    const_reverse_iterator  crend() const noexcept 
        { return const_reverse_iterator(begin()); }
 
public:
    // capacity:
    size_type size() const noexcept {
        return distance(begin(), end());
    }

    size_type max_size() const noexcept {
        return SIZE_MAX / sizeof(node_t);
    }

    void resize(size_type new_sz) {
        resize(new_sz, T());
    }
    void resize(size_type new_sz, const T& val);


    bool empty() const noexcept {
        return dummy->next == dummy;
    }
 
public:
    // element access:
    reference front() { return *begin(); }
    const_reference front() const { return *begin(); }
    reference back() { return *(--end()); }
    const_reference back() const { return *(--end()); }
 
public:
    // modifiers:
    template <class... Args> 
    void emplace_front(Args&&... args) {
        insert(begin(), std::move(T(args...)));
    }

    void pop_front() { erase(begin()); }

    template <class... Args> 
    void emplace_back(Args&&... args) {
        insert(end(), std::move(T(args...)));
    }

    void push_front(const T& val) { insert(begin(), val); }
    void push_front(T&& val) { insert(begin(), std::forward(val)); }

    void push_back(const T& val) { insert(end(), val); }
    void push_back(T&& val) { insert(end(), std::forward(val)); }
    void pop_back() { erase(--end()); }
 
    template <class... Args> 
    iterator emplace(const_iterator pos, Args&&... args) {
        insert(end(), std::move(T(args...)));
    }
    
    // return iterator following the last removed element. If the iterator 
    // pos refers to the last element, the end() iterator is returned.
    iterator erase(const_iterator pos);
    iterator erase(const_iterator first, const_iterator last);

    void swap(list& y){ MiniSTL::swap(dummy, y.dummy); }

    void clear() noexcept;

public: 
    // insert before the pos:
    // return iterator pointing to the inserted value 
    // or pos if n = 0, first == last, ilist is empty
    iterator insert(const_iterator pos, const T& val);
    iterator insert(const_iterator pos, T&& val);
    iterator insert(const_iterator pos, size_type n, const T& val) {
        return fill_insert(pos, n, val);
    }
    template <class InputIt>
    iterator insert(const_iterator pos, InputIt first, InputIt last) {
        return insert_dispatch(pos, first, last, integral<InputIt>());
    }
    iterator insert(const_iterator pos, list<T> ilist) {
        return insert_dispatch(pos, ilist.begin(), ilist.end(), false_type());
    }
 
protected:
	iterator fill_insert(const_iterator pos, size_type n, const T& val);

	template <class Integer>
	iterator insert_dispatch(const_iterator pos, Integer n, Integer val, true_type){
		return fill_insert(pos, static_cast<size_type>(n), static_cast<T>(val));
	}

	template <class InputIt>
	iterator insert_dispatch(const_iterator pos, InputIt first, InputIt last, false_type);
    
public:
    // splice: transfers elements from one list to another.
    // pos - element before which the content will be inserted
    void splice(const_iterator pos, list& x) {
        if(!x.empty())
            transfer(pos, x.begin(), x.end());
    }
    void splice(const_iterator pos, list&& x) {
        // after splice, x has no element， conforming move semantic
        if(!x.empty())
            transfer(pos, x.begin(), x.end());
    }
    // transfers the element pointed to by it from other into *this.
    void splice(const_iterator pos, list& x, const_iterator i) {
        const_iterator j = i;
        ++j;
        // pos == i : cannot insert one before itself
        // pos == j : i already before pos
        if(pos == i || pos == j) return;
        transfer(pos, i, j);
    }
    void splice(const_iterator pos, list&& x, const_iterator i) {
        const_iterator j = i;
        ++j;
        if(pos == i || pos == j) return;
        transfer(pos, i, j);
    }
    void splice(const_iterator pos, list& x,
                const_iterator first, const_iterator last) {
        if(first != last)
            transfer(pos, first, last);
    }
    void splice(const_iterator pos, list&& x,
                const_iterator first, const_iterator last) {
        if(first != last)
            transfer(pos, first, last);
    }

protected:
    void transfer(const_iterator pos, const_iterator first, const_iterator last);
 
public:
    void remove(const T& value);
    template <class Predicate> 
    void remove_if(Predicate pred);
 
    void unique();
    template <class BinaryPredicate> 
    void unique(BinaryPredicate binary_pred);
 
    // precondition: two lists must be sorted
    void merge(list& x);
    void merge(list&& x);
    template <class Compare> 
    void merge(list& x, Compare comp);
    template <class Compare> 
    void merge(list&& x, Compare comp);
 
    void sort();
    template <class Compare> void sort(Compare comp);
 
    void reverse() noexcept;
};

 
template <class T, class Alloc>
bool operator==(const list<T,Alloc>& x, const list<T,Alloc>& y) {
    using const_iterator = typename list<T, Alloc>::const_iterator;
    const_iterator end1 = x.end();
    const_iterator end2 = y.end();
    const_iterator it1 = x.begin();
    const_iterator it2 = y.begin();
    while(it1 != end1 && it2 != end2 && *it1 == *it2) {
        it1++;
        it2++;
    }
    return it1 == end1 && it2 == end2;
}

template <class T, class Alloc>
bool operator<(const list<T,Alloc>& x, const list<T,Alloc>& y) {
    return lexicographical_compare(x.begin(), x.end(),
                                   y.begin(), y.end());
}

template <class T, class Alloc>
bool operator!=(const list<T,Alloc>& x, const list<T,Alloc>& y) {
    return !(x == y);
}

template <class T, class Alloc>
bool operator>(const list<T,Alloc>& x, const list<T,Alloc>& y) {
    return y < x;
}

template <class T, class Alloc>
bool operator>=(const list<T,Alloc>& x, const list<T,Alloc>& y) {
    return !(x < y);
}

template <class T, class Alloc>
bool operator<=(const list<T,Alloc>& x, const list<T,Alloc>& y) {
    return !(y < x);
}

template <class T, class Alloc>
void swap(list<T,Alloc>& x, list<T,Alloc>& y) {
    x.swap(y);
}

template <class T, class Alloc>
list<T, Alloc>& list<T, Alloc>::operator=(const list& x) {
    if(&x != this) {
        // list tmp(x);
        // this->swap(tmp);
        iterator first1 = begin();
        const_iterator first2 = x.begin();
        iterator last1 = end();
        const_iterator last2 = x.end();
        while(first1 != last1 && first2 != last2)
            *first1++ = *first2++;
        if(first2 == last2)
            erase(first1, last2);
        else
            insert(last1, first2, last2);
    }
    return *this;
}

template <class T, class Alloc>
list<T, Alloc>& list<T, Alloc>::operator=(std::initializer_list<T> ilist) {
    iterator first1 = begin();
    iterator first2 = ilist.begin();
    iterator last1 = end();
    iterator last2 = ilist.end();
    while(first1 != last1 && first2 != last2)
        *first1++ = *first2++;
    if(first2 == last2)
        erase(first1, last2);
    else
        insert(last1, first2, last2);
    return *this;
}

template <class T, class Alloc>
void list<T, Alloc>::fill_assign(size_type n, const T& val) {
    iterator cur = begin();
    for(;cur != end() && n > 0;++cur, --n)
        *cur = val;
    if(n > 0)
        fill_insert(end(), n, val);
    else
        erase(cur, end());
}

template <class T, class Alloc>
template<class InputIt>
void list<T, Alloc>::assign_dispatch(InputIt first, InputIt last, false_type) {
    iterator cur = begin();
    for(;cur != end() && first != last;++cur, ++first)
        *cur = *first;
    if(first != last)
        insert(cur, first, last);
    else
        erase(cur, end());
}

template <class T, class Alloc>
void list<T, Alloc>::resize(size_type new_sz, const T& val) {
    iterator cur = begin();
    for(;cur != end() && new_sz > 0;++cur, --new_sz);
    if(new_sz == 0)
        erase(cur, end());
    else
        insert(cur, new_sz, val);
}

template <class T, class Alloc>
typename list<T, Alloc>::iterator
list<T, Alloc>::erase(const_iterator pos) {
    node_t* prev = pos.node->prev;
    node_t* next = pos.node->next;
    prev->next = next;
    next->prev = prev;
    destory(&pos.node->data);
    put_node(pos.node);
    return iterator(next);
}

template <class T, class Alloc>
typename list<T, Alloc>::iterator
list<T, Alloc>::erase(const_iterator first, const_iterator last) {
    while(first != last)
        erase(first++);
    return iterator(last);
}

template <class T, class Alloc>
void list<T, Alloc>::clear() noexcept {
    node_t* cur = dummy->next;
    while(cur != dummy) {
        node_t* tmp = cur;
        cur = cur->next;
        destroy(&tmp->data);
        put_node(cur);
    }
    dummy->next = dummy->prev = dummy;
}

template <class T, class Alloc>
typename list<T, Alloc>::iterator 
list<T, Alloc>::insert(const_iterator pos, const T& val) {
    node_t* tmp = create_node(val);
    tmp->next = pos.node;
    tmp->prev = pos.node->prev;
    pos.node->prev = tmp;
    tmp->prev->next = tmp;
    return iterator(tmp);
}

template <class T, class Alloc>
typename list<T, Alloc>::iterator 
list<T, Alloc>::insert(const_iterator pos, T&& val) {
    node_t* tmp = create_node(std::forward(val));
    tmp->next = pos.node;
    tmp->prev = pos.node->prev;
    pos.node->prev = tmp;
    tmp->prev->next = tmp;
    return iterator(tmp);
}

template <class T, class Alloc>
typename list<T, Alloc>::iterator
list<T, Alloc>::fill_insert(const_iterator pos, size_type n, const T& val) {
    if(n == 0)
        return iterator(pos.node);
    iterator res = insert(pos, val);
    for(--n;n > 0;--n)
        insert(pos, val);
    return res;
}

template <class T, class Alloc>
template <class InputIt>
typename list<T, Alloc>::iterator
list<T, Alloc>::insert_dispatch(const_iterator pos, InputIt first, InputIt last, false_type) {
    if(first == last)
        return iterator(pos.node);
    iterator res = insert(pos, *first);
    for(++first;first != last;++first)
        insert(pos, *first);
    return res;
}

template <class T, class Alloc>
void list<T, Alloc>::transfer(const_iterator pos, 
                              const_iterator first, 
                              const_iterator last) {
    if (pos != last) {
        // Remove [first, last) from its old position.
        node_t* tmp = last.node->prev;
        first.node->prev->next = last.node;
        last.node->prev = first.node->prev;

        // Splice [first, last) into its new position.
        pos.node->prev->next = first.node;
        first.node->prev = pos.node->prev;
        tmp->next = pos.node;
        pos.node->prev = tmp;
    }
}

template <class T, class Alloc>
void list<T, Alloc>::remove(const T& val) {
    iterator first = begin();
    iterator last = end();
    while(first != last) {
        iterator next = first;
        ++next;
        if(*first == val)
            erase(first);
        first = next;
    }
}

template <class T, class Alloc>
template <class Predicate> 
void list<T, Alloc>::remove_if(Predicate pred) {
    iterator first = begin();
    iterator last = end();
    while(first != last) {
        iterator next = first;
        ++next;
        if(pred(*first))
            erase(first);
        first = next;
    }
}
 
template <class T, class Alloc>
void list<T, Alloc>::unique() {
    iterator first = begin();
    iterator last = end();
    if(first == last)
        return;
    iterator next = first;
    ++next;
    while(next != last) {
        if(*first == *next) {
            erase(next);
            next = first;
        } else {
            first = next;
        }
        next++;
    }
}

template <class T, class Alloc>
template <class BinaryPredicate> 
void list<T, Alloc>::unique(BinaryPredicate binary_pred) {
    iterator first = begin();
    iterator last = end();
    if(first == last)
        return;
    iterator next = first;
    ++next;
    while(next != last) {
        if(binary_pred(*first, *next)) {
            erase(next);
            next = first;
        } else {
            first = next;
        }
        next++;
    }
}
 
template <class T, class Alloc>
void list<T, Alloc>::merge(list& x) {
    iterator first1 = begin();
    iterator last1 = end();
    iterator first2 = x.begin();
    iterator last2 = x.end();
    while(first1 != last1 && first2 != last2) {
        if(*first2 < *first1) {
            iterator next = first2;
            next++;
            while(next != last2 && *next < *first1)
                next++;
            transfer(first1, first2, next);
            first2 = next;
        } else {
            while(first1 != last1 && !(*first2 < *first1))
                ++first1;
        }
    }
    if(first2 != last2)
        transfer(last1, first2, last2);
}

template <class T, class Alloc>
void list<T, Alloc>::merge(list&& x) {
    iterator first1 = begin();
    iterator last1 = end();
    iterator first2 = x.begin();
    iterator last2 = end();
    while(first1 != last1 && first2 != last2) {
        if(*first2 < *first1) {
            iterator next = first2;
            next++;
            while(next != last2 && *next < *first1)
                next++;
            transfer(first1, first2, next);
            first2 = next;
        } else {
            while(first1 != last1 && !(*first2 < *first1))
                ++first1;
        }
    }
    if(first2 != last2)
        transfer(last1, first2, last2);
}
     
template <class T, class Alloc>
template <class Compare>
void list<T, Alloc>::merge(list& x, Compare comp) {
    iterator first1 = begin();
    iterator last1 = end();
    iterator first2 = x.begin();
    iterator last2 = end();
    while(first1 != last1 && first2 != last2) {
        if(comp(*first2, *first1)) {
            iterator next = first2;
            next++;
            while(next != last2 && comp(*next, *first1))
                next++;
            transfer(first1, first2, next);
            first2 = next;
        } else {
            while(first1 != last1 && !(comp(*first2, *first1)))
                ++first1;
        }
    }
    if(first2 != last2)
        transfer(last1, first2, last2);
}

template <class T, class Alloc>
template <class Compare>
void list<T, Alloc>::merge(list&& x, Compare comp) {
    iterator first1 = begin();
    iterator last1 = end();
    iterator first2 = x.begin();
    iterator last2 = end();
    while(first1 != last1 && first2 != last2) {
        if(comp(*first2, *first1)) {
            iterator next = first2;
            next++;
            while(next != last2 && comp(*next, *first1))
                next++;
            transfer(first1, first2, next);
            first2 = next;
        } else {
            while(first1 != last1 && !(comp(*first2, *first1)))
                ++first1;
        }
    }
    if(first2 != last2)
        transfer(last1, first2, last2);
}
 

// iterative mergesort, O(n * logn)
template <class T, class Alloc>
void list<T, Alloc>::sort() {
    // empty or size = 1
    if(dummy->next == dummy || dummy->next->next == dummy)
        return;
    list carry;
    list counter[64];
    int fill = 0;
    while (!empty()) {
        carry.splice(carry.begin(), *this, begin());
        int i = 0;
        while(i < fill && !counter[i].empty()) {
            counter[i].merge(carry);
            carry.swap(counter[i]);
            i++;
        }
        carry.swap(counter[i]);         
        if (i == fill) 
            ++fill;
    } 

    for (int i = 1; i < fill; ++i) 
        counter[i].merge(counter[i-1]);
    swap(counter[fill-1]);
}
    
template <class T, class Alloc>
template <class Compare>
void list<T, Alloc>::sort(Compare comp) {
    if(dummy->next == dummy || dummy->next->next == dummy)
        return;
    list carry;
    list counter[64];
    int fill = 0;
    while (!empty()) {
        carry.splice(carry.begin(), *this, begin());
        int i = 0;
        while(i < fill && !counter[i].empty()) {
            counter[i].merge(carry, comp);
            carry.swap(counter[i]);
            i++;
        }
        carry.swap(counter[i]);         
        if (i == fill) 
            ++fill;
    } 

    for (int i = 1; i < fill; ++i) 
        counter[i].merge(counter[i-1], comp);
    swap(counter[fill-1]);
}
 
template <class T, class Alloc>
void list<T, Alloc>::reverse() noexcept {
    node_t* tmp = dummy;
    do {
        MiniSTL::swap(tmp->prev, tmp->next);
        tmp = tmp->prev;
    } while(tmp != dummy);
}

} // MiniSTL