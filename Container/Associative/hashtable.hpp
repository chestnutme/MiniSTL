#pragma once

#include "Algorithms/algo.hpp"
#include "Allocator/memory.hpp"
#include "Util/tempbuf.hpp"
#include "Function/function.hpp"
#include "Container/Sequence/vector.hpp"
#include "hash_fun.h"


namespace MiniSTL {

template <class Value>
struct hashtable_node {
    hashtable_node* next;
    Value val;
}; 

template <class Value, class Key, class HashFunc,
          class ExtractKey, class EqualKey, class Alloc = simple_alloc<Value>>
class hashtable;

template <class Value, class Key, class HashFunc,
          class ExtractKey, class EqualKey, class Alloc>
struct hashtable_iterator;

template <class Value, class Key, class HashFunc,
          class ExtractKey, class EqualKey, class Alloc>
struct hashtable_const_iterator;

template <class Value, class Key, class HashFunc,
          class ExtractKey, class EqualKey, class Alloc>
struct hashtable_iterator {
    using Hashtable = hashtable<Value,Key,HashFunc,ExtractKey,EqualKey,Alloc>;
    using iterator = hashtable_iterator<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>;
    using const_iterator = hashtable_const_iterator<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>;
    using node = hashtable_node<Value>;

    using iterator_category = forward_iterator_tag;
    using value_type = Value;
    using difference_type = ptrdiff_t;
    using size_type = size_t;
    using reference = Value&;
    using pointer = Value*;
    
    node* cur;
    Hashtable* ht;

    hashtable_iterator(node* n, Hashtable* t) 
        :cur(n), ht(t) {}
    
    hashtable_iterator() {}

    reference operator*() const { return cur->val;}
    pointer operator->() const { return &(operator*());}

    iterator& operator++() {
        const node* old = cur;
        cur = cur->next;
        if(!cur) {
            size_type bkt = ht->bkt_num(old->val);
            while(!cur && ++bkt < ht->buckets.size())
                cur = ht->buckets[bkt];
        }
        return *this;
    }
    iterator operator++(int) {
        iterator tmp = *this;
        ++*this;
        return tmp;
    }

    bool operator==(const iterator& it) const
        { return cur == it.cur;}
    bool operator!=(const iterator& it) const
        { return cur != it.cur;}
};


template <class Value, class Key, class HashFunc,
          class ExtractKey, class EqualKey, class Alloc>
struct hashtable_const_iterator {
    using Hashtable = hashtable<Value,Key,HashFunc,ExtractKey,EqualKey,Alloc>;
    using iterator = hashtable_iterator<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>;
    using const_iterator = hashtable_const_iterator<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>;
    using node = hashtable_node<Value>;

    using iterator_category = forward_iterator_tag;
    using value_type = Value;
    using difference_type = ptrdiff_t;
    using size_type = size_t;
    using reference = Value&;
    using pointer = Value*;
    
    const node* cur;
    const Hashtable* ht;

    hashtable_const_iterator(const node* n, const Hashtable* t)
        :cur(n), ht(t) {}
    hashtable_const_iterator() {}
    hashtable_const_iterator(const iterator& it) 
        :cur(it.cur), ht(it.ht) {}
    
    reference operator*() const { return cur->val;}
    pointer operator->() const { return &(operator*());}

    const_iterator& operator++() {
        const node* old = cur;
        cur = cur->next;
        if(!cur) {
            size_type bkt = ht->bkt_num(old->val);
            while(!cur && ++bkt < ht->buckets.size())
                cur = ht->buckets[bkt];
        }
        return *this;
    }

    const_iterator operator++(int) {
        const_iterator tmp = *this;
        ++*this;
        return tmp;
    }

    bool operator==(const const_iterator& it) const 
        { return cur == it.cur;}
    bool operator!=(const const_iterator& it) const 
        { return cur != it.cur;}
};

// Note: assumes long is at least 32 bits.

const int __num_primes = 28;
static const unsigned long __prime_list[__num_primes] =
{
  53ul,         97ul,         193ul,       389ul,       769ul,
  1543ul,       3079ul,       6151ul,      12289ul,     24593ul,
  49157ul,      98317ul,      196613ul,    393241ul,    786433ul,
  1572869ul,    3145739ul,    6291469ul,   12582917ul,  25165843ul,
  50331653ul,   100663319ul,  201326611ul, 402653189ul, 805306457ul, 
  1610612741ul, 3221225473ul, 4294967291ul
};

inline unsigned long __next_prime(unsigned long n)
{
  const unsigned long* first = __prime_list;
  const unsigned long* last = __prime_list + __num_primes;
  const unsigned long* pos = lower_bound(first, last, n);
  return pos == last ? *(last - 1) : *pos;
}


template <class Value, class Key, class HashFunc,
          class ExtractKey, class EqualKey, class Alloc>
class hashtable {
public:
    using key_type = Key;
    using value_type = Value;
    using hasher = HashFunc;
    using key_equal = EqualKey;

    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using pointer = Value*;
    using const_pointer = const Value*;
    using reference = Value&;
    using const_reference = const Value&;

    hasher hash_funct() const { return hash;}
    key_equal key_eq() const { return equals;}

public:
    using allocator_type = Alloc;
    allocator_type get_allocator() const { return allocator_type();}

private:
    using node = hashtable_node<Value>;
    using node_alloc = simple_alloc<node>;

    node* get_node() { return node_alloc::allocate(1);}
    void put_node(node* p) { node_alloc::deallocate(p, 1);}

    node* new_node(const value_type& obj) {
        node* n = get_node();
        n->next = nullptr;
        try {
            construct(&n->val, obj);
            return n;
        } catch(std::exception&) {
            put_node(n);
            throw;
        }
    }
    
    void delete_node(node* n) {
        destroy(&n->val);
        put_node(n);
    }

private:
    hasher          hash;
    key_equal       equals;
    ExtractKey      get_key;
    vector<node*>   buckets;
    size_type       num_elements;

public:
    using iterator = hashtable_iterator<Value,Key,HashFunc,ExtractKey,EqualKey,Alloc>;
    using const_iterator = hashtable_const_iterator<Value,Key,HashFunc,ExtractKey,EqualKey,
                                    Alloc>;

    friend struct
    hashtable_iterator<Value,Key,HashFunc,ExtractKey,EqualKey,Alloc>;
    friend struct
    hashtable_const_iterator<Value,Key,HashFunc,ExtractKey,EqualKey,Alloc>;

    template <class val, class Ky, class HF, class Ex, class Eq, class Al>
    friend bool operator== (const hashtable<val, Ky, HF, Ex, Eq, Al>&,
                            const hashtable<val, Ky, HF, Ex, Eq, Al>&);

private: // buckets related operation
    size_type next_size(size_type n) const
    { return __next_prime(n);}

    void initialize_buckets(size_type n) {
        const size_type n_buckets = next_size(n);
        buckets.reserve(n_buckets);
        buckets.insert(buckets.end(), n_buckets, nullptr);
        num_elements = 0;
    }

    void copy_from(const hashtable& ht);

public: // ctor, dtor
    hashtable(size_type n, const HashFunc& hf, const EqualKey& eql,
              const ExtractKey& ext)
        : hash(hf), equals(eql), get_key(ext), buckets(),
        num_elements(0) {
        initialize_buckets(n);
    }

    hashtable(size_type n, const HashFunc& hf, const EqualKey& eql)
        : hash(hf), equals(eql), get_key(ExtractKey()), buckets(),
        num_elements(0) {
        initialize_buckets(n);
    }

    hashtable(const hashtable& ht)
        : hash(ht.hash), equals(ht.equals), get_key(ht.get_key), buckets(),
        num_elements(0) {
        copy_from(ht);
    }

    hashtable& operator= (const hashtable& ht) {
        if(&ht != this) {
            clear();
            hash = ht.hash;
            equals = ht.equals;
            get_key = ht.get_key;
            copy_from(ht);
        }
        return *this;
    }

    ~hashtable() { clear();}

public: //
    iterator begin() { 
        for(size_type n = 0;n < buckets.size();++n) {
            if(buckets[n])
                return iterator(buckets[n], this);
        }
        return end();
    }

    iterator end() { return iterator(nullptr, this);}

    const_iterator begin() const
    {
        for(size_type n = 0;n < buckets.size();++n) {
            if(buckets[n])
                return const_iterator(buckets[n], this);
        }
        return end();
    }

    const_iterator end() const { return const_iterator(nullptr, this);}


public: // observor
    size_type bucket_count() const { return buckets.size();}

    size_type max_bucket_count() const
        { return __prime_list[__num_primes - 1];} 

    size_type elems_in_bucket(size_type bucket) const {
        size_type result = 0;
        for(node* cur = buckets[bucket];cur;cur = cur->next)
            result += 1;
        return result;
    }

    size_type size() const { return num_elements;}
    size_type max_size() const { return SIZE_MAX;}
    bool empty() const { return size() == 0;}


public: // insert
    pair<iterator, bool> insert_unique(const value_type& obj) {
        resize(num_elements + 1);
        return insert_unique_noresize(obj);
    }

    iterator insert_equal(const value_type& obj) {
        resize(num_elements + 1);
        return insert_equal_noresize(obj);
    }

    pair<iterator, bool> insert_unique_noresize(const value_type& obj);

    iterator insert_equal_noresize(const value_type& obj);
 
    template <class InputIt>
    void insert_unique(InputIt f, InputIt l) {
        insert_unique(f, l, iterator_category_t<InputIt>());
    }

    template <class InputIt>
    void insert_equal(InputIt f, InputIt l) {
        insert_equal(f, l, iterator_category_t<InputIt>());
    }

    template <class InputIt>
    void insert_unique(InputIt f, InputIt l,
                        input_iterator_tag) {
        for(;f != l;++f)
            insert_unique(*f);
    }

    template <class InputIt>
    void insert_equal(InputIt f, InputIt l,
                        input_iterator_tag) {
        for(;f != l;++f)
            insert_equal(*f);
    }

    template <class ForwardIt>
    void insert_unique(ForwardIt f, ForwardIt l,
                        forward_iterator_tag) {
        size_type n = distance(f, l);
        resize(num_elements + n);
        for(;n > 0;--n, ++f)
            insert_unique_noresize(*f);
    }

    template <class ForwardIt>
    void insert_equal(ForwardIt f, ForwardIt l,
                        forward_iterator_tag) {
        size_type n = distance(f, l);
        resize(num_elements + n);
        for(;n > 0;--n, ++f)
            insert_equal_noresize(*f);
    }

private:
    size_type bkt_num_key(const key_type& key) const{
        return bkt_num_key(key, buckets.size());
    }

    size_type bkt_num_key(const key_type& key, size_t n) const{
        return hash(key) % n;
    }

    size_type bkt_num(const value_type& obj) const{
        return bkt_num_key(get_key(obj));
    }

    size_type bkt_num(const value_type& obj, size_t n) const{
        return bkt_num_key(get_key(obj), n);
    }

public: // find
    reference find_or_insert(const value_type& obj);

    iterator find(const key_type& key) {
        size_type n = bkt_num_key(key);
        node* first;
        for(first = buckets[n];
            first && !equals(get_key(first->val), key);
            first = first->next) {}
        return iterator(first, this);
    } 

    const_iterator find(const key_type& key) const {
        size_type n = bkt_num_key(key);
        const node* first;
        for(first = buckets[n];
            first && !equals(get_key(first->val), key);
            first = first->next) {}
        return const_iterator(first, this);
    } 

    size_type count(const key_type& key) const {
        const size_type n = bkt_num_key(key);
        size_type result = 0;

        for(const node* cur = buckets[n];cur;cur = cur->next) {
            if(equals(get_key(cur->val), key))
                ++result;
        }
        return result;
    }

    pair<iterator, iterator> 
    equal_range(const key_type& key);

    pair<const_iterator, const_iterator> 
    equal_range(const key_type& key) const;

private:
    void erase_bucket(const size_type n, node* first, node* last);
    void erase_bucket(const size_type n, node* last);

public: // erase, resize, swap
    size_type erase(const key_type& key);
    void erase(const iterator& it);
    void erase(iterator first, iterator last);

    void erase(const const_iterator& it);
    void erase(const_iterator first, const_iterator last);

    void resize(size_type num_elements_hint);
    void clear();

    void swap(hashtable& ht) {
        MiniSTL::swap(hash, ht.hash);
        MiniSTL::swap(equals, ht.equals);
        MiniSTL::swap(get_key, ht.get_key);
        buckets.swap(ht.buckets);
        MiniSTL::swap(num_elements, ht.num_elements);
    }
};
 

template <class Value, class Key, class HF, class Ex, class Eq, class Al>
bool operator==(const hashtable<Value,Key,HF,Ex,Eq,Al>& ht1,
                const hashtable<Value,Key,HF,Ex,Eq,Al>& ht2) {
    using node = typename hashtable<Value,Key,HF,Ex,Eq,Al>::node;
    if(ht1.buckets.size() != ht2.buckets.size())
        return false;
    for(int n = 0;n < ht1.buckets.size();++n) {
        node* cur1 = ht1.buckets[n];
        node* cur2 = ht2.buckets[n];
        for(;cur1 && cur2 && cur1->val == cur2->val;
            cur1 = cur1->next, cur2 = cur2->next)
        {}
        if(cur1 || cur2)
            return false;
    }
    return true;
}  


template <class Value, class Key, class HF, class Ex, class Eq, class Al>
inline bool operator!=(const hashtable<Value,Key,HF,Ex,Eq,Al>& ht1,
                       const hashtable<Value,Key,HF,Ex,Eq,Al>& ht2) {
    return !(ht1 == ht2);
}

template <class Value, class Key, class HF, class Ex, class Eq, 
          class Al>
inline void swap(hashtable<Value, Key, HF, Ex, Eq, Al>& ht1,
                 hashtable<Value, Key, HF, Ex, Eq, Al>& ht2) {
    ht1.swap(ht2);
}


template <class Value, class Key, class HF, class Ex, class Eq, class Al>
pair<typename hashtable<Value,Key,HF,Ex,Eq,Al>::iterator, bool> 
hashtable<Value,Key,HF,Ex,Eq,Al>::insert_unique_noresize(const value_type& obj) {
    const size_type n = bkt_num(obj);
    node* first = buckets[n];

    for(node* cur = first;cur;cur = cur->next) {
        if(equals(get_key(cur->val), get_key(obj)))
            return make_pair(iterator(cur, this), false);
    }

    node* tmp = new_node(obj);
    tmp->next = first;
    buckets[n] = tmp;
    ++num_elements;
    return make_pair(iterator(tmp, this), true);
}

template <class Value, class Key, class HF, class Ex, class Eq, class Al>
typename hashtable<Value,Key,HF,Ex,Eq,Al>::iterator 
hashtable<Value,Key,HF,Ex,Eq,Al>::insert_equal_noresize(const value_type& obj) {
    const size_type n = bkt_num(obj);
    node* first = buckets[n];

    for(node* cur = first;cur;cur = cur->next) {
        if(equals(get_key(cur->val), get_key(obj))) {
            node* tmp = new_node(obj);
            tmp->next = cur->next;
            cur->next = tmp;
            ++num_elements;
            return iterator(tmp, this);
        }
    }

    node* tmp = new_node(obj);
    tmp->next = first;
    buckets[n] = tmp;
    ++num_elements;
    return iterator(tmp, this);
}

template <class Value, class Key, class HF, class Ex, class Eq, class Al>
typename hashtable<Value,Key,HF,Ex,Eq,Al>::reference 
hashtable<Value,Key,HF,Ex,Eq,Al>::find_or_insert(const value_type& obj) {
    resize(num_elements + 1);

    size_type n = bkt_num(obj);
    node* first = buckets[n];

    for(node* cur = first;cur;cur = cur->next) {
        if(equals(get_key(cur->val), get_key(obj)))
            return cur->val;
    }

    node* tmp = new_node(obj);
    tmp->next = first;
    buckets[n] = tmp;
    ++num_elements;
    return tmp->val;
}

template <class Value, class Key, class HF, class Ex, class Eq, class Al>
pair<typename hashtable<Value,Key,HF,Ex,Eq,Al>::iterator,
     typename hashtable<Value,Key,HF,Ex,Eq,Al>::iterator> 
hashtable<Value,Key,HF,Ex,Eq,Al>::equal_range(const key_type& key) {
    const size_type n = bkt_num_key(key);

    for(node* first = buckets[n];first;first = first->next) {
        if(equals(get_key(first->val), key)) {
            for(node* cur = first->next;cur;cur = cur->next) {
                if(!equals(get_key(cur->val), key))
                    return make_pair(iterator(first, this), iterator(cur, this));
            }
            for(size_type m = n + 1;m < buckets.size();++m) {
                if(buckets[m])
                    return make_pair(iterator(first, this), iterator(buckets[m], this));
            }
            return make_pair(iterator(first, this), end());
        }
    }
    return make_pair(end(), end());
}

template <class Value, class Key, class HF, class Ex, class Eq, class Al>
pair<typename hashtable<Value,Key,HF,Ex,Eq,Al>::const_iterator, 
     typename hashtable<Value,Key,HF,Ex,Eq,Al>::const_iterator> 
hashtable<Value,Key,HF,Ex,Eq,Al>::equal_range(const key_type& key) const {
    const size_type n = bkt_num_key(key);

    for(const node* first = buckets[n];first;first = first->next) {
        if(equals(get_key(first->val), key)) {
            for(const node* cur = first->next;cur;cur = cur->next) {
                if(!equals(get_key(cur->val), key))
                    return make_pair(const_iterator(first, this),
                                     const_iterator(cur, this));
            }
            for(size_type m = n + 1;m < buckets.size();++m) {
                if(buckets[m])
                    return make_pair(const_iterator(first, this),
                                     const_iterator(buckets[m], this));
            }
            return make_pair(const_iterator(first, this), end());
        }
    }
    return make_pair(end(), end());
}

template <class Value, class Key, class HF, class Ex, class Eq, class Al>
typename hashtable<Value,Key,HF,Ex,Eq,Al>::size_type 
hashtable<Value,Key,HF,Ex,Eq,Al>::erase(const key_type& key) {
    const size_type n = bkt_num_key(key);
    node* first = buckets[n];
    size_type erased = 0;

    if(first) {
        node* cur = first;
        node* next = cur->next;
        while(next) {
            if(equals(get_key(next->val), key)) {
                cur->next = next->next;
                delete_node(next);
                next = cur->next;
                ++erased;
                --num_elements;
            } else {
                cur = next;
                next = cur->next;
            }    
        }
        if(equals(get_key(first->val), key)) {
            buckets[n] = first->next;
            delete_node(first);
            ++erased;
            --num_elements;
        }
    }
    return erased;
}

template <class Value, class Key, class HF, class Ex, class Eq, class Al>
void hashtable<Value,Key,HF,Ex,Eq,Al>::erase(const iterator& it) {
    node* p = it.cur;
    if(p) {
        const size_type n = bkt_num(p->val);
        node* cur = buckets[n];

        if(cur == p) {
            buckets[n] = cur->next;
            delete_node(cur);
            --num_elements;
        } else {
            node* next = cur->next;
            while(next) {
                if(next == p) {
                    cur->next = next->next;
                    delete_node(next);
                    --num_elements;
                    break;
                } else {
                    cur = next;
                    next = cur->next;
                }
            }
        }
    }
}

template <class Value, class Key, class HF, class Ex, class Eq, class Al>
inline void
hashtable<Value,Key,HF,Ex,Eq,Al>::erase(const const_iterator& it) {
    erase(iterator(const_cast<node*>(it.cur),
                    const_cast<hashtable*>(it.ht)));
}


template <class Value, class Key, class HF, class Ex, class Eq, class Al>
void hashtable<Value,Key,HF,Ex,Eq,Al>::erase(iterator first, iterator last) {
    size_type f_bkt = first.cur ? 
        bkt_num(first.cur->val) : buckets.size();
    size_type l_bkt = last.cur ? 
        bkt_num(last.cur->val) : buckets.size();

    if(first.cur == last.cur)
        return;
    else if(f_bkt == l_bkt)
        erase_bucket(f_bkt, first.cur, last.cur);
    else {
        erase_bucket(f_bkt, first.cur, nullptr);
        for(size_type n = f_bkt + 1;n < l_bkt;++n)
            erase_bucket(n, nullptr);
        if(l_bkt != buckets.size())
            erase_bucket(l_bkt, last.cur);
    }
}

template <class Value, class Key, class HF, class Ex, class Eq, class Al>
inline void
hashtable<Value,Key,HF,Ex,Eq,Al>::erase(const_iterator first,
                                             const_iterator last) {
    erase(iterator(const_cast<node*>(first.cur),
                   const_cast<hashtable*>(first.ht)),
          iterator(const_cast<node*>(last.cur),
                   const_cast<hashtable*>(last.ht)));
}

template <class Value, class Key, class HF, class Ex, class Eq, class Al>
void hashtable<Value,Key,HF,Ex,Eq,Al>::resize(size_type num_elements_hint) {
    const size_type old_n = buckets.size();
    if(num_elements_hint > old_n) {
        const size_type n = next_size(num_elements_hint);
        if(n > old_n) {
            vector<node*> tmp(n, nullptr);
            try {
                for(size_type bucket = 0;bucket < old_n;++bucket) {
                    node* first = buckets[bucket];
                    while(first) {
                        size_type new_bkt = bkt_num(first->val, n);
                        buckets[bucket] = first->next;

                        first->next = tmp[new_bkt];
                        tmp[new_bkt] = first;

                        first = buckets[bucket];         
                    }
                }
                buckets.swap(tmp);
            } catch(std::exception&) {
                for(size_type bucket = 0;bucket < tmp.size();++bucket) {
                    while(tmp[bucket]) {
                        node* next = tmp[bucket]->next;
                        delete_node(tmp[bucket]);
                        tmp[bucket] = next;
                    }
                }
                throw;
            }
        }
    }
}

template <class Value, class Key, class HF, class Ex, class Eq, class Al>
void hashtable<Value,Key,HF,Ex,Eq,Al>
    ::erase_bucket(const size_type n, node* first, node* last) {
    node* cur = buckets[n];
    if(cur == first)
        erase_bucket(n, last);
    else {
        node* next;
        for(next = cur->next;next != first;cur = next, next = cur->next);
        while(next != last) {
            cur->next = next->next;
            delete_node(next);
            next = cur->next;
            --num_elements;
        }
    }
}

template <class Value, class Key, class HF, class Ex, class Eq, class Al>
void hashtable<Value,Key,HF,Ex,Eq,Al>
    ::erase_bucket(const size_type n, node* last) {
    node* cur = buckets[n];
    while(cur != last) {
        node* next = cur->next;
        delete_node(cur);
        cur = next;
        buckets[n] = cur;
        --num_elements;
    }
}

template <class Value, class Key, class HF, class Ex, class Eq, class Al>
void hashtable<Value,Key,HF,Ex,Eq,Al>::clear() {
    for(size_type i = 0;i < buckets.size();++i) {
        node* cur = buckets[i];
        while(cur != 0) {
            node* next = cur->next;
            delete_node(cur);
            cur = next;
        }
        buckets[i] = nullptr;
    }
    num_elements = 0;
}

    
template <class Value, class Key, class HF, class Ex, class Eq, class Al>
void hashtable<Value,Key,HF,Ex,Eq,Al>::copy_from(const hashtable& ht) {
    buckets.clear();
    buckets.reserve(ht.buckets.size());
    buckets.insert(buckets.end(), ht.buckets.size(), (node*) 0);
    try {
        for(size_type i = 0;i < ht.buckets.size();++i) {
            const node* cur = ht.buckets[i];
            if(cur) {
                node* c = new_node(cur->val);
                buckets[i] = c;

                for(node* next = cur->next;next;
                    cur = next, next = cur->next) {
                    c->next = new_node(next->val);
                    c = c->next;
                }
            }
        }
        num_elements = ht.num_elements;
    } catch(std::exception) {
        clear();
        throw;
    }
}

} // MiniSTL