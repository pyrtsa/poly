// Copyright 2012 Pyry Jahkola.
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <poly/interface.hpp>
#include <iostream>

namespace ns {

    POLY_CALLABLE(increment); // increment(x), increment(in_place, x)

    constexpr struct in_place_ {} in_place = {};

    template <typename T> T call(increment_, T t) {
        ns::increment(in_place, t);
        return t;
    }

    struct incrementable : poly::interface<incrementable
    ,   void(increment_, in_place_, poly::self &)
    ,   incrementable(increment_, poly::self)
    > { POLY_INTERFACE_CONSTRUCTORS(incrementable); };

    void call(increment_, in_place_, int & i) { i++; }

} // ns

int main() {
    ns::incrementable x = 0;
    auto y = ns::increment(x);
    ns::increment(ns::in_place, y);
    std::cout << "result: " << poly::cast<int>(y) << std::endl;
}
