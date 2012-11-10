// Copyright 2012 Pyry Jahkola.
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef GRAPHCIS_DRAWABLE_HPP
#define GRAPHCIS_DRAWABLE_HPP

#include <poly/interface.hpp>

namespace graphics {

    using drawable = poly::interface<
        void(draw_fn, context &, poly::self const &)>;

} // graphics

#endif // GRAPHCIS_DRAWABLE_HPP
