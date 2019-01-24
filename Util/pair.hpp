#pragma once
#include <utility>

namespace MiniSTL {

template <class T1, class T2>
struct pair {
    using first_type = T1;
    using secone_type = T2;

    T1 first;
    T2 second;

    pair() : first(T1()), second(T2()) {}
    pair(const T1& t1, const T2& t2) : first(t1), second(t2) {}

    template<class U1, class U2>
    pair(const pair<U1, U2>& p) : first(p.first), second(p.second) {}

    pair(const pair& p) : first(p.first), second(p.second) {}
    pair(pair&& p) : first(std::move(p.first)), second(std::move(p.second)) {}

    pair& operator=(const pair& p) {
        if(&p != this) {
            first = p.first;
            second = p.second;
        }
        return *this;
    }

    pair& operator=(pair&& p) {
        if(&p != this) {
            first = std::move(p.first);
            second = std::move(p.second);
        }
        return *this;
    }

    void swap(pair<T1, T2>& p) noexcept {
        std::swap(this->first, p.first);
        std::swap(this->second, p.second);
    }
};

template <class T1, class T2>
inline bool operator==(const pair<T1, T2>& x, const pair<T1, T2>& y) {
    return x.first == y.first && x.second == y.second;
}

template <class T1, class T2>
inline bool operator<(const pair<T1, T2>& x, const pair<T1, T2>& y) {
    return x.first < y.first || 
        (!(y.first < x.first) && x.second < y.second);
}

template <class T1, class T2>
inline bool operator!=(const pair<T1, T2>& x, const pair<T1, T2>& y) {
    return !(x == y);
}

template <class T1, class T2>
inline bool operator>(const pair<T1, T2>& x, const pair<T1, T2>& y) {
    return y < x;
}

template <class T1, class T2>
inline bool operator<=(const pair<T1, T2>& x, const pair<T1, T2>& y) {
    return !(y < x);
}

template <class T1, class T2>
inline bool operator>=(const pair<T1, T2>& x, const pair<T1, T2>& y) {
    return !(x < y);
}

template <class T1, class T2>
inline pair<T1, T2> make_pair(const T1& t1, const T2& t2) {
    return pair<T1, T2>(t1, t2);
}

template <class T1, class T2>
void swap(pair<T1, T2>& x, pair<T1, T2>& y) noexcept {
    x.swap(y);
}

} // MiniSTL