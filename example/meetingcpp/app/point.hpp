// Copyright 2012 Pyry Jahkola.
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef APP_POINT_HPP
#define APP_POINT_HPP

#include "graphics/object.hpp"

namespace app {

    class point {
        double x, y;
    public:
        point(double x, double y) : x(x), y(y) {}

        // Implementation for graphics::object:

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

} // app

#endif // APP_POINT_HPP
