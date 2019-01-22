#pragma once

#include "deque.hpp"

namespace MiniSTL {

template <class T, class Container = deque<T> >
class queue {
public:
    using value_type = typename Container::value_type;
    using reference = typename Container::reference;
    using const_reference = typename Container::const_reference;
    using size_type = typename Container::size_type;
    using container_type = Container;

protected:
    Container c;

public:
    explicit queue(const Container& q) : c(q) {}
    explicit queue(Container&& q = Container()) c(std::move(q)) {}
    queue(const queue& q) : c(q.c) {}
    queue(queue&& q) : c(std::move(q.c)) {}
 
    bool empty() const {
        return c.empty();
    }
    size_type size() const {
        return c.size();
    }
    reference front() {
        return c.front();
    }
    const_reference front() const {
        return c.front();
    }
    reference back() {
        return c.back();
    }
    const_reference back() const {
        return c.back();
    }
    void push(const value_type& x) {
        c.push_back(x);
    }
    void push(value_type&& x) {
        c.push_back(std::move(x));
    }
    template <class... Args> void emplace(Args&&... args) {
        c.emplace_back(std::forward<Args>(args)...);
    }
    void pop() {
        c.pop_front();
    }
    void swap(queue& q) noexcept(noexcept(swap(c, q.c))) {
        using std::swap;
        swap(c, q.c);
    }
};

template <class T, class Container>
bool operator==(const queue<T, Container>& x,const queue<T, Container>& y) {
    return x.c == y.c;
}
template <class T, class Container>
bool operator!=(const queue<T, Container>& x,const queue<T, Container>& y) {
    return x.c != y.c;
}

template <class T, class Container>
bool operator< (const queue<T, Container>& x,const queue<T, Container>& y) {
    return x.c < y.c;
}
template <class T, class Container>
bool operator> (const queue<T, Container>& x,const queue<T, Container>& y) {
    return x.c > y.c;
}
template <class T, class Container>
bool operator>=(const queue<T, Container>& x,const queue<T, Container>& y) {
    return x.c >= y.c;
}
template <class T, class Container>
bool operator<=(const queue<T, Container>& x,const queue<T, Container>& y) {
    return x.c <= y.c;
}

template <class T, class Container>
void swap(queue<T, Container>& x, queue<T, Container>& y)
        noexcept(noexcept(x.swap(y))) {
    x.swap(y);
}

} // MiniSTL