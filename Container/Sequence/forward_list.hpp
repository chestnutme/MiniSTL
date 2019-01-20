#pragma once


#include <initializer_list>
#include <cstdint>
#include <utility>

#include "Allocator/memory.hpp"
#include "Traits/type_traits.hpp"
#include "Iterator/iterator.hpp"

namespace MiniSTL {

struct forward_list_node_base {
    forward_list_node_base* next;
};

inline forward_list_node_base*
__make_link(forward_list_node_base* prev_node,
            forward_list_node_base* new_node) {
    new_node->next = prev_node->next;
    prev_node->next = new_node;
    return new_node;
}

inline forward_list_node_base* 
__previous(forward_list_node_base* head,
           const forward_list_node_base* node) {
    while (head && head->next != node)
        head = head->next;
    return head;
}

inline const forward_list_node_base* 
__previous(const forward_list_node_base* head,
           const forward_list_node_base* node) {
    while (head && head->next != node)
        head = head->next;
    return head;
}

inline void 
__splice_after(forward_list_node_base* pos,
             forward_list_node_base* before_first,
             forward_list_node_base* before_last) {
    // pos == before_first : done
    // pos = before_last : can not insert one after itself
    if (pos != before_first && pos != before_last) {
        forward_list_node_base* first = before_first->next;
        forward_list_node_base* after = pos->next;
        before_first->next = before_last->next;
        pos->next = first;
        before_last->next = after;
    }
}

inline void
__splice_after(forward_list_node_base* pos, 
             forward_list_node_base* head) {
    forward_list_node_base* before_last = __previous(head, nullptr);
    if (before_last != head) {
        forward_list_node_base* after = pos->next;
        pos->next = head->next;
        head->next = nullptr;
        before_last->next = after;
    }
}

inline forward_list_node_base* 
__reverse(forward_list_node_base* node) {
    forward_list_node_base* result = node;
    node = node->next;
    result->next = nullptr;
    while(node) {
        forward_list_node_base* next = node->next;
        node->next = result;
        result = node;
        node = next;
    }
    return result;
}

inline size_t __size(forward_list_node_base* node) {
    size_t result = 0;
    for ( ; node != nullptr; node = node->next)
        ++result;
    return result;
}

template <class T>
struct forward_list_node : public forward_list_node_base {
    T data;
};


struct forward_list_iterator_base {
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using iterator_category = forward_iterator_tag;

    forward_list_node_base* node;
    
    forward_list_iterator_base(forward_list_node_base* x) : node(x) {}
    
    void incr() { node = node->next; }

    bool operator==(const forward_list_iterator_base& x) const { return node == x.node; }
    bool operator!=(const forward_list_iterator_base& x) const { return node != x.node; }

};


template <class T, class Ref, class Ptr>
struct forward_list_iterator : public forward_list_iterator_base {
    using value_type = T;
    using pointer = Ptr;
    using reference = Ref;

    using iterator = forward_list_iterator<T, T&, T*>;
    using const_iterator = forward_list_iterator<T, const T&, const T*>;
    using self = forward_list_iterator<T, Ref, Ptr>;
    using node_t = forward_list_node<T>;
    using node_base = forward_list_node_base;

    forward_list_iterator() : forward_list_iterator_base(nullptr) {}
    forward_list_iterator(node_t* x) 
        : forward_list_iterator_base(static_cast<node_base*>(x)) {}
    forward_list_iterator(const iterator& x) 
        : forward_list_iterator_base(x.node)  {}

    reference operator*() const 
        { return static_cast<node_t*>(node)->data; }

    pointer operator->() const { return &(operator*()); }

    self& operator++() {
        incr();
        return *this;
    }

    self operator++(int) {
        self tmp = *this;
        incr();
        return tmp;
    }
};


template <class T, class Allocator = simple_alloc<T>>
class forward_list {
public:
    // types alias
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using value_type = T;
    using reference = T&;
    using pointer = T*;
    using const_reference = const T&;
    using const_pointer = const T*;
    using iterator = forward_list_iterator<T, T&, T*>;
    using const_iterator = forward_list_iterator<T, const T&, const T*>;
    using allocator_type = Allocator;
 
protected:
    using node_alloc = simple_alloc<forward_list_node<T>>;
    using node_t = forward_list_node<T>;
    using node_base = forward_list_node_base;
    
    // head does not need a data member
    forward_list_node_base head;
    
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
    forward_list() { head.next = nullptr; }

    explicit forward_list(size_type n) {
        head.next = nullptr;
        insert_after_fill(static_cast<node_base*>(&head), n, T());
    }
    forward_list(size_type n, const T& val) {
        head.next = nullptr;
        insert_after_fill(static_cast<node_base*>(&head), n, T());
    }
    template <class InputIt>
    forward_list(InputIt first, InputIt last) {
        head.next = nullptr;
        insert_after_range(static_cast<node_base*>(&head), first, last);
    }
    forward_list(const forward_list& x) {
        head.next = nullptr;
        insert_after_range(static_cast<node_base*>(&head), x.begin(), x.end());
    }
    forward_list(forward_list&& x) {
        head = x.head;
        x.head.next = nullptr;
    }
    forward_list(std::initializer_list<T> ilist) {
        head.next = nullptr;
        insert_after_range(before_begin(), ilist.begin(), ilist.end());
    }
 
    ~forward_list() {
        clear();
    }

    forward_list& operator=(const forward_list& x);
    forward_list& operator=(forward_list&& x) {
        if(&x != this) {
            clear();
            swap(x);
        }
        return *this;

    }

    forward_list& operator=(std::initializer_list<T>);

public:
    //assign
    template <class InputIt>
    void assign(InputIt first, InputIt last) {
        assign_dispatch(first, last, integral<InputIt>());
    }
    void assign(size_type n, const T& val) {
        fill_assign(n, val);
    }
    void assign(std::initializer_list<T> ilist) {
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
    iterator        before_begin() noexcept {
        return iterator(static_cast<node_t*>(&head));
    }
    const_iterator  before_begin() const noexcept {
        // wrong : const_iterator(static_cast<node_t*>(&head))
        // because &head is const node_base*, cannot cast away const qualifier
        const_iterator(before_begin());
    }
    iterator        begin() noexcept {
        return iterator(static_cast<node_t*>(head.next));
    }
    const_iterator  begin() const noexcept {
        return const_iterator(static_cast<node_t*>(head.next));
    }
    iterator        end() noexcept {
        return iterator();
    }
    const_iterator  end() const noexcept {
        return const_iterator();
    }
 
    const_iterator  cbegin() noexcept {
        return const_iterator(begin()) ;
    }
    const_iterator  cbefore_begin() const noexcept {
        // 
        return const_iterator(before_begin());
    }
    const_iterator  cend() noexcept{
        return const_iterator();
    }
 
public:
    // capacity:
    size_type max_size() const noexcept {
        return SIZE_MAX / sizeof(node_t);
    }
    bool empty() const noexcept {
        return head.next == nullptr;
    }
    void resize(size_type new_sz) {
        resize(new_sz, T());
    }
    void resize(size_type new_sz, const value_type& val);

public:
    // element access:
    reference       front() { return *begin(); }
    const_reference front() const { return *begin(); }
 
    // modifiers:
    template <class... Args>
    void emplace_front(Args&&... args) {
        insert_after(before_begin(), std::move(T(args...)));
    }
 
    void push_front(const T& val) {
        insert_after(before_begin(), val);
    }
    void push_front(T&& val) {
        insert_after(before_begin(), std::forward(val));
    }
    void pop_front() {
        erase_after(before_begin());
    }

    template <class... Args> 
    iterator emplace_after(const_iterator pos, Args&&... args) {
        return insert_after(pos, std::move(T(args...)));
    }

public:
    iterator insert_after(const_iterator pos, const T& val) {
        return iterator(static_cast<node_t*>(__make_link(
           static_cast<node_base*>(pos.node), create_node(val))));
    }
    iterator insert_after(const_iterator pos, T&& val) {
        return iterator(static_cast<node_t*>(__make_link(
           static_cast<node_base*>(pos.node), 
           create_node(std::forward(val)))));
    }
 
    iterator insert_after(const_iterator pos, size_type n, const T& val) {
        return insert_after_fill(pos, n, val);
    }

    // return iterator pointing to the last element inserted
    template <class InputIt>
    iterator insert_after_range(iterator pos, 
                            InputIt first, InputIt last) {
        return insert_after_range(pos, first, last,            
                                  integral<InputIt>());
    }

    iterator insert_after(const_iterator pos, 
                          std::initializer_list<T> ilist) {
        return insert_after_range(pos, ilist.begin(),
                                  ilist.end(), false_type());
    }

protected:
    iterator insert_after_fill(iterator pos,
                           size_type n, const value_type& val) {
        if(n == 0)
            return pos;
        node_base* cur = static_cast<node_base*>(pos.node);
        for(size_type i = 0; i < n; ++i)
            cur = __make_link(cur, create_node(val));
        return iterator(static_cast<node_t*>(cur));
    }

    template <class Integer>
    iterator insert_after_range(iterator pos, Integer n, 
                                Integer val, true_type) {
        return insert_after_fill(pos, static_cast<size_type>(n), 
                                 static_cast<T>(val));
    }

    template <class InputIt>
    iterator insert_after_range(iterator pos, InputIt first, 
                                InputIt last, false_type) {
        if(first == last)
            return pos;
        node_base* cur = static_cast<node_base*>(pos.node);
        while (first != last) {
            cur = __make_link(cur, create_node(*first));
            ++first;
        }
        return iterator(static_cast<node_t*>(cur));
    }


public:
    iterator erase_after(const_iterator pos) {
        if(pos.node == nullptr || pos.node->next == nullptr)
            return end();
        node_base* prev = static_cast<node_base*>(pos.node);
        node_base* cur = prev->next;
        prev->next = cur->next;
        destroy(&(static_cast<node_t*>(cur)->data));
        put_node(static_cast<node_t*>(cur));
        return iterator(prev->next);
    }

    iterator erase_after(const_iterator pos, iterator last) {
        node_base* prev = static_cast<node_base*>(pos.node);
        node_base* cur = prev->next;
        node_base* last1 = static_cast<node_base*>(last.node);
        while(cur != last1) {
            prev->next = cur->next;
            destroy(&(static_cast<node_t*>(cur)->data));
            put_node(static_cast<node_t*>(cur));
            cur = prev->next;
        }
        return last;
    }

    void swap(forward_list& x) {
        MiniSTL::swap(head.next, x.head.next);
    }

    void clear() noexcept {
        erase_after(before_begin(), end());
    }
 
public:
    // forward_list operations:
    void splice_after(const_iterator pos, forward_list& x) {
        node_base* cur = static_cast<node_base*>(pos.node);
        node_base* prev = &x.head;
        if(prev->next)
            __splice_after(pos, prev);
    }
    
    void splice_after(const_iterator pos, forward_list&& x) {
        node_base* cur = static_cast<node_base*>(pos.node);
        node_base* prev = &x.head;
        if(prev->next)
            __splice_after(pos, prev);
    }
    
    void splice_after(const_iterator pos, forward_list& x,
                      const_iterator i) {
        if(i == x.before_begin() || i == end()) return;
        node_base* cur = static_cast<node_base*>(pos.node);
        node_base* n1 = static_cast<node_base*>(i.node);
        __splice_after(cur, __previous(&x.head, n1), n1);
    }
    
    void splice_after(const_iterator pos, forward_list&& x,
                     const_iterator i) {
        if(i == x.before_begin() || i == end()) return;
        node_base* cur = static_cast<node_base*>(pos.node);
        node_base* n1 = static_cast<node_base*>(i.node);
        __splice_after(cur, __previous(&x.head, n1), n1);
    }
    
    // Moves the elements in the range (first, last) from other into *this
    void splice_after(const_iterator pos, forward_list& x,
                      const_iterator first,
                      const_iterator last) {
        node_base* cur = static_cast<node_base*>(pos.node);
        node_base* n1 = static_cast<node_base*>(first.node);
        node_base* n2 = static_cast<node_base*>(last.node);
        // n1 == n2 || n1->next == n2 : splice nothing
        if(n1 == n2 || n1->next == n2) 
            return;
        __splice_after(cur, n1, __previous(&x.head, n2));
    }

    void splice_after(const_iterator pos, forward_list&& x,
                      const_iterator first, const_iterator last) {
        node_base* cur = static_cast<node_base*>(pos.node);
        node_base* n1 = static_cast<node_base*>(first.node);
        node_base* n2 = static_cast<node_base*>(last.node);
        // n1 == n2 || n1->next == n2 : splice nothing
        if(n1 == n2 || n1->next == n2) 
            return;
        __splice_after(cur, n1, __previous(x, n2));
    }
    
    void remove(const T& val);
    
    template <class Predicate> 
    void remove_if(Predicate pred);
 
    void unique();
    template <class BinaryPredicate> 
    void unique(BinaryPredicate binary_pred);
 
    void merge(forward_list& x);
    void merge(forward_list&& x);
    template <class Compare> void 
    merge(forward_list& x, Compare comp);
    template <class Compare> void 
    merge(forward_list&& x, Compare comp);
 
    void sort();
    template <class Compare> 
    void sort(Compare comp);
    
    void reverse() noexcept;
};

template <class T, class Alloc>
bool operator==(const forward_list<T,Alloc>& x, const forward_list<T,Alloc>& y) {
    using const_iterator = typename forward_list<T, Alloc>::const_iterator;
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
bool operator< (const forward_list<T,Alloc>& x, const forward_list<T,Alloc>& y) {
    return lexicographical_compare(x.begin(), x.end(),
                                   y.begin(), y.end());
}

template <class T, class Alloc>
bool operator!=(const forward_list<T,Alloc>& x, const forward_list<T,Alloc>& y) {
    return !(x == y);
}

template <class T, class Alloc>
bool operator> (const forward_list<T,Alloc>& x, const forward_list<T,Alloc>& y) {
    return y < x;
}

template <class T, class Alloc>
bool operator>=(const forward_list<T,Alloc>& x, const forward_list<T,Alloc>& y) {
    return !(x < y);
}

template <class T, class Alloc>
bool operator<=(const forward_list<T,Alloc>& x, const forward_list<T,Alloc>& y) {
    return !(y < x);
}

template <class T, class Alloc>
void swap(forward_list<T,Alloc>& x, forward_list<T,Alloc>& y) {
    x.swap(y);
}

template <class T, class Alloc>
forward_list<T, Alloc>& forward_list<T, Alloc>::operator=(const forward_list& x) {
    if(&x != this) {
        // another implement:
        // forward_list tmp(x);
        // this->swap(tmp);

        // prev for erase_after and insert_after
        iterator prev = before_begin(); 
        iterator first1 = begin();
        const_iterator first2 = x.begin();
        iterator last1 = end();
        const_iterator last2 = x.end();
        while(first1 != last1 && first2 != last2) {
            prev = first1;
            *first1++ = *first2++;
        }
        if(first2 == last2)
            erase_after(prev, last2);
        else
            insert_after_range(prev, first2, last2);
    }
    return *this;
}

template <class T, class Alloc>
forward_list<T, Alloc>& forward_list<T, Alloc>::operator=(std::initializer_list<T> ilist) {
    iterator prev = before_begin();
    iterator first1 = begin();
    const_iterator first2 = ilist.begin();
    iterator last1 = end();
    const_iterator last2 = ilist.end();
    while(first1 != last1 && first2 != last2) {
        prev = first1;
        *first1++ = *first2++;
    }
    if(first2 == last2)
        erase_after(prev, last2);
    else
        insert_after_range(prev, first2, last2);
    return *this;
}

template <class T, class Alloc>
void forward_list<T, Alloc>::fill_assign(size_type n, const T& val) {
    iterator prev = before_begin();
    iterator cur = begin();
    for(;cur != end() && n > 0;++cur, --n) {
        prev = cur;
        *cur = val;
    }
    if(n > 0)
        insert_after_fill(prev, n, val);
    else
        erase_after(prev, end());
}

template <class T, class Alloc>
template<class InputIt>
void forward_list<T, Alloc>::assign_dispatch(InputIt first, InputIt last, false_type) {
    iterator prev = before_begin();
    iterator cur = begin();
    for(;cur != end() && first != last;++cur, ++first) {
        prev = cur;
        *cur = *first;
    }
    if(first != last)
        insert_after_range(prev, first, last);
    else
        erase_after(prev, end());
}

template <class T, class Alloc>
void forward_list<T, Alloc>::resize(size_type new_sz, const T& val) {
    iterator prev = before_begin();
    iterator cur = begin();
    for(;cur != end() && new_sz > 0;++cur, --new_sz)
        prev = cur;
    if(new_sz == 0)
        erase_after(prev, end());
    else
        insert_after_fill(prev, new_sz, val);
}

template <class T, class Alloc>
void forward_list<T, Alloc>::remove(const T& val) {
    iterator prev = before_begin();
    iterator cur = begin();
    iterator last = end();
    while(cur != last) {
        if(*cur == val) {
            erase_after(prev);
            cur = prev;
            cur++;
        } else {
            prev = cur++;
        }
    }
}

template <class T, class Alloc>
template <class Predicate> 
void forward_list<T, Alloc>::remove_if(Predicate pred) {
    iterator prev = before_begin();
    iterator cur = begin();
    iterator last = end();
    while(cur != last) {
        if(pred(*cur)) {
            erase_after(prev);
            cur = prev;
            cur++;
        } else {
            prev = cur++;
        }
    }
}
 
template <class T, class Alloc>
void forward_list<T, Alloc>::unique() {
    iterator first = begin();
    iterator last = end();
    if(first == last)
        return;
    iterator next = first;
    ++next;
    while(next != last) {
        if(*first == *next) {
            erase_after(first);
            next = first;
        }
        else {
            first = next;
        }
        ++next;
    }
}

template <class T, class Alloc>
template <class BinaryPredicate> 
void forward_list<T, Alloc>::unique(BinaryPredicate binary_pred) {
    iterator first = begin();
    iterator last = end();
    if(first == last)
        return;
    iterator next = first;
    ++next;
    while(next != last) {
        if(binary_pred(*first, *next)) {
            erase_after(first);
            next = first;
        }
        else {
            first = next;
        }
        ++next;
    }
}
 
template <class T, class Alloc>
void forward_list<T, Alloc>::merge(forward_list& x) {
    const_iterator prev1 = cbefore_begin();
    const_iterator first1 = cbegin();
    const_iterator last1 = cend();
    const_iterator prev2 = x.cbefore_begin();
    const_iterator first2 = x.cbegin();
    const_iterator last2 = cend();
    while(first1 != last1 && first2 != last2) {
        if(*first2 < *first1) {
            splice_after(prev1, x, first2);
            ++prev1;
            first2 = prev2; 
            ++first2;
        } else {
            prev1 = first1;
            ++first1;
        }
    }
    if(first2 != last2)
        splice_after(prev1, x, x.before_begin(), last2);
}

template <class T, class Alloc>
void forward_list<T, Alloc>::merge(forward_list&& x) {
    const_iterator prev1 = cbefore_begin();
    const_iterator first1 = cbegin();
    const_iterator last1 = cend();
    const_iterator prev2 = x.cbefore_begin();
    const_iterator first2 = x.cbegin();
    const_iterator last2 = cend();
    while(first1 != last1 && first2 != last2) {
        if(*first2 < *first1) {
            splice_after(prev1, x, first2);
            ++prev1;
            first2 = prev2;
            ++first2;
        } else {
            prev1 = first1;
            ++first1;
        }
    }
    if(first2 != last2)
        splice_after(prev1, x, x.before_begin(), last2);
}

template <class T, class Alloc>
template <class Compare>
void forward_list<T, Alloc>::merge(forward_list& x, Compare comp) {
    const_iterator prev1 = cbefore_begin();
    const_iterator first1 = cbegin();
    const_iterator last1 = cend();
    const_iterator prev2 = x.cbefore_begin();
    const_iterator first2 = x.cbegin();
    const_iterator last2 = cend();
    while(first1 != last1 && first2 != last2) {
        if(comp(*first2 , *first1)) {
            splice_after(prev1, x, first2);
            ++prev1;
            first2 = prev2;
            ++first2;
        } else {
            prev1 = first1;
            ++first1;
        }
    }
    if(first2 != last2)
        splice_after(prev1, x, x.before_begin(), last2);
}

template <class T, class Alloc>
template <class Compare>
void forward_list<T, Alloc>::merge(forward_list&& x, Compare comp) {
    const_iterator prev1 = cbefore_begin();
    const_iterator first1 = cbegin();
    const_iterator last1 = cend();
    const_iterator prev2 = x.cbefore_begin();
    const_iterator first2 = x.cbegin();
    const_iterator last2 = cend();
    while(first1 != last1 && first2 != last2) {
        if(comp(*first2 , *first1)) {
            splice_after(prev1, x, first2);
            ++prev1;
            first2 = prev2;
            ++first2;
        } else {
            prev1 = first1;
            ++first1;
        }
    }
    if(first2 != last2)
        splice_after(prev1, x, x.before_begin(), last2);
}

// iterative mergesort, O(n * logn)
template <class T, class Alloc>
void forward_list<T, Alloc>::sort() {
    // empty or size = 1
    if(head.next == nullptr || head.next->next == nullptr)
        return;
    forward_list carry;
    forward_list counter[64];
    int fill = 0;
    while (!empty()) {
        carry.splice_after(carry.before_begin(), *this, begin());
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
void forward_list<T, Alloc>::sort(Compare comp) {
    if(head.next == nullptr || head.next->next == nullptr)
        return;
    forward_list carry;
    forward_list counter[64];
    int fill = 0;
    while (!empty()) {
        carry.splice_after(carry.before_begin(), *this, begin());
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
void forward_list<T, Alloc>::reverse() noexcept {
    node_base* tmp = static_cast<node_base*>(&head)->next;
    if(tmp)
        __reverse(tmp);
}


// Specialization of insert_iterator so that insertions 
// will be constant time instead of linear time.

template <class T, class Alloc>
class insert_iterator<forward_list<T, Alloc> > {
protected:
    using Container = forward_list<T, Alloc>;
    using iterator_type = typename Container::iteartor;

    Container* container;
    iterator_type iter;

public:
    using container_type = Container;
    using iterator_category = output_iterator_tag;
    using value_type = void;
    using difference_type = void;
    using pointer = void;
    using reference = void;

    insert_iterator(Container& c, iterator_type i) : container(&c) {
        iter = c.before_begin();
        while(iter.node->next != i.node)
            ++iter;
    }

    insert_iterator<Container>&
    operator=(const typename Container::value_type& val) { 
        iter = container->insert_after(iter, val);
        return *this;
    }

    insert_iterator<Container>& operator*() { return *this; }
    insert_iterator<Container>& operator++() { return *this; }
    insert_iterator<Container>& operator++(int) { return *this; }
};

} // MiniSTL