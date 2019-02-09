#pragma once   

#include "hashtable.hpp"
#include "Function/function.hpp"

namespace MiniSTL {
    
    
template <class Key, class T, class HashFunc, class EqualKey, 
          class Alloc = simple_alloc<pair<const Key, T>>>
class hash_multimap {
private:
    using Ht = hashtable<pair<const Key, T>, Key, HashFunc,
                         select1st<pair<const Key, T>>, EqualKey, Alloc>;
    Ht ht;

public:
    using key_type = typename Ht::key_type;
    using data_type = T;
    using mapped_type = T;
    using value_type = typename Ht::value_type;
    using hasher = typename Ht::hasher;
    using key_equal = typename Ht::key_equal;
    
    using size_type = typename Ht::size_type;
    using difference_type = typename Ht::difference_type;
    using pointer = typename Ht::pointer;
    using const_pointer = typename Ht::const_pointer;
    using reference = typename Ht::reference;
    using const_reference = typename Ht::const_reference;

    using iterator = typename Ht::iterator;
    using const_iterator = typename Ht::const_iterator;

    using allocator_type = typename Ht::allocator_type;

    hasher hash_funct() const { return ht.hash_funct(); }
    key_equal key_eq() const { return ht.key_eq(); }
    allocator_type get_allocator() const { return ht.get_allocator(); }

    template <class K1, class T1, class HF, class Eq, class Al>
    friend bool operator== (const hash_multimap<K1, T1, HF, Eq, Al>&,
                            const hash_multimap<K1, T1, HF, Eq, Al>&);
public:
hash_multimap() : ht(100, hasher(), key_equal()) {}
    explicit hash_multimap(size_type n)
        : ht(n, hasher(), key_equal()) {}
    hash_multimap(size_type n, const hasher& hf)
        : ht(n, hf, key_equal()) {}
    hash_multimap(size_type n, const hasher& hf, const key_equal& eql)
        : ht(n, hf, eql) {}


    template <class InputIt>
    hash_multimap(InputIt f, InputIt l)
        : ht(100, hasher(), key_equal())
        { ht.insert_equal(f, l); }
    template <class InputIt>
    hash_multimap(InputIt f, InputIt l, size_type n)
        : ht(n, hasher(), key_equal())
        { ht.insert_equal(f, l); }
    template <class InputIt>
    hash_multimap(InputIt f, InputIt l, size_type n, const hasher& hf)
        : ht(n, hf, key_equal())
        { ht.insert_equal(f, l); }
    
    template <class InputIt>
    hash_multimap(InputIt f, InputIt l, size_type n,
            const hasher& hf, const key_equal& eql)
        : ht(n, hf, eql)
        { ht.insert_equal(f, l); }

    hash_multimap(std::initializer_list<pair<const Key, T>> ilist)
        : ht(ilist.size(), hasher(), key_equal()) 
        { ht.insert_equal(ilist.begin(), ilist.end()); }

public: // size
    size_type size() const { return ht.size(); }
    size_type max_size() const { return ht.max_size(); }
    bool empty() const { return ht.empty(); }

    void resize(size_type hint) { ht.resize(hint); }
    size_type bucket_count() const { return ht.bucket_count(); }
    size_type max_bucket_count() const { return ht.max_bucket_count(); }
    size_type elems_in_bucket(size_type n) const
        { return ht.elems_in_bucket(n); }

    iterator begin() { return ht.begin(); }
    iterator end() { return ht.end(); }
    const_iterator begin() const { return ht.begin(); }
    const_iterator end() const { return ht.end(); }

public: // insert
    pair<iterator,bool> insert(const value_type& obj)
        { return ht.insert_equal(obj); }
    
    template <class InputIt>
    void insert(InputIt f, InputIt l)
        { ht.insert_equal(f,l); }
    
    void insert(const value_type* f, const value_type* l) {
        ht.insert_equal(f,l);
    }

    void insert(const_iterator f, const_iterator l)
        { ht.insert_equal(f, l); }
    
    pair<iterator,bool> insert_noresize(const value_type& obj)
        { return ht.insert_unique_noresize(obj); }    

public: // find
    iterator find(const key_type& key) { return ht.find(key); }

    const_iterator find(const key_type& key) const 
        { return ht.find(key); }

    // map[key] semantics: if key exists, return value;else insert (key, T());
    T& operator[](const key_type& key) {
        return ht.find_or_insert(value_type(key, T())).second;
    }

    size_type count(const key_type& key) const { return ht.count(key); }
    
    pair<iterator, iterator> equal_range(const key_type& key)
        { return ht.equal_range(key); }
    
    pair<const_iterator, const_iterator>
    equal_range(const key_type& key) const
        { return ht.equal_range(key); }

public: // erase
    size_type erase(const key_type& key) {return ht.erase(key); }
    void erase(iterator it) { ht.erase(it); }
    void erase(iterator f, iterator l) { ht.erase(f, l); }
    void clear() { ht.clear(); }

    void swap(hash_multimap& hm) { ht.swap(hm.ht); }
};

template <class Key, class T, class HashFunc, class EqlKey, class Alloc>
inline bool 
operator==(const hash_multimap<Key,T,HashFunc,EqlKey,Alloc>& hm1,
           const hash_multimap<Key,T,HashFunc,EqlKey,Alloc>& hm2) {
    return hm1.ht == hm2.ht;
}

template <class Key, class T, class HashFunc, class EqlKey, class Alloc>
inline bool 
operator!=(const hash_multimap<Key,T,HashFunc,EqlKey,Alloc>& hm1,
           const hash_multimap<Key,T,HashFunc,EqlKey,Alloc>& hm2) {
    return !(hm1 == hm2);
}

template <class Key, class T, class HashFunc, class EqlKey, class Alloc>
inline void 
swap(hash_multimap<Key,T,HashFunc,EqlKey,Alloc>& hm1,
     hash_multimap<Key,T,HashFunc,EqlKey,Alloc>& hm2) {
    hm1.swap(hm2);
}


// Specialization of insert_iterator so that it will work for hash_multimap

template <class Key, class T, class HF,  class Eq, class Alloc>
class insert_iterator<hash_multimap<Key, T, HF, Eq, Alloc> > {
protected:
    using Container = hash_multimap<Key, T, HF, Eq, Alloc>;
    Container* c;
public:

    using container_type = Container;
    using iterator_category = output_iterator_tag;
    using value_type = void;
    using difference_type = void;
    using pointer = void;
    using reference = void;

    insert_iterator(Container& x) : c(&x) {}
    insert_iterator(Container& x, typename Container::iterator)
        : c(&x) {}
    
    insert_iterator<Container>&
    operator=(const typename Container::value_type& val) { 
        c->insert(val);
        return *this;
    }

    insert_iterator<Container>& operator*() { return *this; }
    insert_iterator<Container>& operator++() { return *this; }
    insert_iterator<Container>& operator++(int) { return *this; }
};

} // MiniSTL