// Copyright 2012 Pyry Jahkola.
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CONTAINERS_HPP
#define CONTAINERS_HPP

namespace containers {

/// Usage:
///     size(T const & self) -> std::size_t
constexpr struct size_fn : poly::callable<size_fn> {} size {};

} // containers

#endif // CONTAINERS_HPP
