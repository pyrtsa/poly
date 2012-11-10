// Copyright 2012 Pyry Jahkola.
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOX_AS_GRAPHICS_HPP
#define BOX_AS_GRAPHICS_HPP

#include "box.hpp"
#include "graphics.hpp"

namespace graphics { // <- theirs::box

    size_type call(size_fn, theirs::box const & b) {
        return { b.right - b.left, b.bottom - b.top };
    }

    point_type call(center_fn, theirs::box const & b) {
        return { 0.5 * (b.left + b.right), 0.5 * (b.top + b.bottom) };
    }

    void call(draw_fn, std::ostream & out,
                       theirs::box const & b,
                       std::size_t indent)
    {
        out << std::string(indent, ' ') << b.to_json();
    }

} // graphics <- theirs::box

#endif // BOX_AS_GRAPHICS_HPP
