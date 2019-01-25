#pragma once

#include "rb_tree.hpp"
#include "Function/function.hpp"

#include <initializer_list>


namespace MiniSTL {
    
template <class Key, class Compare = less<Key>,
          class Allocator = simple_alloc<Key> >
class multiset {
protected:
    using impl_t = rb_tree<Key, Key, identity<Key>, Compare, Allocator>;
	impl_t impl;

public:
    // types alias
    using key_type = Key;
    using value_type = Key;
    using key_compare = Compare;
    using value_compare = Compare;
    using allocator_type = Allocator;

    using size_type	= typename impl_t::size_type;
    using difference_type = typename impl_t::difference_type;
    using reference	= typename impl_t::reference;
    using const_reference = typename impl_t::const_reference;
    using pointer = typename impl_t::pointer;
    using const_pointer = typename impl_t::const_pointer;
    using iterator = typename impl_t::iterator;
    using const_iterator = typename impl_t::const_iterator;
    using reverse_iterator	= typename impl_t::reverse_iterator;
    using const_reverse_iterator = typename impl_t::const_reverse_iterator;

public:
    // construct/copy/destroy:
    explicit multiset(const Compare& comp = Compare()) : impl(comp) {}

    template <class InputIt>
    multiset(InputIt first, InputIt last, const Compare& comp = Compare()) 
        : impl(comp) { impl.insert_equal(first, last); }
    
    multiset(const multiset& x) : impl(x.impl) {}
    multiset(multiset&& x) : impl(std::move(impl)) {}
    multiset(std::initializer_list<value_type> ilist, const Compare& comp = Compare())
        : impl(comp) { impl.insert_equal(ilist.begin(), ilist.end()); }
    ~multiset() {}

    // assign
    multiset& operator=(const multiset& x) { impl = x.impl; return *this; }
    multiset& operator=(multiset&& x) { impl = std::move(impl); return *this; }
    multiset& operator=(std::initializer_list<value_type> ilist) {
        impl.clear();
        impl.insert_equal(ilist.begin(), ilist.end());
        return *this;
    }

    allocator_type get_allocator() const noexcept { return allocator_type(); }
 
    // iterators:
    iterator        begin() noexcept { return impl.begin(); }
    const_iterator  begin() const noexcept { return impl.begin(); }
    iterator        end() noexcept { return impl.end(); }
    const_iterator  end() const noexcept { return impl.end(); }
 
    reverse_iterator        rbegin() noexcept { return impl.rbegin(); }
    const_reverse_iterator  rbegin() const noexcept { return impl.rbegin(); }
    reverse_iterator        rend() noexcept { return impl.rend(); }
    const_reverse_iterator  rend() const noexcept { return impl.rend(); }
 
    const_iterator          cbegin() const noexcept { return impl.cbegin(); }
    const_iterator          cend() const noexcept { return impl.cend(); }
    const_reverse_iterator  crbegin() const noexcept { return impl.crbegin(); }
    const_reverse_iterator  crend() const noexcept { return impl.crend(); }
 
    // capacity:
    bool        empty() const noexcept { return impl.empty(); }
    size_type   size() const noexcept { return impl.size(); }
    size_type   max_size() const noexcept { return impl.max_size(); }
 
    // modifiers:
    template <class... Args> 
    pair<iterator, bool> emplace(Args&&... args)
        { return impl.insert_equal(std::move(value_type(args...))); }
    
    template <class... Args> 
    iterator emplace_hint(const_iterator pos, Args&&... args)
        { return impl.insert_equal(pos, std::move(std::move(value_type(args...)))); }
 
    pair<iterator,bool> insert(const value_type& x) 
    { return impl.insert_equal(x); }

    pair<iterator,bool> insert(value_type&& x)
        { return impl.insert_equal(std::move(x)); }
    iterator insert(const_iterator pos, const value_type& x) {
        return impl.insert_equal(pos, x);
    }
    iterator insert(const_iterator pos, value_type&& x) 
        { return impl.insert_equal(pos, std::move(x)); }

    template <class InputIt>
    void insert(InputIt first, InputIt last) 
        { impl.insert_equal(first, last); }
    void insert(std::initializer_list<value_type> ilist) 
        { impl.insert_equal(ilist.begin(), ilist.end()); }
       
    iterator erase(const_iterator pos) { return impl.erase(pos); }
    size_type erase(const key_type& x) { return impl.erase(x); }
    iterator erase(const_iterator first, const_iterator last)
        { return erase(first, last); }
 
    void swap(multiset& x) { impl.swap(x.impl); }
    void clear() noexcept { impl.clear(); }
 
    // observers:
    key_compare   key_comp() const { return key_compare(); }
    value_compare value_comp() const { return value_compare(); }
 
    // multiset operations:
    iterator       find(const key_type& x) { return impl.find(x); }
    const_iterator find(const key_type& x) const 
        { return impl.find(x); }

    size_type      count(const key_type& x) const 
        { return impl.count(x); }

    iterator       lower_bound(const key_type& x)
        { return impl.lower_bound(x); }
    const_iterator lower_bound(const key_type& x) const
        { return impl.lower_bound(x); }
    iterator       upper_bound(const key_type& x) 
        { return impl.upper_bound(x); }
    const_iterator upper_bound(const key_type& x) const
        { return impl.upper_bound(x); }

    pair<iterator, iterator> 
    equal_range(const key_type& x) 
        { return impl.equal_range(x); }
    pair<const_iterator, const_iterator> 
    equal_range(const key_type& x) const 
        { return impl.equal_range(x); }
};

template <class Key, class Compare, class Alloc>
bool operator==(const multiset<Key,Compare,Alloc>& x, const multiset<Key,Compare,Alloc>& y) {
    return x.impl == y.impl;
}
template <class Key, class Compare, class Alloc>
bool operator< (const multiset<Key,Compare,Alloc>& x, const multiset<Key,Compare,Alloc>& y) {
    return x.impl != y.impl;
}
template <class Key, class Compare, class Alloc>
bool operator!=(const multiset<Key,Compare,Alloc>& x, const multiset<Key,Compare,Alloc>& y) {
    return x.impl != y.impl;
}
template <class Key, class Compare, class Alloc>
bool operator> (const multiset<Key,Compare,Alloc>& x, const multiset<Key,Compare,Alloc>& y) {
    return x.impl > y.impl;
}
template <class Key, class Compare, class Alloc>
bool operator>=(const multiset<Key,Compare,Alloc>& x, const multiset<Key,Compare,Alloc>& y) {
    return x.impl >= y.impl;
}
template <class Key, class Compare, class Alloc>
bool operator<=(const multiset<Key,Compare,Alloc>& x, const multiset<Key,Compare,Alloc>& y) {
    return x.impl <= y.impl;
}

template <class Key, class Compare, class Alloc>
void swap(multiset<Key,Compare,Alloc>& x, multiset<Key,Compare,Alloc>& y) {
    x.impl(y.impl);
}

} // MiniSTL
