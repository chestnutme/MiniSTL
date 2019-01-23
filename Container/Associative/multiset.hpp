#pragma

#include "rb_tree.hpp"
#include "Function/function.hpp"

#include <initializer_list>


namespace MiniSTL {
    
template <class Key, class Compare = less<Key>,
          class Allocator = simple_alloc<Key> >
class multiset {
public:
    // types alias
    using key_type = Key;
    using value_type = Key;
    using key_compare = Compare;
    using value_compare = Compare;
    using allocator_type = Allocator;
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

protected:
    using impl_type = rb_tree<key_type, value_type, identity<value_type>, Compare, Alloccator>;
	impl_type impl;
 
    // construct/copy/destroy:
    explicit multiset(const Compare& comp = Compare()));

    template <class InputIt>
    multiset(InputIt first, InputIt last, const Compare& comp = Compare());
    multiset(const multiset& x);
    multiset(multiset&& x);
    multiset(initializer_list<value_type> ilist, const Compare& = Compare());
    ~multiset();
 
    multiset& operator=(const multiset& x);
    multiset& operator=(multiset&& x);
    multiset& operator=(initializer_list<value_type> ilist);
    allocator_type get_allocator() const noexcept;
 
    // iterators:
    iterator        begin() noexcept;
    const_iterator  begin() const noexcept;
    iterator        end() noexcept;
    const_iterator  end() const noexcept;
 
    reverse_iterator        rbegin() noexcept;
    const_reverse_iterator  rbegin() const noexcept;
    reverse_iterator        rend() noexcept;
    const_reverse_iterator  rend() const noexcept;
 
    const_iterator          cbegin() const noexcept;
    const_iterator          cend() const noexcept;
    const_reverse_iterator  crbegin() const noexcept;
    const_reverse_iterator  crend() const noexcept;
 
    // capacity:
    bool        empty() const noexcept;
    size_type   size() const noexcept;
    size_type   max_size() const noexcept;
 
    // modifiers:
    template <class... Args> 
    pair<iterator, bool> emplace(Args&&... args);
    template <class... Args> 
    iterator emplace_hint(const_iterator pos, Args&&... args);
 
    pair<iterator,bool> insert(const value_type& x);
    pair<iterator,bool> insert(value_type&& x);
    iterator insert(const_iterator pos, const value_type& x);
    iterator insert(const_iterator pos, value_type&& x);
    template <class InputIt>
    void insert(InputIt first, InputIt last);
    void insert(initializer_list<value_type> ilist);
 
    iterator erase(const_iterator pos);
    size_type erase(const key_type& x);
    iterator erase(const_iterator first, const_iterator last);
 
    void swap(multiset& x);
    void clear() noexcept;
 
    // observers:
    key_compare   key_comp() const;
    value_compare value_comp() const;
 
    // multiset operations:
    iterator       find(const key_type& x);
    const_iterator find(const key_type& x) const;

    size_type      count(const key_type& x) const;

    iterator       lower_bound(const key_type& x);
    const_iterator lower_bound(const key_type& x) const;
    iterator       upper_bound(const key_type& x);
    const_iterator upper_bound(const key_type& x) const;

    pair<iterator, iterator> 
    equal_range(const key_type& x);
    pair<const_iterator, const_iterator> 
    equal_range(const key_type& x) const;
};

template <class Key, class Compare, class Alloc>
bool operator==(const multiset<Key,Compare,Alloc>& x,
                const multiset<Key,Compare,Alloc>& y);
template <class Key, class Compare, class Alloc>
bool operator< (const multiset<Key,Compare,Alloc>& x,
                const multiset<Key,Compare,Alloc>& y);
template <class Key, class Compare, class Alloc>
bool operator!=(const multiset<Key,Compare,Alloc>& x,
                const multiset<Key,Compare,Alloc>& y);
template <class Key, class Compare, class Alloc>
bool operator> (const multiset<Key,Compare,Alloc>& x,
                const multiset<Key,Compare,Alloc>& y);
template <class Key, class Compare, class Alloc>
bool operator>=(const multiset<Key,Compare,Alloc>& x,
                const multiset<Key,Compare,Alloc>& y);
template <class Key, class Compare, class Alloc>
bool operator<=(const multiset<Key,Compare,Alloc>& x,
                const multiset<Key,Compare,Alloc>& y);

template <class Key, class Compare, class Alloc>
void swap(multiset<Key,Compare,Alloc>& x, multiset<Key,Compare,Alloc>& y);


} // MiniSTL
