#pragma once

#include <cstddef>
#include <iostream>

namespace MiniSTL {
    
// back inserter, container must support func push_back
template <class Container>
class back_insert_iterator {
protected:
    Container* container;

public:
    using container_type = Container;
    using iterator_category = output_iterator_tag;
    using value_type = void;
    using difference_type = void;
    using pointer = void;
    using reference = void;

    explicit back_insert_iterator(Container& c) : container(c) {}

    // assign a value v to back_insert_iterator
    // = push_back v to container
    back_insert_iterator& operator=(const typename Container::value_type& v) {
        container->push_back(v);
        return *this;
    }

    // semantically, do nothing
    back_insert_iterator& operator*() { return *this; }
    back_insert_iterator& operator++() { return *this; }
    back_insert_iterator& operator++(int) { return *this; }
};

// helper function to quickly build a inserter
template <class Container>
inline back_insert_iterator<Container> back_inserter(Container& c) {
    return back_insert_iterator<Container>(c);
}


// front inserter, container must support func push_front
template <class Container>
class front_insert_iterator {
protected:
    Container* container;

public:
    using container_type = Container;
    using iterator_category = output_iterator_tag;
    using value_type = void;
    using difference_type = void;
    using pointer = void;
    using reference = void;

    explicit front_insert_iterator(Container& c) : container(c) {}

    // assign a value v to front_insert_iterator
    // = push_front v to container
    front_insert_iterator& operator=(const typename Container::value_type& v) {
        container->push_front(v);
        return *this;
    }

    front_insert_iterator& operator*() { return *this; }
    front_insert_iterator& operator++() { return *this; }
    front_insert_iterator& operator++(int) { return *this; }
};

// helper function to quickly build a inserter
template <class Container>
inline front_insert_iterator<Container> front_inserter(Container& c) {
    return front_insert_iterator<Container>(c);
}


// inserter, container must support func insert(iter, v)
template <class Container>
class insert_iterator {
protected:
    Container* container;
    using iterator_type = typename Container::iteartor;
    iterator_type iter;

public:
    using container_type = Container;
    using iterator_category = output_iterator_tag;
    using value_type = void;
    using difference_type = void;
    using pointer = void;
    using reference = void;

    insert_iterator(Container& c, iterator_type i) 
        : container(c), iter(i) {}

    // assign a value v to insert_iterator
    // = insert v to pos pointerd by iter in container
    // then advance iter
    insert_iterator& operator=(const typename Container::value_type& v) {
        container->insert(iter, v);
        ++iter;
        return *this;
    }

    insert_iterator& operator*() { return *this; }
    insert_iterator& operator++() { return *this; }
    insert_iterator& operator++(int) { return *this; }
};

template <class Container, class Iterator>
inline insert_iterator<Container>
inserter(Container& c, Iterator i) {
    return insert_iterator<Container>(c,
                        typename Container::iterator(i));
}


// reverse iterator: trun a iterator to reverse edition
template <class Iterator>
class __reverse_iterator {
protected:
    Iterator cur;

public:
    using iterator_category = iterator_category_t<Iterator>;
    using value_type = value_type_t<Iterator>;
    using difference_type = difference_type_t<Iterator>;
    using pointer = pointer_t<Iterator>;
    using reference = reference_t<Iterator>;

    using iterator_type = Iterator;

public:
    __reverse_iterator() {}
    explicit __reverse_iterator(iterator_type i) : cur(i) {}
    __reverse_iterator(const __reverse_iterator& i) :  cur(i.cur) {}

    template <class Iter>
    __reverse_iterator(const __reverse_iterator<Iter>& i) : cur(i.cur) {} 

    iterator_type base() const { return cur; }

    // physically, iter and its reverse_iter point to same postion
    // logically, reverse_iter points to the prev object that its iter points
    reference operator*() const {
        iterator_type tmp = cur;
        return *--tmp;
    }

    pointer operator->() const {
        return &(operator*());
    }

    __reverse_iterator& operator++() {
        --cur;
        return *this;
    }

    __reverse_iterator operator++(int) {
        __reverse_iterator tmp = *this;
        --cur;
        return tmp;
    }

    __reverse_iterator& operator--() {
        ++cur;
        return *this;
    }

    __reverse_iterator operator--(int) {
        __reverse_iterator tmp = *this;
        ++cur;
        return tmp;
    }

    __reverse_iterator operator+(difference_type n) const {
        return __reverse_iterator(cur - n);
    }

    __reverse_iterator& operator+=(difference_type n) {
        cur -= n;
        return *this;
    }

    __reverse_iterator operator-(difference_type n) const {
        return __reverse_iterator(cur + n);
    }

    __reverse_iterator& operator-=(difference_type n) {
        cur -= n;
        return *this;
    }

    reference operator[](difference_type n) const {
        return *(*this + n);
    }
};

template <class Iter>
inline bool operator==(const __reverse_iterator<Iter>& x,
                       const __reverse_iterator<Iter>& y) {
    return x.base() == y.base();
}

template <class Iter>
inline bool operator<(const __reverse_iterator<Iter>& x,
                       const __reverse_iterator<Iter>& y) {
    return x.base() < y.base();
}

template <class Iter>
inline bool operator!=(const __reverse_iterator<Iter>& x,
                       const __reverse_iterator<Iter>& y) {
    return !(x == y);
}

template <class Iter>
inline bool operator>(const __reverse_iterator<Iter>& x,
                       const __reverse_iterator<Iter>& y) {
    return y < x;
}

template <class Iter>
inline bool operator<=(const __reverse_iterator<Iter>& x,
                       const __reverse_iterator<Iter>& y) {
    return !(y < x);
}

template <class Iter>
inline bool operator>=(const __reverse_iterator<Iter>& x,
                       const __reverse_iterator<Iter>& y) {
    return !(x < y);
}

// return distance between two iterator adaptors
template <class Iter>
inline difference_type_t<Iter>
operator-(const __reverse_iterator<Iter>& x,
          const __reverse_iterator<Iter>& y) {
    return y.base() - x.base();
}

// Returns the iterator it incremented by n.
template <class Iter>
inline __reverse_iterator<Iter>
operator+(difference_type_t<Iter> n,
          const __reverse_iterator<Iter>& x) {
    return x + n;
}



// input stream iterator declare
template <class T, class Distance>
class istream_iterator;

template <class T, class Distance>
inline bool operator==(const istream_iterator<T, Distance>&,
                        const istream_iterator<T, Distance>&);

template <class T, class Distance = ptrdiff_t>
class istream_iterator {
    
    // friend func declare use different type argument: each instance
    // of func is friend of according instance of iter
    // template <class T1, class Distance1>
    // inline bool operator==(const istream_iterator<T1, Distance1>&,
    //                        const istream_iterator<T1, Distance1>&);

protected:
    std::istream* istream;
    T value;
    bool end_marker;

    void read() {
        end_marker = (*istream) ? true : false;
        if(end_marker)
            istream >> value;
        end_marker = (*istream) ? true : false;
    }

public:
    using iterator_category = input_iterator_tag;
    using value_type = T;
    using difference_type = Distance;
    using pointer = const T*;
    using reference = const T&;

    // cin as default input stream
    istream_iterator() : istream(std::cin), end_marker(false) {}
    istream_iterator(std::istream& s) : istream(s) { read(); }
    
    reference operator*() const { return value; }
    pointer operator->() const { return &(operator*()); }

    // semantic: iter++, read a value from istream
    istream_iterator& operator++() {
        read();
        return *this;
    }

    istream_iterator operator++(int) {
        istream_iterator tmp = *this;
        read();
        return tmp;
    }
};

// ? why equal the following condition?
template <class T, class Distance>
inline bool operator==(const istream_iterator<T, Distance>& x,
                        const istream_iterator<T, Distance>& y) {
    return (x.istream == y.istream && x.end_marker == y.end_marker) || 
            (x.end_marker == false && y.end_marker == false);
}

template <class T, class Distance>
inline bool operator!=(const istream_iterator<T, Distance>& x,
                        const istream_iterator<T, Distance>& y) {
    return !(x == y);
}


template <class T>
class ostream_iterator {
protected:
    std::ostream* ostream;
    const char* deli; // delimeter

public:
    using iterator_category = output_iterator_tag;
    using value_type = void;
    using difference_type = void;
    using pointer = void;
    using reference = void;

    // cout as default output stream
    ostream_iterator() : ostream(std::cout), deli(' ') {}
    ostream_iterator(std::ostream& s) : ostream(s), deli(' ') {}
    ostream_iterator(std::ostream& s, const char* c)
        : ostream(s), deli(c) {}
    
    // semantic: iter=v, output v to ostream
    ostream_iterator& operator=(const T& v) {
        *ostream << v;
        if(deli) *ostream << deli;
        return *this;
    }

    ostream_iterator& operator*() { return *this; }
    ostream_iterator& operator++() { return *this; }
    ostream_iterator& operator++(int) { return *this; }
};

} // MiniSTL
