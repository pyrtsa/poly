// Copyright 2012 Pyry Jahkola.
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef ADOPT_CIRCLE_AS_GRAPHICS_HPP
#define ADOPT_CIRCLE_AS_GRAPHICS_HPP

#include "app/circle.hpp"
#include "graphics/graphics.hpp"

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

    void call(draw_fn, context & g, app::circle const & c) {
        g << "{ \"x\": " << c.x
          << ", \"y\": " << c.y
          << ", \"r\": " << c.r
          << " }";
    }

} // graphics <- app::circle

#endif // ADOPT_CIRCLE_AS_GRAPHICS_HPP
