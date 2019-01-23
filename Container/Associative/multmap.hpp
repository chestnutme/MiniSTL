#pragma

#include "rb_tree.hpp"
#include "Function/function.hpp"

#include <initializer_list>


namespace MiniSTL {
    
template <class Key, class T, class Compare = less<Key>,
          class Allocator = simple_alloc<pair<const Key, T>> >
class multimap {
public:
    // types alias
    using key_type = Key;
    using map_type = T;
    using value_type = pair<const Key, T>;
    using key_compare = Compare;
    using allocator_type = Allocator;
    using size_type	= size_t;
    using difference_type = ptrdiff_t;
    using reference	= value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using iterator = rb_tree_iterator<value_type, value_type&, value_type*>;
    using const_iterator = rb_tree_iterator<value_type, const value_type&, const value_type*>;
    using reverse_iterator	= __reverse_iterator<iterator>;
    using const_reverse_iterator = __reverse_iterator<const_iterator>;

    class value_compare : public binary_function<value_type, value_type, bool> {
		friend class multimap;
	protected:
		Compare comp;
		value_compare(Compare c) : comp(c) {}
	public:
        typedef bool        result_type;
        typedef value_type  first_argument_type;
        typedef value_type  second_argument_type;
        bool operator()(const value_type& x, const value_type& y) const {
            return comp(x.first, y.first);
        }
	};

protected:
    using impl_type = rb_tree<key_type, value_type,     
                              select1st<value_type>, 
                              Compare, Alloccator>;
	impl_type impl;
 
    // construct/copy/destroy:
    explicit multimap(const Compare& comp = Compare()));

    template <class InputIt>
    multimap(InputIt first, InputIt last, const Compare& comp = Compare());
    multimap(const multimap& x);
    multimap(multimap&& x);
    multimap(initializer_list<value_type> ilist, const Compare& = Compare());
    ~multimap();
 
    multimap& operator=(const multimap& x);
    multimap& operator=(multimap&& x);
    multimap& operator=(initializer_list<value_type> ilist);
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
    
    // element access:
    T& operator[](const key_type& x);
    T& operator[](key_type&& x);
    T&       at(const key_type& x);
    const T& at(const key_type& x) const;

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
 
    void swap(multimap& x);
    void clear() noexcept;
 
    // observers:
    key_compare   key_comp() const;
    value_compare value_comp() const;
 
    // multimap operations:
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

template <class Key, class T, class Compare, class Alloc>
bool operator==(const multimap<Key,T,Compare,Alloc>& x,
                const multimap<Key,T,Compare,Alloc>& y);
template <class Key, class T, class Compare, class Alloc>
bool operator< (const multimap<Key,T,Compare,Alloc>& x,
                const multimap<Key,T,Compare,Alloc>& y);
template <class Key, class T, class Compare, class Alloc>
bool operator!=(const multimap<Key,T,Compare,Alloc>& x,
                const multimap<Key,T,Compare,Alloc>& y);
template <class Key, class T, class Compare, class Alloc>
bool operator> (const multimap<Key,T,Compare,Alloc>& x,
                const multimap<Key,T,Compare,Alloc>& y);
template <class Key, class T, class Compare, class Alloc>
bool operator>=(const multimap<Key,T,Compare,Alloc>& x,
                const multimap<Key,T,Compare,Alloc>& y);
template <class Key, class T, class Compare, class Alloc>
bool operator<=(const multimap<Key,T,Compare,Alloc>& x,
                const multimap<Key,T,Compare,Alloc>& y);

template <class Key, class T, class Compare, class Alloc>
void swap(multimap<Key,T,Compare,Alloc>& x, multimap<Key,T,Compare,Alloc>& y);


} // MiniSTL
