// Copyright 2012 Pyry Jahkola.
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <poly/interface.hpp>
#include <iostream>

POLY_CALLABLE(negate);
POLY_CALLABLE(print);

template <typename T>
auto call(negate_, T && x) POLY_RETURNS(-std::forward<T>(x));

template <typename T>
void call(print_, T const & x) { std::cout << x << std::endl; }

struct negatable : poly::interface<negatable,
    negatable(negate_, poly::self const &),
    void(print_, poly::self const &)>
{ POLY_INTERFACE_CONSTRUCTORS(negatable); };

int main() {
    negatable i = 123;
    negatable j = negate(i);
    print(i); //=> 123
    print(j); //=> -123
}
