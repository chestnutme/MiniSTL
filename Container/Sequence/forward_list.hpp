#pragma once


#include <initializer_list>

#include "Allocator/memory.hpp"
#include "Traits/type_traits.hpp"
#include "Iterator/iterator.hpp"

namespace MiniSTL {

struct forward_list_node_base {
    forward_list_node_base* next;
};

inline forward_list_node_base*
make_link(forward_list_node_base* prev_node,
          forward_list_node_base* new_node) {
    new_node->next = prev_node->next;
    prev_node->next = new_node;
    return new_node;
}

inline forward_list_node_base* 
previous(forward_list_node_base* head,
                 const forward_list_node_base* node) {
    while (head && head->next != node)
        head = head->next;
    return head;
}

inline const forward_list_node_base* 
previous(const forward_list_node_base* head,
         const forward_list_node_base* node) {
    while (head && head->next != node)
        head = head->next;
    return head;
}

inline void 
splice_after(forward_list_node_base* pos,
             forward_list_node_base* before_first,
             forward_list_node_base* before_last) {
    if (pos != before_first && pos != before_last) {
        forward_list_node_base* first = before_first->next;
        forward_list_node_base* after = pos->next;
        before_first->next = before_last->next;
        pos->next = first;
        before_last->next = after;
    }
}

inline void
splice_after(forward_list_node_base* pos, 
             forward_list_node_base* head) {
    forward_list_node_base* before_last = previous(head, 0);
    if (before_last != head) {
        forward_list_node_base* after = pos->next;
        pos->next = head->next;
        head->next = 0;
        before_last->next = after;
    }
}

inline forward_list_node_base* 
reverse(forward_list_node_base* node) {
    forward_list_node_base* result = node;
    node = node->next;
    result->next = 0;
    while(node) {
        forward_list_node_base* next = node->next;
        node->next = result;
        result = node;
        node = next;
    }
    return result;
}

inline size_t size(forward_list_node_base* node) {
    size_t result = 0;
    for ( ; node != 0; node = node->next)
        ++result;
    return result;
}

template <class T>
struct forward_list_node : public forward_list_node_base {
    T data;
};

template <class T, class Ref, class Ptr>
struct forward_list_iterator {
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using iterator_category = forward_iterator_tag;
    using value_type = T;
    using pointer = Ptr;
    using reference = Ref;

    using iterator = forward_list_iterator<T, T&, T*>;
    using const_iterator = forward_list_iterator<T, const T&, const T*>;
    using self = forward_list_iterator<T, Ref, Ptr>;
    using node_t = forward_list_node<T>;

    node_t* node;

    forward_list_iterator() {}
    forward_list_iterator(node_t* x) : node(x) {}
    forward_list_iterator(const iterator& x) : node(x.node) {}

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

    bool operator==(const iterator& x) const { return node == x.node; }
    bool operator!=(const iterator& x) const { return node != x.node; }
};


template <class T, class Allocator = simple_alloc<T> >
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
    using reverse_iterator = reverse_iterator<iterator>;
    using const_reverse_iterator = reverse_iterator<const_iterator>;
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
            construct(p->data, val);
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
    template <class InputIterator>
    forward_list(InputIterator first, InputIterator last) {
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
    forward_list(initializer_list<T> ilist) {
        head.next = nullptr;
        insert_after_range(static_cast<node_base*>(&head), ilist.begin(), ilist.end());
    }
 
    ~forward_list() {
        clear();
    }

    forward_list& operator=(const forward_list& x);
    forward_list& operator=(forward_list&& x);
    forward_list& operator=(initializer_list<T>);

public:
    //assign
    template <class InputIterator>
    void assign(InputIterator first, InputIterator last);
    void assign(size_type n, const T& t);
    void assign(initializer_list<T>);

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
    iterator                before_begin() noexcept;
    const_iterator          before_begin() const noexcept;
    iterator                begin() noexcept;
    const_iterator          begin() const noexcept;
    iterator                end() noexcept;
    const_iterator          end() const noexcept;
 
    const_iterator          cbegin() noexcept;
    const_iterator          cbefore_begin() const noexcept;
    const_iterator          cend() noexcept;
 
public:
    // capacity:
    size_type max_size() const noexcept;
    bool      empty() const noexcept;
    void resize(size_type new_sz);
    void resize(size_type new_sz, const value_type& val);

public:
    // element access:
    reference       front();
    const_reference front() const;
 
    // modifiers:
    template <class... Args> void emplace_front(Args&&... args);
 
    void push_front(const T& x);
    void push_front(T&& x);
    void pop_front();

    template <class... Args> 
    iterator emplace_after(const_iterator position, Args&&... args);

public:
    iterator insert_after(const_iterator position, const T& x);
    iterator insert_after(const_iterator position, T&& x);
 
    iterator insert_after(const_iterator position, size_type n, const T& x);
    template <class InputIterator>
    iterator insert_after(const_iterator position, InputIterator first, 
                              InputIterator last);
    iterator insert_after(const_iterator position, initializer_list<T> il);

protected:

    void insert_after_fill(node_base* pos,
                           size_type n, const value_type& val) {
        for (size_type i = 0; i < n; ++i)
            pos = make_link(pos, create_node(val));
    }

    template <class InputIt>
    void insert_after_range(node_base* pos, 
                                InputIt first, InputIt last) {
        insert_after_range(pos, first, last, isInteger<InputIt>());
    }

    template <class Integer>
    void insert_after_range(node_base* pos, Integer n, Integer val,
                            true_type) {
        insert_after_fill(pos, static_cast<size_type>(n), 
                          static_cast<T>(val));
    }

    template <class InputIt>
    void insert_after_range(node_base* pos, InputIt first, 
                            InputIt last, false_type) {
        while (first != last) {
            pos = make_link(pos, create_node(*first));
            ++first;
        }
    }
public:
    iterator erase_after(const_iterator position);
    iterator erase_after(const_iterator position, iterator last);
    void swap(forward_list&);
    void clear() noexcept;
 
public:
    // forward_list operations:
    void splice_after(const_iterator position, forward_list& x);
    void splice_after(const_iterator position, forward_list&& x);
    void splice_after(const_iterator position, forward_list& x,
                    const_iterator i);
    void splice_after(const_iterator position, forward_list&& x,
                    const_iterator i);
    void splice_after(const_iterator position, forward_list& x,
                    const_iterator first, const_iterator last);
    void splice_after(const_iterator position, forward_list&& x,
                    const_iterator first, const_iterator last);
 
    void remove(const T& value);
    template <class Predicate> void remove_if(Predicate pred);
 
    void unique();
    template <class BinaryPredicate> void unique(BinaryPredicate binary_pred);
 
    void merge(forward_list& x);
    void merge(forward_list&& x);
    template <class Compare> void merge(forward_list& x, Compare comp);
    template <class Compare> void merge(forward_list&& x, Compare comp);
 
    void sort();
    template <class Compare> void sort(Compare comp);
    void reverse() noexcept;
};

template <class T, class Alloc>
bool operator==(const forward_list<T,Alloc>& x, const forward_list<T,Alloc>& y) {
    using const_iterator = forward_list<T, Alloc>::const_iterator;
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








} // MiniSTL