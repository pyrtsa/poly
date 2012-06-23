// Copyright 2012 Pyry Jahkola.
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <poly/callable.hpp>
#include <algorithm>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

namespace ns {
    struct add_ : poly::callable<add_> {};
    constexpr add_ add = {};

    int call(add_, int a, int b) { return a + b; }
    double call(add_, double a, double b) { return a + b; }

    template <typename A, typename B>
    auto call(add_, A const & a, B const & b)
    -> decltype(a + b) { return a + b; }

    template <typename A, typename B, typename... More>
    auto call(add_, A const & a, B const & b, More const &... more)
    -> decltype(ns::add(a + b, more...)) { return ns::add(a + b, more...); }
}

int main() {
    std::cout << ns::add(-10, 10) << std::endl;                 //=> 0
    std::cout << ns::add(-1.0, 2.0) << std::endl;               //=> 1
    std::cout << ns::add(short(1), 2, 3, 4, -10) << std::endl;  //=> 0
    
    using std::placeholders::_1;
    std::vector<char> secret = {8, 5, 12, 12, 15};
    std::string message;
    std::transform(begin(secret), end(secret), begin(secret),
                   back_inserter(message), std::bind(ns::add, 'a' - 1, _1));
    std::cout << message << std::endl;                          //=> hello
}
