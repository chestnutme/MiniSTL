#pragma once

#include "Algorithms/algobase.hpp"
#include "Allocator/memory.hpp"
#include "Function/function.hpp"

#include <cstddef>


namespace MiniSTL {
    
using rb_tree_color_t = bool;
const rb_tree_color_t rb_tree_red = false;
const rb_tree_color_t rb_tree_black = true;

template <class Value>
struct rb_tree_node {
    using color_t = rb_tree_color_t;
    using node_ptr_t = rb_tree_node*;

    color_t color;
    node_ptr_t parent;
    node_ptr_t left;
    node_ptr_t right;
    Value value_field;

    static node_ptr_t minimum(node_ptr_t x) {
        while(x->left)
            x = x->left;
        return x;
    }

    static node_ptr_t maximum(node_ptr_t x) {
        while(x->right)
            x = x->right;
        return x;
    }
};

template <class Value, class Ref, class Ptr>
struct rb_tree_iterator {
    using iterator = rb_tree_iterator<Value, Value&, Value*>;
	using const_iterator = rb_tree_iterator<Value, const Value&, const Value*>;
	
	using iterator_category = bidirectional_iterator_tag;
	using value_type = Value;
	using pointer = Ptr;
	using reference = Ref;
	using size_type = size_t;
	using difference_type = ptrdiff_t;

    using node_ptr_t = rb_tree_node<Value>*;
    using self_t = rb_tree_iterator<Value, Ref, Ptr>;

    node_ptr_t node;

    rb_tree_iterator() : node(nullptr) {}
    rb_tree_iterator(node_ptr_t x) : node(x) {}
    rb_tree_iterator(const iterator& i) : node(i.node) {}

    void incre() {
        if(node->right) {
            // case1: next is right subtree 's min
            // ? if node = header, aks end(), then node will be
            // ? begin(), aka min()
            node = rb_tree_node::minimum(node->right);
        } else {
            // case2: no right subtree
            // up until node is left of p
            // case 2.1: p is next
            node_ptr_t p = node->parent;
            while(node == p->right) {
                node = p;
                p = p->parent;
            }
            // special case2.2: node = root withour right and 
            // p == header, next is header, aka end();
            if(node->right != p)
                node = p;
        }
    }

    void decre() {
        if(node->color = rb_tree_red && 
                    node->parent->parent == node) {
            // special case1: node = header, 
            // prev = mostright, aka max;
            node = node->right;
        } else if (node->left) {
            // case2: prev = left subtree's max
            node = rb_tree_node::maximum(node->left); 
        } else {
            // case3: no left subtree
            // up until node is right of p, p is prev
            // ? if node = begin(), aks min, after loop p =  
            // ? header and node = root, then node will be end()
            node_ptr_t p = node->parent;
            while(node == p->left) {
                node = p;
                p = p->parent;
            }
            node = p;
        }
    }

    reference opreator*() const { return node->value_field; }
    pointer opreator->() const { return &(oparetor*()); }

    self_t& operator++() { incre(); return *this; }
    self_t operator++(int) {
        self_t tmp = *this;
        incre();
        return tmp;
    }

    self_t& operator--() { decre(); return *this; }
    self_t operator--(int) {
        self_t tmp = *this;
        decre();
        return tmp;
    }

    bool operator==(const self_t& y) { return node == y.node; }
    bool operator==(const self_t& y) { return node != y.node; }
};


template <class Key, class Value, class KeyOfValue, 
          class Compare, class Alloc = simple_alloc<Value>>
class rb_tree {
protected:
    using node_t = rb_tree_node<Value>;
    using node_ptr_t = rb_tree_node<Value>*;
    using color_t = rb_tree_color_t;
    using node_alloc = simple_alloc<node_t>;
public:
    using key_type = Key;
    using value_type = Value;
    using allocator_type = Alloc;
    using size_type	= size_t;
    using difference_type = ptrdiff_t;
    using reference	= Value&;
    using const_reference = const Value&;
    using pointer = Value*;
    using const_pointer = const Value*;
    using iterator = rb_tree_iterator<Value, Value&, Value*>;
    using const_iterator = rb_tree_iterator<Value, const Value&, const Value*>;
    using reverse_iterator	= __reverse_iterator<iterator>;
    using const_reverse_iterator = __reverse_iterator<const_iterator>;

public:
    allocator_type get_allocator() { return allocator_type(); }

protected:
    // data member
    node_ptr_t header;
    size_t node_count;
    Compare comp;

public:
    Compare key_compare() const { return comp; }

protected:
    node_ptr_t get_node() { return node_alloc::allocate(1); }
    void put_node(node_ptr_t p) { node_alloc::deallocate(p, 1); }

    node_ptr_t create_node(const Value& val);
    node_ptr_t clone_node(node_ptr_t p);
    void destroy_node(node_ptr_t p) 
        { destroy(&p->value_field); put_node(p); }

protected:
    node_ptr_t& root() const { return header->parent; }
    node_ptr_t& leftmost() const { return header->left; }
    node_ptr_t& rightmost() const { return header->right; }

    static node_ptr_t& left(node_ptr_t p) { return p->left; }
    static node_ptr_t& right(node_ptr_t p) { return p->right; }
    static node_ptr_t& parent(node_ptr_t p) { return p->parent; }
    static Value& value(node_ptr_t p) { return p->value_field; }
    static const Key& key(node_ptr_t p) 
        { return KeyOfValue()(value(p)); }
    static color_t& color(node_ptr_t p) { return p->color; }

    static node_ptr_t minimum(node_ptr_t p) 
        { return rb_tree_node::minimum(p); }

    static node_ptr_t maximum(node_ptr_t p)
        { return rb_tree_node::maximum(p); }

private:
    iterator insert(node_ptr_t x, node_ptr_t y, const Value& val);
    iterator insert(node_ptr_t x, node_ptr_t y, const Value&& val);
    node_ptr_t copy(node_ptr_t x, node_ptr_t y);
    void erase(node_ptr_t p);
    void empty_initialize();
    void clear();

public:
    //  ctor/dtor/assign
    rb_tree();
    rb_tree(const Compare& comp);
    rb_tree(const rb_tree& x);
    rb_tree(rb_tree&& x);

    ~rb_tree();

    rb_tree& opreator==(const rb_tree& x);
    rb_tree& opreator==(rb_tree&& x);

public:
    // element access
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
    
public:
    // capacity
    size_type size() const noexcept;
    size_type max_size() const noexcept;
    bool      empty() const noexcept;

public:
    //swap
    void swap(rb_tree& y);

protected:
    void rb_tree_rotate_left(node_ptr_t x, node_ptr_t& root);
	void rb_tree_rotate_right(node_ptr_t x node_ptr_t& root);
	void rb_tree_rebalance(node_ptr_t x, node_ptr_t& root);
	node_ptr_t 
    rb_tree_rebalance_for_erase(node_ptr_t z, node_ptr_t& root, 
                node_ptr_t& leftmost, node_ptr_t& rightmost);

public:
    // insert
	pair<iterator, bool> insert_unique(const Value& val);
    pair<iterator, bool> insert_unique(Value&& val);

	iterator insert_unique(iterator pos, const Value& val);
    iterator insert_unique(iterator pos, Value&& val); 

	template<class InputIt>
	void insert_unique(InputIt first, InputIt last);

    iterator insert_equal(const Value& val);
    iterator insert_equal(Value&& val);

	iterator insert_equal(iterator pos, const Value& val);
    iterator insert_equal(iterator pos, Value&& val);

	template<class InputIt>
	void insert_equal(InputIt first,InputIt last);

public:
    // erase
	void erase(iterator pos);
	size_type erase(const Key& key);
	void erase(iterator first, iterator last);

public:
    // find
	iterator find(const Key& x) noexcept;
	const_iterator find(const Key& x) const noexcept;

	size_type count(const Key& x) const noexcept;

	iterator lower_bound(const Key& x) noexcept;
	const_iterator lower_bound(const Key& x) const noexcept;
	iterator upper_bound(const Key& x) noexcept;
	const_iterator upper_bound(const Key& x) const noexcept;
    
	pair<iterator,iterator> 
    equal_range(const Key& x) noexcept;
	pair<const_iterator,const_iterator> 
    equal_range(const Key& x) const noexcept;
};

template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
inline bool operator==(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& x, rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& y){
	return x.size() == y.size() && std::equal(x.cbegin(),x.cend(),y.cbegin());
}

template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
inline bool operator!=(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& x, rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& y){
	return !(x == y);
}

template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
inline bool operator<(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& x, rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& y){
	return std::lexicographical_compare(x.cbegin(), x.cend(),                                        y.cbegin(), y.cend());
}

template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
inline bool operator>(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& x, rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& y){
	return y < x;
}

template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
inline bool operator<=(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& x, rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& y){
	return !(y < x);
}

template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
inline bool operator>=(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& x, rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& y){
	return !(x < y);
}

template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
inline void swap(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& x, rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& y) noexcept {
	x.swap(y);
}


} // MiniSTL
