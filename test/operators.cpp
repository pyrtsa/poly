// Copyright 2012 Pyry Jahkola.
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <poly/operators.hpp>
#include <poly/interface.hpp>
#include <iostream>

// POLY_CALLABLE(add);

struct addable : poly::interface<addable
    , addable(poly::operator_add_, poly::self const &, addable const &)
    // , addable(add_, poly::self const &, addable const &)
    >
{ POLY_INTERFACE_CONSTRUCTORS(addable); };

// template <typename T>
// addable call(add_, T const & a, addable const & b) {
//     return (b.type() == typeid(T)) ? a + poly::cast<T>(b) : a;
// }

namespace poly {

template <typename T>
::addable call(operator_add_, T const & a, ::addable const & b) {
    return a + cast<T>(b);
}

} // poly

int main() {
    addable a = 1;
    addable b = a;
    //addable c = a + b; // doesn't work yet...
    std::cout << poly::operator_add(1, 2) << std::endl;
    std::cout << poly::cast<int>(poly::operator_add(a, b)) << std::endl;
}
