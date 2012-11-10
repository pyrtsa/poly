// Copyright 2012 Pyry Jahkola.
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CIRCLE_AS_GRAPHICS_HPP
#define CIRCLE_AS_GRAPHICS_HPP

#include "circle.hpp"
#include "graphics.hpp"

namespace graphics { // <- app::circle

    size_type call(size_fn, app::circle const & c) {
        return { 2 * c.r, 2 * c.r };
    }

    void call(size_fn, app::circle & c,
                       size_type value)
    {
        c.r = 0.5 * std::min(value[0], value[1]);
    }

    point_type call(center_fn, app::circle const & c) {
        return { c.x, c.y };
    }

    void call(center_fn, app::circle & c,
                         point_type value)
    {
        c.x = value[0];
        c.y = value[1];
    }

    void call(draw_fn, std::ostream & out,
                       app::circle const & c,
                       std::size_t indent)
    {
        out << std::string(indent, ' ')
            << "[\"circle\", " << c.x << ", " << c.y << ", " << c.r << "]";
    }

} // graphics <- app::circle

#endif // CIRCLE_AS_GRAPHICS_HPP