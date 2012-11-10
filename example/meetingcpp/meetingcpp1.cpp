// Copyright 2012 Pyry Jahkola.
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "adopt/box_as_graphics.hpp"
#include "adopt/circle_as_graphics.hpp"
#include "adopt/vector_as_graphics.hpp"

#include "app/point.hpp"

#include "graphics/drawable.hpp"
#include "graphics/object.hpp"

// -----------------------------------------------------------------------------

#include <iostream>

/// Usage:
///     void show(std::string const & name, T const & x)
constexpr struct show_fn : poly::callable<show_fn> {} show = {};

template <typename T>
void call(show_fn, std::string const & name, T const & x) {
    graphics::draw_fn{}(std::cout << name << ": ", x);
    std::cout << '\n' << std::endl;
}

// -----------------------------------------------------------------------------

#include <cmath>

bool may_overlap(graphics::object const & a,
                 graphics::object const & b)
{
    using graphics::size;
    using graphics::center;
    auto ca = center(a), cb = center(b);
    auto sa = size(a), sb = size(b);
    return std::abs(ca[0] - cb[0]) < 0.5 * (sa[0] + sb[0])
        || std::abs(ca[1] - cb[1]) < 0.5 * (sa[1] + sb[1]);
}

// -----------------------------------------------------------------------------

using group_type = std::vector<graphics::drawable>;

int main() {
    app::point a = {5, 5};
    their::box b = { 1, 2, 10, 20 };
    app::circle c = { 10, 20, 5 };

    ::show("a", a);
    ::show("b", b);
    ::show("c", c);

    group_type group;
    group.push_back(a);
    group.push_back(b);
    group.push_back(group);

    ::show("group", group);
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
