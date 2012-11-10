// Copyright 2012 Pyry Jahkola.
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef VECTOR_AS_GRAPHICS_HPP
#define VECTOR_AS_GRAPHICS_HPP

#include <vector>
#include "graphics.hpp"

namespace graphics {

    template< typename Graphics
            , typename Enable =
                decltype(call(draw_fn{}, std::declval<context &>(),
                                         std::declval<Graphics const &>()))
            >
    void call(draw_fn, context & g, std::vector<Graphics> const & v) {
        auto i = begin(v), e = end(v);

        g << "[";

        if    (i != e) draw_fn{}(g,         *i++);
        while (i != e) draw_fn{}(g << ",   ", *i++);

        g << ']';
    }

} // graphics

#endif // VECTOR_AS_GRAPHICS_HPP
