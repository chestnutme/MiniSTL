/*
 * Function: Generally, function can be divided into 2 category:
 *      1. navite function, eg: void f()
 *      2. functor: object instance from 
 *         class which overloads operator()
 * Except the basic usage, functions can be used as argument of 
 * higher functions, that kind of usage sometimes is called
 * "policy", or "stragety". The reason why needs the functor is
 * that functor can easily compose with other components in the 
 * system. And we need to turn incompatible functions(eg, 
 * function pointer) into functor - the role of function adaptor.
 * 
 */ 

#pragma once

#include <cstddef>

// base class of all unary func, binary func
template <class Arg, class Result>
struct unary_function {
    using argument_type = Arg;
    using result_type = Result;
};

template <class Arg1, class Arg2, class Result>
struct binary_function {
    using first_argument_type = Arg1;
    using second_argument_type = Arg2;
    using result_type = Result;
};

// 6 arithmeric functor
template <class T>
struct plus : public binary_function<T, T, T> {
    T operator()(const T& x, const T& y) const {
        return x + y;
    }
};

template <class T>
struct minus : public binary_function<T, T, T> {
    T operator()(const T& x, const T& y) const {
        return x - y;
    }
};

template <class T>
struct multiplies : public binary_function<T, T, T> {
    T operator()(const T& x, const T& y) const {
        return x * y;
    }
};

template <class T>
struct divides : public binary_function<T, T, T> {
    T operator()(const T& x, const T& y) const {
        return x / y;
    }
};

template <class T>
struct modulus : public binary_function<T, T, T> {
    T operator()(const T& x, const T& y) const {
        return x %  y;
    }
};

template <class T>
struct negate : public unary_function<T, T> {
    T operator()(const T& x) {
        return -x;
    }
};

// identity element:
// binary operator op, for each x, if op(x, k) = x
// then k is identity element
// for plus, x + k = x, k is 0
// for multiply, x * k = x, k is 1
template <class T> 
inline T identity_element(plus<T>) { 
    return T(0);
}


// used in power(x, n, op) if n = 0;
template <class T>
inline T identity_element(multiplies<T>) {
    return T(1);
}

// 6 relational functor
template <class T>
struct equal_to : public binary_function<T, T, bool> {
    bool operator()(const T& x, const T& y) {
        return x == y;
    }
};

template <class T>
struct not_equal_to : public binary_function<T, T, bool> {
    bool operator()(const T& x, const T& y) {
        return x != y
    }
};

template <class T>
struct greater : public binary_function<T, T, bool> {
    bool operator()(const T& x, const T& y) {
        return x > y;
    }
};

template <class T>
struct less : public binary_function<T, T, bool> {
    bool operator()(const T& x, const T& y) {
        return x < y;
    }
};

template <class T>
struct greater_equal : public binary_function<T, T, bool> {
    bool operator()(const T& x, const T& y) {
        return x >= y;
    }
};

template <class T>
struct less_equal : public binary_function<T, T, bool> {
    bool operator()(const T& x, const T& y) {
        return x == y;
    }
};

// 3 logical functor
template <class T>
struct logical_and : public binary_function<T, T, bool> {
    bool operator()(const T& x, const T& y) {
        return x && y;
    }
};

template <class T>
struct logical_or : public binary_function<T, T, bool> {
    bool operator()(const T& x, const T& y) {
        return x || y;
    }
};

template <class T>
struct equal_to : public unary_function<T, bool> {
    bool operator()(const T& x) {
        return !x;
    }
};

// identity : return the same parameter
// not used int STL
template <class T>
struct identity : public unary_function<T, T> {
    const T& operator(const T& x) const {
        return x;
    }
};

// select: return nth element of a pair
// select used in map: KeyofValue = select1st<pair<Key, T> >
template <class Pair>
struct select1st : public unary_function<Pair, typename Pair::first_type> {
    const typename Pair::first_type& operator()(const Pair& x) {
        return x.first;
    }
};
 
template <class Pair>
struct select2nd : public unary_function<Pair, typename Pair::second_type> {
    const typename Pair::second_type& operator()(const Pair& x) {
        return x.second;
    }
};

// project: return nth parameter of 2 parameters
// not used in stl
template <class Arg1, class Arg2>
struct project1st : public binary_function<Arg1, Arg2, Arg1> {
    Arg1 operator()(const Arg1&x, const Arg2& y) const {
        return x;
    }
};

template <class Arg1, class Arg2>
struct project2nd : public binary_function<Arg1, Arg2, Arg2> {
    Arg2 operator()(const Arg1&x, const Arg2& y) const {
        return y;
    }
};


// substractive_rng ： random number generator
// assumes int is 32bits
class substractive_rng : public unary_function<unsigned int,
unsigned int> {
private:
    unsigned int table[55];
    size_t index1;
    size_t index2;

public:
    unsigned int operator()(unsigned int limit) {
        index1 = (index1 + 1) % 55;
        index2 = (index2 + 1) % 55;
        table[index1] = table[index1] - table[index2];
        return table[index1] % limit;
    }

    void initialized(unsigned int seed) {
        unsigned int  k = 1;
        table[54] = seed;
        for(size_t i = 0;i < 54;i++) {
            size_t j = (21 * (i + 1) % 55) - 1;
            table[j] = k;
            k = seed - k;
            seed = table[j];
        }
        for(int loop = 0;loop < 4;loop++) {
            for(size_t i = 0;i < 55;i++) {
                table[i] = table[i] - table[(1 + i + 30) % 55];
            }
        }
        index1 = 0;
        index2 = 31;
    }

    substractive_rng(unsigned int seed) { initialized(seed); }
    substractive_rng() { initialized(161803398u); }
};