#pragma once

/*
This header file provides a framework for allowing compile time
dispatch based on type attributes. This is useful when writing
template code.
For example, when making a copy of an array of an unknown type,
it helps to know if the type has a trivial copy constructor or not,
to help decide if a memcopy can be used.

The class template type_traits provides a series of typedefs each
of which is either of true_type or false_type. The argument to 
type_traits can be any type. The typedef within this template will 
attain their correct values by one of these means:
    1. The general instantiation contain conservative values which
        work for all types. 
    2. Specializations may be declared to make distinctions
        between types.
    3. Some compilers will automatically provide the approviate
        specializations for all types.
*/

namespace MiniSTL {


// not like bool value(true/false), the following two types can be 
// used in type deduction then function overloading, which is in compile-time.
struct true_type {};
struct false_type {};

// ? how to diminish a type is true_type or not?
// ? generally, type which has dynamic resources is false_type
// ? what is pod_type?  see: http://www.cplusplus.com/reference/type_traits/is_pod/
template <class T>
struct type_traits {
    using has_trivial_default_constructor = false_type;
    using has_trivial_copy_constructor = false_type;
    using has_trivial_assignment_operator = false_type;
    using has_trivial_destructor = false_type;
    using is_POD_type = false_type;
};

//alias template
template <class T>
using has_trivial_default_constructor_t = typename type_traits<T>::has_trivial_copy_constructor;

template <class T> 
using has_trivial_copy_constructor_t = typename type_traits<T>::has_trivial_copy_constructor;

template <class T>
using has_trivial_assignment_operator_t = typename type_traits<T>::has_trivial_assignment_operator;

template <class T>
using has_trivial_destructor_t = typename type_traits<T>::has_trivial_destructor;

template <class T>
using is_POD_type_t = typename type_traits<T>::is_POD_type;



template <>
struct type_traits<bool> {
    using has_trivial_default_constructor = true_type;
    using has_trivial_copy_constructor = true_type;
    using has_trivial_assignment_operator = true_type;
    using has_trivial_destructor = true_type;
    using is_POD_type = true_type;
};

template <>
struct type_traits<char> {
    using has_trivial_default_constructor = true_type;
    using has_trivial_copy_constructor = true_type;
    using has_trivial_assignment_operator = true_type;
    using has_trivial_destructor = true_type;
    using is_POD_type = true_type;
};

template <>
struct type_traits<signed char> {
    using has_trivial_default_constructor = true_type;
    using has_trivial_copy_constructor = true_type;
    using has_trivial_assignment_operator = true_type;
    using has_trivial_destructor = true_type;
    using is_POD_type = true_type;
};

template <>
struct type_traits<unsigned char> {
    using has_trivial_default_constructor = true_type;
    using has_trivial_copy_constructor = true_type;
    using has_trivial_assignment_operator = true_type;
    using has_trivial_destructor = true_type;
    using is_POD_type = true_type;
};

template <>
struct type_traits<wchar_t> {
    using has_trivial_default_constructor = true_type;
    using has_trivial_copy_constructor = true_type;
    using has_trivial_assignment_operator = true_type;
    using has_trivial_destructor = true_type;
    using is_POD_type = true_type;
};

template <>
struct type_traits<short> {
    using has_trivial_default_constructor = true_type;
    using has_trivial_copy_constructor = true_type;
    using has_trivial_assignment_operator = true_type;
    using has_trivial_destructor = true_type;
    using is_POD_type = true_type;
};

template <>
struct type_traits<unsigned short> {
    using has_trivial_default_constructor = true_type;
    using has_trivial_copy_constructor = true_type;
    using has_trivial_assignment_operator = true_type;
    using has_trivial_destructor = true_type;
    using is_POD_type = true_type;
};

template <>
struct type_traits<int> {
    using has_trivial_default_constructor = true_type;
    using has_trivial_copy_constructor = true_type;
    using has_trivial_assignment_operator = true_type;
    using has_trivial_destructor = true_type;
    using is_POD_type = true_type;
};

template <>
struct type_traits<unsigned int> {
    using has_trivial_default_constructor = true_type;
    using has_trivial_copy_constructor = true_type;
    using has_trivial_assignment_operator = true_type;
    using has_trivial_destructor = true_type;
    using is_POD_type = true_type;
};

template <>
struct type_traits<long> {
    using has_trivial_default_constructor = true_type;
    using has_trivial_copy_constructor = true_type;
    using has_trivial_assignment_operator = true_type;
    using has_trivial_destructor = true_type;
    using is_POD_type = true_type;
};

template <>
struct type_traits<unsigned long> {
    using has_trivial_default_constructor = true_type;
    using has_trivial_copy_constructor = true_type;
    using has_trivial_assignment_operator = true_type;
    using has_trivial_destructor = true_type;
    using is_POD_type = true_type;
};

template <>
struct type_traits<long long> {
    using has_trivial_default_constructor = true_type;
    using has_trivial_copy_constructor = true_type;
    using has_trivial_assignment_operator = true_type;
    using has_trivial_destructor = true_type;
    using is_POD_type = true_type;
};

template <>
struct type_traits<unsigned long long> {
    using has_trivial_default_constructor = true_type;
    using has_trivial_copy_constructor = true_type;
    using has_trivial_assignment_operator = true_type;
    using has_trivial_destructor = true_type;
    using is_POD_type = true_type;
};

template <>
struct type_traits<float> {
    using has_trivial_default_constructor = true_type;
    using has_trivial_copy_constructor = true_type;
    using has_trivial_assignment_operator = true_type;
    using has_trivial_destructor = true_type;
    using is_POD_type = true_type;
};

template <>
struct type_traits<double> {
    using has_trivial_default_constructor = true_type;
    using has_trivial_copy_constructor = true_type;
    using has_trivial_assignment_operator = true_type;
    using has_trivial_destructor = true_type;
    using is_POD_type = true_type;
};

template <>
struct type_traits<long double> {
    using has_trivial_default_constructor = true_type;
    using has_trivial_copy_constructor = true_type;
    using has_trivial_assignment_operator = true_type;
    using has_trivial_destructor = true_type;
    using is_POD_type = true_type;
};

// partial specialization
// native pointer also is a scalar type
template <class T>
struct type_traits<T*> {
    using has_trivial_default_constructor = true_type;
    using has_trivial_copy_constructor = true_type;
    using has_trivial_assignment_operator = true_type;
    using has_trivial_destructor = true_type;
    using is_POD_type = true_type;
};


// ? why don't need a partial specialization for const T*
// ? I think type_traits will be used to mutate the value, so
// template <class T>
// struct type_traits<const T*> {
//     using has_trivial_default_constructor = true_type;
//     using has_trivial_copy_constructor = true_type;
//     using has_trivial_assignment_operator = true_type;
//     using has_trivial_destructor = true_type;
//     using is_POD_type = true_type;
// };

template <class T>
struct is_integer {
    using integral = false_type;
};

template <>
struct is_integer<bool> {
    using integral = true_type;
};

template <>
struct is_integer<char> {
    using integral = true_type;
};

template <>
struct is_integer<signed char> {
    using integral = true_type;
};

template <>
struct is_integer<unsigned char> {
    using integral = true_type;
};

template <>
struct is_integer<wchar_t> {
    using integral = true_type;
};

template <>
struct is_integer<short> {
    using integral = true_type;
};

template <>
struct is_integer<unsigned short> {
    using integral = true_type;
};

template <>
struct is_integer<int> {
    using integral = true_type;
};

template <>
struct is_integer<unsigned int> {
    using integral = true_type;
};

template <>
struct is_integer<long> {
    using integral = true_type;
};

template <>
struct is_integer<unsigned long> {
    using integral = true_type;
};

template <>
struct is_integer<long long> {
    using integral = true_type;
};

template <>
struct is_integer<unsigned long long> {
    using integral = true_type;
};


// alias template
template <class T>
using integral = typename is_integer<T>::integral;

} // MiniSTL
