// Copyright 2012 Pyry Jahkola.
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <memory>
#include <ostream>

using context = std::ostream;

//template <typename T> void draw(context & g, T const & x) { g << x; }

struct drawable {
    template <typename T> drawable(T const & x) : p(new model<T>(x)) {}
    drawable(drawable && r) = default;
    drawable(drawable const & r) : p(r.p->copy()) {}
    drawable & operator=(drawable r) { p = move(r.p); return *this; }
    friend void draw(context & g, drawable const & d) { d.p->do_draw(g); }

private:
    struct concept {
        virtual ~concept() = default;
        virtual concept * copy() const = 0;
        virtual void do_draw(context & g) const = 0;
    };

    template <typename T> struct model : concept {
        explicit model(T x) : x(std::move(x)) {}
        virtual concept * copy() const override { return new model(*this); }
        virtual void do_draw(context & g) const override { draw(g, x); }
        T x;
    };

    std::unique_ptr<concept> p;
};

struct point { double x, y; };

void draw(context & g, point const & p) {
    g << "(" << p.x << ", " << p.y << ")";
}

#include <iostream>
#include <vector>

template <typename T>
void draw(context & g, std::vector<T> const & ds) {
    g << "[";
    auto i = begin(ds), e = end(ds);
    if    (i != e) draw(g,         *i++);
    while (i != e) draw(g << ", ", *i++);
    g << "]";
}

int main() {
    drawable pt = point{1, 2};
    draw(std::cout, pt);
    std::cout << std::endl;
    std::vector<drawable> ds = { point{1, 2} };
    ds.push_back(ds);
    draw(std::cout, ds);
    std::cout << std::endl;
}
