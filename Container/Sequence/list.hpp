#pragma once

#include "Iterator/iterator_base.hpp"
#include "Allocator/allocator.hpp"
#include <cstddef>


namespace MiniSTL {


template <class T>
struct list_node {
    T data;
    list_node* prev;
    list_node* next;
};

template <class T, class Ref, class Ptr>
struct list_iterator {
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using iterator_category = bidirectional_iterator_tag;
    using value_type = T;
    using pointer = Ptr;
    using reference = Ref;

    using iterator = list_iterator<T, T&, T*>;
    using const_iterator = list_iterator<T, const T&, const T*>;
    using self = list_iterator<T, Ref, Ptr>;
    using node_t = list_node<T>;

    node_t* node;

    list_iterator() {}
    list_iterator(node* x) : node(x) {}
    list_iterator(const iterator& x) : node(x.node) {}

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

    bool operator==(const iterator& x) const { return node == x.node; }
    bool operator!=(const iterator& x) const { return node != x.node; }
};

template <class T, class Allocator = simple_alloc<T>>
class list {
public:
    // types:
    typedef value_type&                                         reference;
    typedef const value_type&                                   const_reference;
    typedef /*implementation-defined*/                          iterator;
    typedef /*implementation-defined*/                          const_iterator; 
    typedef /*implementation-defined*/                          size_type;
    typedef /*implementation-defined*/                          difference_type;
    typedef T                                                   value_type;
    typedef Allocator                                           allocator_type;
    typedef typename allocator_traits<Allocator>::pointer       pointer;
    typedef typename allocator_traits<Allocator>::const_pointer const_pointer;
    typedef std::reverse_iterator<iterator>                     reverse_iterator;
    typedef std::reverse_iterator<const_iterator>               const_reverse_iterator;
 
    // construct/copy/destroy:
    explicit list(const Allocator& = Allocator());
    explicit list(size_type n);
    list(size_type n, const T& value,const Allocator& = Allocator());
    template <class InputIterator>
        list(InputIterator first, InputIterator last,const Allocator& = Allocator());
    list(const list<T,Allocator>& x);
    list(list&&);
    list(const list&, const Allocator&);
    list(list&&, const Allocator&);
    list(initializer_list<T>, const Allocator& = Allocator());
    ~list();
    list<T,Allocator>& operator=(const list<T,Allocator>& x);
    list<T,Allocator>& operator=(list<T,Allocator>&& x);
    list& operator=(initializer_list<T>);
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
 
    const_iterator          cbegin() const noexcept;
    const_iterator          cend() const noexcept;
    const_reverse_iterator  crbegin() const noexcept;
    const_reverse_iterator  crend() const noexcept;
 
    // capacity:
    size_type size() const noexcept;
    size_type max_size() const noexcept;
    void      resize(size_type sz);
    void      resize(size_type sz, const T& c);
    bool      empty() const noexcept;
 
    // element access:
    reference       front();
    const_reference front() const;
    reference       back();
    const_reference back() const;
 
    // modifiers:
    template <class... Args> void emplace_front(Args&&... args);
    void pop_front();
    template <class... Args> void emplace_back(Args&&... args);
    void push_front(const T& x);
    void push_front(T&& x);
    void push_back(const T& x);
    void push_back(T&& x);
    void pop_back();
 
    template <class... Args> iterator emplace(const_iterator position, Args&&... args);
    iterator insert(const_iterator position, const T& x);
    iterator insert(const_iterator position, T&& x);
    iterator insert(const_iterator position, size_type n, const T& x);
    template <class InputIterator>
        iterator insert (const_iterator position, InputIterator first, 
                         InputIterator last);
    iterator insert(const_iterator position, initializer_list<T>);
 
 
    iterator erase(const_iterator position);
    iterator erase(const_iterator first, const_iterator last);
    void     swap(list<T,Allocator>&);
    void     clear() noexcept;
 
    // list operations:
    void splice(const_iterator position, list<T,Allocator>& x);
    void splice(const_iterator position, list<T,Allocator>&& x);
    void splice(const_iterator position, list<T,Allocator>& x,
                    const_iterator i);
    void splice(const_iterator position, list<T,Allocator>&& x,
                    const_iterator i);
    void splice(const_iterator position, list<T,Allocator>& x,
                    const_iterator first, const_iterator last);
    void splice(const_iterator position, list<T,Allocator>&& x,
                    const_iterator first, const_iterator last);
 
    void remove(const T& value);
    template <class Predicate> void remove_if(Predicate pred);
 
    void unique();
    template <class BinaryPredicate> void unique(BinaryPredicate binary_pred);
 
    void merge(list<T,Allocator>& x);
    void merge(list<T,Allocator>&& x);
    template <class Compare> void merge(list<T,Allocator>& x, Compare comp);
    template <class Compare> void merge(list<T,Allocator>&& x, Compare comp);
 
    void sort();
    template <class Compare> void sort(Compare comp);
 
    void reverse() noexcept;
};


} // MiniSTL