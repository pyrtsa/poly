// Copyright 2012 Pyry Jahkola.
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef ADOPT_BOX_AS_GRAPHICS_HPP
#define ADOPT_BOX_AS_GRAPHICS_HPP

#include "their/box.hpp"
#include "graphics/graphics.hpp"

namespace graphics { // <- their::box

    size_type call(size_fn, their::box const & b) {
        return { b.right - b.left, b.bottom - b.top };
    }

    point_type call(center_fn, their::box const & b) {
        return { 0.5 * (b.left + b.right), 0.5 * (b.top + b.bottom) };
    }
    
    void call(size_fn, their::box & b, size_type value) {
        point_type c = graphics::center(b);
        b.left   = c[0] - 0.5 * value[0];
        b.top    = c[1] - 0.5 * value[1];
        b.right  = c[0] + 0.5 * value[0];
        b.bottom = c[1] + 0.5 * value[1];
    }

    void call(center_fn, their::box & b, point_type value) {
        point_type c = graphics::center(b);
        size_type offset = { value[0] - c[0], value[1] - c[1] };
        b.left   += offset[0];
        b.top    += offset[1];
        b.right  += offset[0];
        b.bottom += offset[1];
    }

    void call(draw_fn, context & g, their::box const & b) {
        g << b.to_json();
    }

} // graphics <- their::box

#endif // ADOPT_BOX_AS_GRAPHICS_HPP
