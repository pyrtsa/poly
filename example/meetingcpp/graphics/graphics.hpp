// Copyright 2012 Pyry Jahkola.
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef GRAPHICS_GRAPHICS_HPP
#define GRAPHICS_GRAPHICS_HPP

#include <poly/callable.hpp>
#include <array>
#include <ostream>
#include <string>

namespace graphics {

using context = std::ostream;
using size_type = std::array<double, 2>;
using point_type = std::array<double, 2>;

/// Usage:
///     size(T const & self) -> size_type
///     size(T & self, size_type value) -> void
constexpr struct size_fn : poly::callable<size_fn> {} size = {};

/// Usage:
///     center(T const & self) -> point_type
///     center(T & self, point_type value) -> void
constexpr struct center_fn : poly::callable<center_fn> {} center {};

/// Usage:
///     draw(context & g, T const & self) -> void
constexpr struct draw_fn : poly::callable<draw_fn> {} draw = {};

} // graphics

#endif // GRAPHICS_GRAPHICS_HPP
