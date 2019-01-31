/*
 * Author: chestnutme
 * EmaiL: 1724759858@qq.com
 * Date: 2018/12/28
 * Description:
 *      Binary-search tree class, designed for use in implementing STL
 *      associative containers(set, multiset, map, multimap).
 *      
 *      specific implementation:
 *      header node:
 *          1. root->parent = header
 *          2. header->parent = root
 *          3. header->left = min(begin())
 *          4. headr->right = max
 *          5. end() = header  
 * 
 */

#pragma once

#include "Algorithms/algobase.hpp"
#include "Allocator/memory.hpp"
#include "Function/function.hpp"
#include "Iterator/iterator.hpp"
#include "Util/pair.hpp"

#include <cstddef>
#include <exception>
#include <climits>

namespace MiniSTL {

template <class Value>
struct bs_tree_node {
    using node_ptr_t = bs_tree_node*;

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
struct bs_tree_iterator {
    using iterator = bs_tree_iterator<Value, Value&, Value*>;
	using const_iterator = bs_tree_iterator<Value, const Value&, const Value*>;
	
	using iterator_category = bidirectional_iterator_tag;
	using value_type = Value;
	using pointer = Ptr;
	using reference = Ref;
	using size_type = size_t;
	using difference_type = ptrdiff_t;

    using node_t = bs_tree_node<Value>;
    using node_ptr_t = bs_tree_node<Value>*;
    using self_t = bs_tree_iterator<Value, Ref, Ptr>;

    node_ptr_t node;

    bs_tree_iterator() : node(nullptr) {}
    bs_tree_iterator(node_ptr_t x) : node(x) {}
    bs_tree_iterator(const iterator& i) : node(i.node) {}

    void incre() {
        if(node->right) {
            // case1: next is right subtree 's min
            // ? if node = header, aks end(), then node will be
            // ? begin(), aka min()
            node = node_t::minimum(node->right);
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
        if(node->parent->parent == node) {
            // special case1: node = header, 
            // prev = mostright, aka max;
            node = node->right;
        } else if (node->left) {
            // case2: prev = left subtree's max
            node = node_t::maximum(node->left); 
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

    reference operator*() const { return node->value_field; }
    pointer operator->() const { return &(operator*()); }

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
    bool operator!=(const self_t& y) { return node != y.node; }
};

template <class Key, class Value, class KeyOfValue, 
          class Compare, class Alloc = simple_alloc<Value>>
class bs_tree {
protected:
    using node_t = bs_tree_node<Value>;
    using node_ptr_t = bs_tree_node<Value>*;
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
    using iterator = bs_tree_iterator<Value, Value&, Value*>;
    using const_iterator = bs_tree_iterator<Value, const Value&, const Value*>;
    using reverse_iterator	= __reverse_iterator<iterator>;
    using const_reverse_iterator = __reverse_iterator<const_iterator>;

public:
    // observior
    allocator_type get_allocator() { return allocator_type(); }
    Compare key_compare() const { return key_comp; }

protected:
    // data member
    node_ptr_t header;
    size_t node_count;
    Compare key_comp;

protected:
    node_ptr_t get_node() { return node_alloc::allocate(1); }
    void put_node(node_ptr_t p) { node_alloc::deallocate(p, 1); }

    node_ptr_t create_node(const Value& val) {
        node_ptr_t tmp = get_node();
        try {
            construct(&value(tmp), val);
        } catch(std::exception&) {
            put_node(tmp);
            throw;
        }
        return tmp;
    }

    node_ptr_t create_node(Value&& val) {
        node_ptr_t tmp = get_node();
        try {
            new (&value(tmp)) value_type(std::move(val));
        } catch(std::exception&) {
            put_node(tmp);
            throw;
        }
        return tmp;
    }

    // clone a new node with value, set pointer nullptr
    node_ptr_t clone_node(node_ptr_t p) {
        node_ptr_t tmp = create_node(p->value_field);
        tmp->parent = nullptr;
        tmp->left = nullptr;
        tmp->right = nullptr;
        return tmp;
    }

    void destroy_node(node_ptr_t p) 
        { destroy(&p->value_field); put_node(p); }

protected:
    node_ptr_t& root() const { return header->parent; }
    node_ptr_t& leftmost() const { return header->left; }
    node_ptr_t& rightmost() const { return header->right; }

    // helper func to get value and key
    static Value& value(node_ptr_t p) { return p->value_field; }
    static const Key& key(node_ptr_t p) 
        { return KeyOfValue()(value(p)); }

    static node_ptr_t minimum(node_ptr_t p) 
        { return node_t::minimum(p); }

    static node_ptr_t maximum(node_ptr_t p)
        { return node_t::maximum(p); }

private:
    void empty_initialize() {
        header = get_node();
        root() = nullptr;
        leftmost() = header;
        rightmost() = header;
    }

    void clear() {
        if(node_count != 0) {
            erase(root());
            root() = nullptr;
            leftmost() = header;
            rightmost() = header;
            node_count = 0;
        }
    }

    node_ptr_t copy(node_ptr_t x, node_ptr_t p);

public:
    //  ctor/dtor/assign
    bs_tree() : node_count(0), key_comp() { empty_initialize(); }
    bs_tree(const Compare& c) : node_count(0), key_comp(c)
        { empty_initialize(); }
    
    bs_tree(const bs_tree& x) : node_count(0), key_comp(x.key_comp) {
        if(x.root() == nullptr)
            empty_initialize();
        else {
            header = get_node();
            root() = copy(x.root(), header);
            leftmost() = node_t::minimum(root());
            rightmost() = node_t::maximum(root());
        }
        node_count = x.node_count;
    }

    bs_tree(bs_tree&& x) 
        : header(x.header), node_count(x.node_count), key_comp(x.key_comp) {
        x.node_count = 0;
        x.header = nullptr;
    }

    ~bs_tree() {
        clear();
        put_node(header);
    }

    bs_tree& operator==(const bs_tree& x);
    bs_tree& operator==(bs_tree&& x);

public:
    // element access
    iterator                begin() noexcept { return leftmost(); }
    const_iterator          begin() const noexcept { return leftmost(); }
    iterator                end() noexcept { return header; }
    const_iterator          end() const noexcept { return header;}
 
    reverse_iterator        rbegin() noexcept 
        { return reverse_iterator(end()); }
    const_reverse_iterator  rbegin() const noexcept
        { return const_reverse_iterator(end()); }
    reverse_iterator        rend() noexcept
        { return reverse_iterator(begin()); }
    const_reverse_iterator  rend() const noexcept
        { return const_reverse_iterator(begin()); }
 
    const_iterator          cbegin() noexcept { return leftmost(); }
    const_iterator          cend() noexcept { return header(); }
    const_reverse_iterator  crbegin() const noexcept
        { return const_reverse_iterator(end()); }
    const_reverse_iterator  crend() const noexcept
        { return const_reverse_iterator(begin()); }

public:
    // capacity
    size_type size() const noexcept { return node_count; }
    size_type max_size() const noexcept 
        { return UINT_MAX / sizeof(value_type); }
    bool empty() const noexcept { return node_count == 0; }

public:
    //swap
    void swap(bs_tree& y) {
        swap(header, y.header);
        swap(node_count, y.node_count);
        swap(key_comp, y.key_comp);
    }

private:
    node_ptr_t erase_aux(node_ptr_t z, node_ptr_t& root, 
                node_ptr_t& leftmost, node_ptr_t& rightmost);

    iterator insert_aux(node_ptr_t x, node_ptr_t y, const Value& val);
    iterator insert_aux(node_ptr_t x, node_ptr_t y, Value&& val);

public:
    // insert
	pair<iterator, bool> insert_unique(const Value& val);
    pair<iterator, bool> insert_unique(Value&& val);

	iterator insert_unique(iterator pos, const Value& val);
    iterator insert_unique(iterator pos, Value&& val); 

	template<class InputIt>
	void insert_unique(InputIt first, InputIt last) {
        while(first != last)
            insert_unique(*first++);
    }

    iterator insert_equal(const Value& val);
    iterator insert_equal(Value&& val);

	iterator insert_equal(iterator pos, const Value& val);
    iterator insert_equal(iterator pos, Value&& val);

	template<class InputIt>
	void insert_equal(InputIt first, InputIt last) {
       while(first != last)
            insert_equal(*first++);
    }

private:
    // erase without rebalance
    void erase(node_ptr_t x) {
        while(x) {
            erase(x->right);
            // reduce stack usage due to recursion
            node_ptr_t y = x->left;
            destroy_node(x);
            x = y;
        }
    }

public:
    // erase
	iterator erase(iterator pos) {
        node_ptr_t y = erase_aux(pos.node, root(), leftmost(), rightmost());
        iterator i(y);
        ++i;
        destroy_node(y);
        --node_count;
        return i;
    }

	size_type erase(const Key& x) {
        pair<iterator, iterator> p = equal_range(x);
        size_type n = distance(p.first, p.second);
        erase(p.first, p.second);
        return n;
    }

	iterator erase(iterator first, iterator last) {
        if(first == begin() && last == end())
            clear();
        else {
            while(first != last)
                erase(*first++);
        }
    }

public:
    // find
    // if x exists, return first x, else return end();
	iterator find(const Key& k) noexcept {
        node_ptr_t y = header;
        node_ptr_t x = root();

        while(x) {
            if(key_comp(key(x), k)) // k > x
                x = x->right;
            else { // k <= x,set y = x,last node not less than k
                y = x;
                x = x->left;
            }
        }

        iterator i(y);
        // if found, y == k, but can't use y == k, because
        // y may be end(), that is empty tree
        // if found, y == k and 2 cases no found:
        //      a. empty tree
        //      b. k < y and y is last node which k <= y
        return (i == end() || key_comp(k, key(y))) ? end() : i;
    }

	const_iterator find(const Key& k) const noexcept {
        node_ptr_t y = header;
        node_ptr_t x = root();

        while(x) {
            if(key_comp(key(x), k))
                x = x->right;
            else {
                y = x;
                x = x->left;
            }
        }

        const_iterator i(y);
        return (i == end() || key_comp(k, key(y))) ? end() : i;
    }

	size_type count(const Key& k) const noexcept {
        pair<iterator, iterator> p = equal_range(k);
        return distance(p.first, p.second);
    }

	iterator lower_bound(const Key& k) noexcept {
        node_ptr_t y = header;
        node_ptr_t x = root();

        while(x) {
            if(key_comp(key(x), k))
                x = x->right;
            else {
                // if k <= x, set y is last node which y >= k
                y = x;
                x = x->left;
            }
        }
        // if k > max, y header, aka end()
        return iterator(y);
    }

	const_iterator lower_bound(const Key& k) const noexcept {
        node_ptr_t y = header;
        node_ptr_t x = root();

        while(x) {
            if(key_comp(key(x), k))
                x = x->right;
            else {
                y = x;
                x = x->left;
            }
        }

        return const_iterator(y);
    }

	iterator upper_bound(const Key& k) noexcept {
        node_ptr_t y = header;
        node_ptr_t x = root();

        while(x) {
            if(key_comp(k, key(x))) {
                // k < x, set y is first node which k < y
                y = x;
                x = x->left;
            }
            else
                x = x->right;
        }

        return iterator(y);
    }

	const_iterator upper_bound(const Key& k) const noexcept {
        node_ptr_t y = header;
        node_ptr_t x = root();

        while(x) {
            if(key_comp(k, key(x))) {
                // k < x, set y is first node which k < y
                y = x;
                x = x->left;
            }
            else
                x = x->right;
        }

        return const_iterator(y);
    }

	pair<iterator,iterator> 
    equal_range(const Key& k) noexcept {
        return make_pair(lower_bound(k), upper_bound(k));
    }

	pair<const_iterator,const_iterator> 
    equal_range(const Key& k) const noexcept {
        return make_pair(lower_bound(k), upper_bound(k));
    }
};


template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
inline bool operator==(const bs_tree<Key, Value, KeyOfValue, Compare, Alloc>& x, bs_tree<Key, Value, KeyOfValue, Compare, Alloc>& y){
	return x.size() == y.size() && equal(x.cbegin(), x.cend(), y.cbegin());
}

template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
inline bool operator!=(const bs_tree<Key, Value, KeyOfValue, Compare, Alloc>& x, bs_tree<Key, Value, KeyOfValue, Compare, Alloc>& y){
	return !(x == y);
}

template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
inline bool operator<(const bs_tree<Key, Value, KeyOfValue, Compare, Alloc>& x, bs_tree<Key, Value, KeyOfValue, Compare, Alloc>& y){
	return std::lexicographical_compare(x.cbegin(), x.cend(),                                        y.cbegin(), y.cend());
}

template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
inline bool operator>(const bs_tree<Key, Value, KeyOfValue, Compare, Alloc>& x, bs_tree<Key, Value, KeyOfValue, Compare, Alloc>& y){
	return y < x;
}

template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
inline bool operator<=(const bs_tree<Key, Value, KeyOfValue, Compare, Alloc>& x, bs_tree<Key, Value, KeyOfValue, Compare, Alloc>& y){
	return !(y < x);
}

template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
inline bool operator>=(const bs_tree<Key, Value, KeyOfValue, Compare, Alloc>& x, bs_tree<Key, Value, KeyOfValue, Compare, Alloc>& y){
	return !(x < y);
}

template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
inline void swap(const bs_tree<Key, Value, KeyOfValue, Compare, Alloc>& x, bs_tree<Key, Value, KeyOfValue, Compare, Alloc>& y) noexcept {
	x.swap(y);
}

template<class Key, class Value, class KeyOfValue, 
         class Compare, class Alloc>
bs_tree<Key, Value, KeyOfValue, Compare, Alloc>&
bs_tree<Key, Value, KeyOfValue, Compare, Alloc>::
operator==(const bs_tree& x) {
    if(&x != this) {
        clear();
        node_count = 0;
        key_comp = x.key_comp;
        if(x.root()) {
            root() = copy(x.root(), header);
            leftmost() = node_t::minimum(root());
            rightmost() = node_t::maximum(root());
            node_count = x.node_count;
        } else {
            root() = nullptr;
            leftmost() = header;
            rightmost() = header;
        }
    }
    return *this;
}

template<class Key, class Value, class KeyOfValue, 
         class Compare, class Alloc>
bs_tree<Key, Value, KeyOfValue, Compare, Alloc>&
bs_tree<Key, Value, KeyOfValue, Compare, Alloc>::
operator==(bs_tree&& x) {
    if(&x != this) {
        clear();
        node_count = 0;
        key_comp = x.key_comp;
        if(x.root()) {
            header = x.header;
            node_count = x.node_count;
        } else {
            root() = nullptr;
            leftmost() = header;
            rightmost() = header;
        }
        x.node_count = 0;
        x.header = nullptr;
    }
    return *this;
}

template<class Key, class Value, class KeyOfValue, 
         class Compare, class Alloc>
typename bs_tree<Key, Value, KeyOfValue, Compare, Alloc>::node_ptr_t
bs_tree<Key, Value, KeyOfValue, Compare, Alloc>::
copy(node_ptr_t x, node_ptr_t p) {
    node_ptr_t top = clone_node(x);
    top->parent = p;

    try {
        // copy right child recursively
        if(x->right)
            top->right = copy(x->right, top);
        
        p = top;
        x = x->left;

        // copy left child iteratively
        while(x) {
            node_ptr_t y = clone_node(x);
            p->left = y;
            y->parent = p;
            if(x->right)
                y->right = copy(x->right, y);
            p = y;
            x = x->left;
        }
    } catch(std::exception&) {
        erase(top);
        throw;
    }
    return top;
}


template<class Key, class Value, class KeyOfValue, 
         class Compare, class Alloc>
typename bs_tree<Key, Value, KeyOfValue, Compare, Alloc>::node_ptr_t
bs_tree<Key, Value, KeyOfValue, Compare, Alloc>::
erase_aux(node_ptr_t z, node_ptr_t& root, 
                node_ptr_t& leftmost, node_ptr_t& rightmost) {
    node_ptr_t y = z;
    node_ptr_t x = nullptr; // x one of y's child, may be null
    node_ptr_t x_parent = nullptr; 

    if(y->left) { // z has at least one child
        if(y->right) { // z has two child
            y = y->right; // let y be z's successor
            while(y->left)
                y = y->left;
            x = y->right; // x may be null
        } else // z only has left
            x = y->left;
    } else // z has at most one child
        x = y->right; // x may be null
    
    if(y != z) { // z has successor y, relink y in place of z
        z->left->parent = y;
        y->left = z->left;
        if(y == z->right)
            x_parent = y;
        else { // y is not right of z directly
            x_parent = y->parent; 
            if(x)
                x->parent = y->parent; // set x's parent = y's parent
            x->parent->left = x; // y is y's parent 's left
            y->right = z->right;
            z->right->parent = y;
        }

        // revise parent
        if(root == z)
            root = y;
        else if(z->parent->left == z)
            z->parent->left = y;
        else
            z->parent->right = y;

        y->parent = z->parent;
        swap(y->color, z->color);
        y = z; // y now points to node to be actually deleted
    } else { // y == z, z has no successor
        x_parent = y->parent;
        if(x)
            x->parent = y->parent;
        
        if(root == z)
            root = x; // set root be x, one of child of z
        else if(z->parent->left == z)
            z->parent->left = x;
        else 
            z->parent->right = x;

        if(leftmost == z) {
            if(z->right) // x is z's right, set leftmost min(x)
                leftmost = node_t::minimum(x);
            else   // z has no child
                leftmost = z->parent; // set leftmost z->parent
        }
        if(rightmost == z) {
            if(z->left)
                rightmost = node_t::maximum(x);
            else
                rightmost = z->parent;
        }
    }

    // now y has replaced z, x has replaced previous y
    return y;
}


template<class Key, class Value, class KeyOfValue, 
         class Compare, class Alloc>
typename bs_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
bs_tree<Key, Value, KeyOfValue, Compare, Alloc>::
insert_aux(node_ptr_t x, node_ptr_t y, const Value& val) {
    node_ptr_t z = create_node(val);

    if(y == header || x || key_comp(key(z), key(y))) {
        y->left = z; // if y = header, set leftmost = z;

        if(y == header) {
            root() = z;
            rightmost() = z;
        } else if(y == leftmost()) 
            leftmost() = z;
    } else {
        y->right = z;
        if(y == rightmost())
            rightmost() = z;
    }
    z->parent = y;
    z->left = nullptr;
    z->right = nullptr;
    ++node_count;
    return iterator(z);
}

template<class Key, class Value, class KeyOfValue, 
         class Compare, class Alloc>
typename bs_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
bs_tree<Key, Value, KeyOfValue, Compare, Alloc>::
insert_aux(node_ptr_t x, node_ptr_t y, Value&& val) {
    node_ptr_t z = create_node(std::move(val));

    if(y == header || x || key_comp(key(z), key(y))) {
        y->left = z;

        if(y == header) {
            root() = z;
            rightmost() = z;
        } else if(y == leftmost()) 
            leftmost() = z;
    } else {
        y->right = z;
        if(y == rightmost())
            rightmost() = z;
    }
    z->parent = y;
    z->left = nullptr;
    z->right = nullptr;
    ++node_count;
    return iterator(z);
}

template <class Key, class Value, class KeyOfValue, 
          class Compare, class Alloc>
pair<typename bs_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator, bool> 
bs_tree<Key, Value, KeyOfValue, Compare, Alloc>::
insert_unique(const Value& val) {
    node_ptr_t y = header;
    node_ptr_t x = root();

    bool comp = true;
    while(x) {
        y = x;
        comp = key_comp(KeyOfValue()(val), key(x));
        x = comp ? x->left : x->right;
    }
    
    // y is insert pos
    iterator i(y);
    if(comp) { // val < y, new node will at left tree
        if(i == begin()) 
        // case1: y is leftmost, insert new leftmost
            return make_pair(insert_aux(x, y, val), true);
        else
            --i; // check the prev
    }
    // case2: val >= y and y < val, not equal
    // case3: prev(y) < val < y , not equal
    if(key_comp(key(i.node), KeyOfValue()(val)))
        return make_pair(insert_aux(x, y, val), true);
    // get there, val equals to y or --y;
    return make_pair(i, false);
}

template <class Key, class Value, class KeyOfValue, 
          class Compare, class Alloc>
pair<typename bs_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator, bool> 
bs_tree<Key, Value, KeyOfValue, Compare, Alloc>::insert_unique(Value&& val) {
    node_ptr_t y = header;
    node_ptr_t x = root();

    bool comp = true;
    while(x) {
        y = x;
        comp = key_comp(KeyOfValue()(val), key(x));
        x = comp ? x->left : x->right;
    }
    
    iterator i(y);
    if(comp) {
        if(i == begin())
            return make_pair(insert_aux(x, y, std::move(val)), true);
        else
            --i;
    }
    if(key_comp(key(i.node), KeyOfValue()(val)))
        return make_pair(insert_aux(x, y, std::move(val)), true);
    return make_pair(i, false);
}

// inserts value in the pos as close as possible, 
// pos used as a suggestion as to where to start the search
template <class Key, class Value, class KeyOfValue, 
          class Compare, class Alloc>
typename bs_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
bs_tree<Key, Value, KeyOfValue, Compare, Alloc>::
insert_unique(iterator pos, const Value& val) {
    if(pos.node == leftmost()) { // case1: pos = begin()
        if(size() > 0 && 
           key_comp(KeyOfValue()(val), key(pos.node)))
           // need size() > 0 because pos may be header
           // header's key has no meaning
           return insert_aux(pos.node, pos.node, val);
        else
            insert_unique(val).first;
    } else if(pos.node == header) { // case2: pos = end()
        if(key_comp(key(rightmost()), KeyOfValue()(val)))
            return insert_aux(nullptr, rightmost(), val);
        else
            insert_unique(val).first;
    } else {
        iterator prev = pos;
        --prev;
        // prev < val < pos
        if(key_comp(key(prev.node), KeyOfValue()(val)) &&
            key_comp(KeyOfValue()(val), key(pos.node))) {
            if(prev.node->right)
                return insert_aux(pos.node, pos.node, val);
            else
                return insert_aux(nullptr, prev.node, val);
        } else
            return insert_unique(val).first;
    }
}

template <class Key, class Value, class KeyOfValue, 
          class Compare, class Alloc>
typename bs_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
bs_tree<Key, Value, KeyOfValue, Compare, Alloc>::
insert_unique(iterator pos, Value&& val) {
    if(pos.node == leftmost()) {
        if(size() > 0 && 
           key_comp(KeyOfValue()(val), key(pos.node)))
           return insert_aux(pos.node, pos.node, std::move(val));
        else
            insert_unique(std::move(val)).first;
    } else if(pos.node == header) {
        if(key_comp(key(rightmost()), KeyOfValue()(val)))
            return insert_aux(nullptr, rightmost(), std::move(val));
        else
            insert_unique(std::move(val)).first;
    } else {
        iterator prev = pos;
        --prev;
        if(key_comp(key(prev.node), KeyOfValue()(val)) &&
            key_comp(KeyOfValue()(val), key(pos.node))) {
            if(prev.node->right)
                return insert_aux(pos.node, pos.node, std::move(val));
            else
                return insert_aux(nullptr, prev.node, std::move(val));
        } else
            return insert_unique(std::move(val)).first;
    }
}


template <class Key, class Value, class KeyOfValue, 
          class Compare, class Alloc>
typename bs_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
bs_tree<Key, Value, KeyOfValue, Compare, Alloc>::
insert_equal(const Value& val) {
    node_ptr_t y = header;
    node_ptr_t x = root();
    while(x) {
        y = x;
        x = key_comp(KeyOfValue()(val), key(x)) ?
            x->left : x->right;
    }
    return insert_aux(x, y, val);
}


template <class Key, class Value, class KeyOfValue, 
          class Compare, class Alloc>
typename bs_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
bs_tree<Key, Value, KeyOfValue, Compare, Alloc>::
insert_equal(Value&& val) {
    node_ptr_t y = header;
    node_ptr_t x = root();
    while(x) {
        y = x;
        x = key_comp(KeyOfValue()(val), key(x)) ?
            x->left : x->right;
    }
    return insert_aux(x, y, std::move(val));
}

template <class Key, class Value, class KeyOfValue, 
          class Compare, class Alloc>
typename bs_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
bs_tree<Key, Value, KeyOfValue, Compare, Alloc>::
insert_equal(iterator pos, const Value& val) {
    if(pos.node == leftmost()) { // case1: pos = begin()
        if(size() > 0 && 
           !key_comp(key(pos.node)), KeyOfValue()(val))
           // case: pos >= val
           // need size() > 0 because pos may be header
           // header's key has no meaning
           return insert_aux(pos.node, pos.node, val);
        else
            insert_equal(val);
    } else if(pos.node == header) { // case2: pos = end()
        if(!key_comp(KeyOfValue()(val)), key(rightmost()))
            // case: val >= rightmost
            return insert_aux(nullptr, rightmost(), val);
        else
            insert_equal(val);
    } else {
        iterator prev = pos;
        --prev;
        // case: prev <= val <= pos
        if(!key_comp(KeyOfValue()(val), key(prev.node)) &&
            !key_comp(key(pos.node), KeyOfValue()(val))) {
            if(prev.node->right)
                return insert_aux(pos.node, pos.node, val);
            else
                return insert_aux(nullptr, prev.node, val);
        } else
            return insert_equal(val).first;
    }
}

template <class Key, class Value, class KeyOfValue, 
          class Compare, class Alloc>
typename bs_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
bs_tree<Key, Value, KeyOfValue, Compare, Alloc>::
insert_equal(iterator pos, Value&& val) {
    if(pos.node == leftmost()) {
        if(size() > 0 && 
           !key_comp(key(pos.node)), KeyOfValue()(val))
           return insert_aux(pos.node, pos.node, std::move(val));
        else
            insert_equal(std::move(val));
    } else if(pos.node == header) {
        if(!key_comp(KeyOfValue()(val)), key(rightmost()))
            return insert_aux(nullptr, rightmost(), std::move(val));
        else
            insert_equal(std::move(val));
    } else {
        iterator prev = pos;
        --prev;
        if(!key_comp(KeyOfValue()(val), key(prev.node)) &&
            !key_comp(key(pos.node), KeyOfValue()(val))) {
            if(prev.node->right)
                return insert_aux(pos.node, pos.node, std::move(val));
            else
                return insert_aux(nullptr, prev.node, std::move(val));
        } else
            return insert_equal(std::move(val)).first;
    }
}



} // MiniSTL