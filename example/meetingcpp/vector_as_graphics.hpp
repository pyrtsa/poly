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
                decltype(call(draw_fn{}, std::declval<std::ostream &>(),
                                         std::declval<Graphics const &>(),
                                         std::declval<std::size_t>()))
            >
    void call(draw_fn, std::ostream & out,
                       std::vector<Graphics> const & v,
                       std::size_t indent)
    {
        auto i = begin(v), e = end(v);
        auto const spaces = std::string(indent, ' ');

        out << spaces << "[";

        if (i != e) {
            draw_fn{}(out << '\n', *i++, indent + 4);
            while (i != e)
                draw_fn{}(out << ",\n", *i++, indent + 4);
            out << '\n' << spaces;
        }

        out << ']';
    }

} // graphics

#endif // VECTOR_AS_GRAPHICS_HPP
