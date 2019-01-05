#include <iostream>
#include <typeinfo>

/*  design:
 *      1. iterator_traits extract the value_type "V" of object pointed by iter
 *      2. type_traits extract some feature "F" of value_type "V"
 *      3. algorithms use "F" to improve efficiency for different iter which has different value_type
*/

// use for argument decucation
struct white {};
struct black {};

// traits type's color feature
template <class T>
struct type_traits {
    using color = white;
};

// partial specialization for bool, int, double
template <>
struct type_traits<bool> {
    using color = black;
};

template <>
struct type_traits<int> {
    using color = black;
};

template <>
struct type_traits<double> {
    using color = black;
};

// alias template
template <class T> 
using color = typename type_traits<T>::color;


// traits iterator's value_type
template <class Iterator>
struct iterator_traits {
    using value_type = typename Iterator::value_type;
};

// alias template 
template <class Iterator>
using value_type = typename iterator_traits<Iterator>::value_type;



// color<value_type<ForwardIterator> >()
// = type_traits< iterator_traits<ForwardIter>::value_type >::color()
template <class ForwardIterator>
inline void dosth(ForwardIterator f) {
    __dosth_aux(f, color<value_type<ForwardIterator> >());
}

template <class ForwardIterator>
inline void __dosth_aux(ForwardIterator f, white) {
    std::cout << "do white thing" << std::endl;
}

template <class ForwardIterator>
inline void __dosth_aux(ForwardIterator f, black) {
    std::cout << "do black thing" << std::endl;
}

template <class T>
struct MyIter {
    using value_type = T;
    T t;
    MyIter(T& tmp) : t(tmp) {
        std::cout << typeid(t).name() << " " << t << std::endl;
    };
};

int main() {
    int a = 10;
    long b = 20;
    MyIter<int> it1(a);
    MyIter<long> it2(b);
    dosth(it1);
    dosth(it2);

    return 0;
}

