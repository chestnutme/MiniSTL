#pragma once

namespace Unused {

// auto_ptr(Deprecated in C++11) is a smart pointer that manages an object obtained 
// via new expression and deletes that object when auto_ptr 
// itself is destroyed. It may be used to provide exception safety 
// for dynamically-allocated objects, for passing ownership of
// dynamically-allocated objects into functions and for returning 
// dynamically-allocated objects from functions.

// Copying an auto_ptr copies the pointer and transfers ownership 
// to the destination: both copy construction and copy assignment 
// of auto_ptr modify their right hand arguments, and the "copy" 
// is not equal to the original. Because of these unusual copy 
// semantics, auto_ptr may not be placed in standard containers. 
// std::unique_ptr is preferred for this and other uses. (since C++11)


template <class T>
struct auto_ptr_ref {
    T* ptr;
    auto_ptr_ref(T* p) : ptr(p) {}
};

template <class T>
class auto_ptr {
private:
    T* ptr;

public:
    using element_type = T;

    explicit auto_ptr(T* p = nullptr) noexcept : ptr(p);

    // transfers ownership from another auto_ptr 
    auto_ptr(auto_ptr& a) noexcept {
        ptr(a.release());
    }

    // transfers ownership from another auto_ptr 
    template<T1> 
    auto_ptr(auto_ptr<T1>& a) noexcept : ptr(a.release()) {}

    ~auto_ptr() {
        delete ptr;
    }

    // transfers ownership from another auto_ptr
    auto_ptr& operator=(auto_ptr& a) noexcept {
        // not a.ptr == this->ptr
        if(&a != this) {
            delete ptr;
            ptr(a.release());
        }
        return *this;
    }

    T& operator*() noexcept {
        return *ptr;
    }

    T* operator->() noexcept {
        return ptr;
    }

    T* get() noexcept {
        return ptr;
    }

    void reset(T* p = nullptr) noexcept {
        if(p) {
            delete ptr;
            ptr(p);
        }
    }

    T* release() noexcept {
        T* tmp = ptr;
        ptr = nullptr;
        return tmp;
    }

    template <class T1>
    operator auto_ptr<T1> () noexcept {
        return auto_ptr<T1>(this->release());
    }

public:
    auto_ptr(auto_ptr_ref<T> ref) noexcept
        : ptr(ref.ptr) {}

    auto_ptr& operator=(auto_ptr_ref<T> ref) noexcept {
        if (ref.ptr != this->get()) {
            delete ptr;
            ptr = ref.ptr;
        }
        return *this;
    }

    template <class T1> 
    operator auto_ptr_ref<T1>() noexcept { 
        return auto_ptr_ref<T1>(this->release()); 
    }
};

} // Unused