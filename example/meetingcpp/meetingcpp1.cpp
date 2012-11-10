// Copyright 2012 Pyry Jahkola.
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "circle_as_graphics.hpp"
#include "vector_as_graphics.hpp"
#include "box_as_graphics.hpp"

#include <iostream>

template <typename T>
void show(T const & t) {
    graphics::draw_fn{}(std::cout << '\n', t, 0);
    std::cout << '\n' << std::endl;
}

#include <poly/interface.hpp>

namespace graphics {

    using drawable = poly::interface<
        void(draw_fn, std::ostream &, poly::self const &, std::size_t)>;

    struct object : poly::interface<object,
          void(draw_fn, std::ostream &, poly::self const &, std::size_t)
        , size_type(size_fn, poly::self const &)
        , point_type(center_fn, poly::self const &)
        // , void(size_fn, poly::self &, size_type)
        // , void(center_fn, poly::self &, point_type)
        >
    {
        POLY_INTERFACE_CONSTRUCTORS(object);
    };

} // graphics

#include <cmath>

bool may_overlap(graphics::object const & a,
                 graphics::object const & b)
{
    using graphics::size;
    using graphics::center;
    auto ca = center(a);
    auto cb = center(b);
    auto sa = size(a);
    auto sb = size(b);
    return std::abs(ca[0] - cb[0]) > 0.5 * (sa[0] + sb[0])
        || std::abs(ca[1] - cb[1]) > 0.5 * (sa[1] + sb[1]);
}

using scene = std::vector<graphics::drawable>;

namespace mine {

    class point {
        double x, y;
    public:
        point(double x, double y) : x(x), y(y) {}
        friend graphics::point_type call(graphics::center_fn, point const & p) {
            return {p.x, p.y};
        }
    };

    graphics::size_type call(graphics::size_fn, point const &) { return {0,0}; }
    void call(graphics::draw_fn, std::ostream & out,
                                 point const & p,
                                 std::size_t indent)
    {
        auto c = graphics::center(p);
        out << std::string(indent, ' ') << '[' << c[0] << ", " << c[1] << ']';
    }

} // mine

int main() {
    mine::point a = {5, 5};
    ::show(a);
    
    theirs::box b = { 1, 2, 10, 20 };
    ::show(b);

    app::circle c = { 10, 20, 5 };
    ::show(c);

    scene s = { b, c };
    ::show(s);

    s.push_back(s);
    ::show(s);

    s.push_back(a);
    ::show(s);

    std::cout << "a & b: " << std::boolalpha << may_overlap(a, b) << std::endl;
    std::cout << "a & c: " << std::boolalpha << may_overlap(a, c) << std::endl;
    std::cout << "b & c: " << std::boolalpha << may_overlap(b, c) << std::endl;
}
