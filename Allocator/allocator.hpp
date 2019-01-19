#pragma once

#include "alloc.hpp"

/*
 * class relationship:
 *  1. allocator: cpp standard allocator, using sgi-alloc as underlying allocator
 *  2. simple_alloc: call alloc's func, turn sizeof(T) to bytes
 *  3. allocAdaptor: adaptor turns sgi-alloc to standard allocator
 *  4. alloc_traits: encapualute the difference among std/sgi/allocAdaptor 
 * 
 * 
 * 
 */

namespace MiniSTL {

#ifdef USE_MALLOC
using alloc_t = malloc_alloc;
#else
using alloc_t = default_alloc;
#endif

// This implements allocators as specified in the C++ standard.  
//
// Note that standard-conforming allocators use many language features
// that are not yet widely implemented.  In particular, they rely on
// member templates, partial specialization, partial ordering of function
// templates, the typename keyword, and the use of the template keyword
// to refer to a template member of a dependent type.

template <class T>
class allocator {
    using Alloc_t = alloc_t;          // The underlying allocator.
public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = size_t;
    using difference_type = ptrdiff_t;

    template <class U> struct rebind {
      typedef allocator<U> other;
    };

    allocator() noexcept {}
    allocator(const allocator&) noexcept {}
    template <class U> allocator(const allocator<U>&) noexcept {}
    ~allocator() noexcept {}

    pointer address(reference x) const { return &x; }
    const_pointer address(const_reference x) const { return &x; }

    // sz is permitted to be 0.  The C++ standard says nothing
    // default_alloc the return value is when sz == 0.
    T* allocate(size_type sz, const void* = 0) {
        return sz != 0 ? static_cast<T*>(Alloc_t::allocate(sz * sizeof(T))) : 0;
    }

    // p is not permitted to be a null pointer.
    void deallocate(pointer p, size_type sz) { 
        Alloc_t::deallocate(p, sz * sizeof(T));
    }

    size_type max_size() const noexcept { 
        return SIZE_MAX / sizeof(T);
    }

    void construct(pointer p, const T& v) { new(p) T(v); }
    void destroy(pointer p) { p->~T(); }
};

template<>
class allocator<void> {
public:
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using pointer = void*;
    using const_pointer = const void*;
    using value_type = void;

    template <class U> 
    struct rebind {
        typedef allocator<U> other;
    };
};


template <class T1, class T2>
inline bool operator==(const allocator<T1>&, const allocator<T2>&) 
{
  return true;
}

template <class T1, class T2>
inline bool operator!=(const allocator<T1>&, const allocator<T2>&)
{
  return false;
}


template<class T, class Alloc_t = alloc_t>
class simple_alloc {
public:
    static T* allocate(size_t sz) { 
        return 0 == sz ? 
                0 : (T*) Alloc_t::allocate(sz * sizeof(T));
    }

    static T* allocate(void) { 
        return (T*) Alloc_t::allocate(sizeof(T)); 
    }

    static void deallocate(T* p, size_t sz) { 
        if(0 != sz) {
            Alloc_t::deallocate(p, sz * sizeof(T)); 
        }
    }

    static void deallocate(T* p) { 
        Alloc_t::deallocate(p, sizeof(T));
    }
};


// Allocator adaptor to turn an SGI-style allocator (e.g. alloc, malloc_alloc)
// into a standard-conforming allocator.   Note that this adaptor does
// *not* assume that all objects of the underlying alloc class are
// identical, nor does it assume that all of the underlying alloc's
// member functions are static member functions.  Note, also, that 
// allocator<T, alloc> is essentially the same thing as allocator<T>.

template <class T, class Alloc_t>
struct allocAdaptor {
    Alloc_t underlying_alloc;

    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = size_t;
    using difference_type = ptrdiff_t;

    template <class U> struct rebind {
        typedef allocAdaptor<U, Alloc_t> other;
    };

    allocAdaptor() noexcept {}
    allocAdaptor(const allocAdaptor& a) noexcept
        : underlying_alloc(a.underlying_alloc) {}
    template <class U> 
    allocAdaptor(const allocAdaptor<U, Alloc_t>& a) noexcept
        : underlying_alloc(a.underlying_alloc) {}
    ~allocAdaptor() noexcept {}

    pointer address(reference x) const { return &x; }
    const_pointer address(const_reference x) const { return &x; }

    // sz is permitted to be 0.
    T* allocate(size_type sz, const void* = 0) {
        return sz != 0 
            ? static_cast<T*>(underlying_alloc.allocate(sz * sizeof(T))) 
            : 0;
    }

    // p is not permitted to be a null pointer.
    void deallocate(pointer p, size_type sz) {         
        underlying_alloc.deallocate(p, sz * sizeof(T)); 
    }

    size_type max_size() const noexcept {
        return SIZE_MAX / sizeof(T); 
    }

    void construct(pointer p, const T& val) { new(p) T(val); }
    void destroy(pointer p) { p->~T(); }
};


template <class Alloc_t>
class allocAdaptor<void, Alloc_t> {
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using pointer = void*;
    using const_pointer = const void*;
    using value_type = void;

    template <class U> 
    struct rebind {
        typedef allocAdaptor<U, Alloc_t> other;
    };
};

template <class T, class Alloc_t>
inline bool operator==(const allocAdaptor<T, Alloc_t>& a1,
                       const allocAdaptor<T, Alloc_t>& a2) {
    return a1.underlying_alloc == a2.underlying_alloc;
}

template <class T, class Alloc_t>
inline bool operator!=(const allocAdaptor<T, Alloc_t>& a1,
                       const allocAdaptor<T, Alloc_t>& a2) {
  return a1.underlying_alloc != a2.underlying_alloc;
}


// Another allocator adaptor: alloc_traits.  This serves two
// purposes.  First, make it possible to write containers that can use
// either SGI-style allocators or standard-conforming allocator.
// Second, provide a mechanism so that containers can query whether or
// not the allocator has distinct instances.  If not, the container
// can avoid wasting a word of memory to store an empty object.

// This adaptor uses partial specialization.  The general case of
// alloc_traits<T, Alloc_t> assumes that Alloc_t is a
// standard-conforming allocator, possibly with non-equal instances
// and non-static members.

// There are always two members: allocator_type, which is a standard-
// conforming allocator type for allocating objects of type T, and
// instanceless, a static const member of type bool.  If
// instanceless is true, this means that there is no difference
// between any two instances of type allocator_type.  Furthermore, if
// instanceless is true, then alloc_traits has one additional
// member: _Alloc_type.  This type encapsulates allocation and
// deallocation of objects of type T through a static interface; it
// has two member functions, whose signatures are
//    static T* allocate(size_t)
//    static void deallocate(T*, size_t)

// The fully general version.

template <class T, class Alloc_t>
struct alloc_traits
{
    static const bool instanceless = false;
    using allocator_type = typename Alloc_t::template rebind<T>::other;
};

template <class T, class Alloc_t>
const bool alloc_traits<T, Alloc_t>::instanceless;

// The version for the std allocator.
template <class T, class U>
struct alloc_traits<T, allocator<U>> {
    static const bool instanceless = true;
    using alloc_type = simple_alloc<T, alloc_t>;
    using allocator_type = allocator<T>;
};


// Versions for the predefined SGI-style allocators.
template <class T>
struct alloc_traits<T, malloc_alloc> {
    static const bool instanceless = true;
    using alloc_type = simple_alloc<T, malloc_alloc>;
    using allocator_type = allocAdaptor<T, malloc_alloc>;
};

template <class T>
struct alloc_traits<T, default_alloc> {
    static const bool instanceless = true;
    using alloc_type = simple_alloc<T, default_alloc>;
    using allocator_type = allocAdaptor<T, default_alloc>;
};


// Versions for the allocator adaptor used with the predefined
// SGI-style allocators.
template <class T, class U>
struct alloc_traits<T, allocAdaptor<U, malloc_alloc>> {
    static const bool instanceless = true;
    using alloc_type = simple_alloc<T, malloc_alloc>;
    using allocator_type = allocAdaptor<T, malloc_alloc>;
};

template <class T, class U>
struct alloc_traits<T, allocAdaptor<U, default_alloc>> {
    static const bool instanceless = true;
    using alloc_type = simple_alloc<T, default_alloc>;
    using allocator_type = allocAdaptor<T, default_alloc>;
};

} // MiniSTL