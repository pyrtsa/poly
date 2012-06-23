// Copyright 2012 Pyry Jahkola.
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef DRAWABLE_HPP_YC42FMI
#define DRAWABLE_HPP_YC42FMI

#include <poly/interface.hpp>
#include <ostream>
#include <string>
#include <vector>

namespace example {

POLY_CALLABLE(draw);

using drawable = poly::interface<
    void(draw_, poly::self const &, std::ostream &, std::size_t)>;

template <typename T>
void call(draw_, T const &x, std::ostream& out, std::size_t position) {
    out << std::string(position, ' ') << x << std::endl;
}

template <typename T>
void call(draw_, std::vector<T> const& xs, std::ostream& o, std::size_t p) {
    o << std::string(p, ' ') << "<document>" << std::endl;
    for (auto& x : xs) example::draw(x, o, p + 2);
    o << std::string(p, ' ') << "</document>" << std::endl;
}

} // example

#endif // DRAWABLE_HPP_YC42FMI
