// Copyright 2012 Pyry Jahkola.
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "circle_as_graphics.hpp"
#include "vector_as_graphics.hpp"
#include "box_as_graphics.hpp"

#include <iostream>

template <typename T>
void show(std::string const & name, T const & t) {
    graphics::draw_fn{}(std::cout << name << ": ", t);
    std::cout << '\n' << std::endl;
}

#include <poly/interface.hpp>

namespace graphics {

    using drawable = poly::interface<
        void(draw_fn, context &, poly::self const &)>;

    struct object : poly::interface<object,
          void(draw_fn, context &, poly::self const &)
        , size_type(size_fn, poly::self const &)
        , point_type(center_fn, poly::self const &)
        , void(size_fn, poly::self &, size_type)
        , void(center_fn, poly::self &, point_type)
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
    return std::abs(ca[0] - cb[0]) < 0.5 * (sa[0] + sb[0])
        || std::abs(ca[1] - cb[1]) < 0.5 * (sa[1] + sb[1]);
}

using group_type = std::vector<graphics::drawable>;

namespace mine {

    class point {
        double x, y;
    public:
        point(double x, double y) : x(x), y(y) {}
        friend graphics::point_type call(graphics::center_fn, point const & p) {
            return {p.x, p.y};
        }
        friend void call(graphics::center_fn, point & p,
                                              graphics::point_type value)
        {
            p.x = value[0];
            p.y = value[1];
        }
    };

    graphics::size_type call(graphics::size_fn, point const &) { return {0,0}; }

    void call(graphics::size_fn, point &, graphics::size_type) {} // no-op

    void call(graphics::draw_fn, graphics::context & g, point const & p) {
        auto c = graphics::center(p);
        g << "{ \"x\": " << c[0] << ", \"y\": " << c[1] << " }";
    }

} // mine

int main() {
    mine::point a = {5, 5};
    ::show("a", a);
    
    theirs::box b = { 1, 2, 10, 20 };
    ::show("b", b);
    
    app::circle c = { 10, 20, 5 };
    ::show("c", c);
    
    group_type group = { a, b };
    ::show("group", group);
    
    group.push_back(group);
    ::show("group", group);
    
    group.push_back(c);
    ::show("group", group);
    
    std::cout << std::boolalpha;
    std::cout << "a & b: " << may_overlap(a, b) << std::endl;
    std::cout << "a & c: " << may_overlap(a, c) << std::endl;
    std::cout << "b & c: " << may_overlap(b, c) << std::endl;
    std::cout << std::endl;
    
    std::vector<graphics::object> objects = { a, b, c };
    
    ::show("objects", objects);
    for (auto & o : objects) graphics::center(o, graphics::point_type{{0, 0}});
    ::show("objects", objects);
    
    std::cout << "0 & 1: " << may_overlap(objects[0], objects[1]) << std::endl;
    std::cout << "0 & 2: " << may_overlap(objects[0], objects[2]) << std::endl;
    std::cout << "1 & 2: " << may_overlap(objects[1], objects[2]) << std::endl;
    
    std::cout << "\nDone." << std::endl;
}
