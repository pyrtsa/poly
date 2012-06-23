// Copyright 2012 Pyry Jahkola.
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// -----------------------------------------------------------------------------

/// Example `callable1.cpp`
/// =======================
///
/// Demonstrate the use of callables:
///
/// - their relation to argument-dependent name lookup (ADL),
/// - `constexpr` support,
/// - overloading callables with and without templates, and
/// - the use of callables in functional constructs like `std::transform`.

// -----------------------------------------------------------------------------

#include <poly/callable.hpp>
#include <vector>
#include <algorithm>
#include <iostream>

namespace example {

    /// Callable `example::minus(a, ...)` --- Minus operator for 1 or more args.

    POLY_CALLABLE(minus);

    template <typename A>
    constexpr auto call(minus_, A const & a)
    POLY_RETURNS(-a);

    template <typename A, typename B>
    constexpr auto call(minus_, A const & a, B const & b)
    POLY_RETURNS(a - b);

    template <typename A, typename B, typename... More>
    constexpr auto call(minus_, A const & a, B const & b, More const &... more)
    POLY_RETURNS(minus(a - b, more...));

    /// Callable `example::print(x)` --- Print x to std::cout.

    POLY_CALLABLE(print);

    template <typename T,
              typename Enable=decltype(
                  std::declval<std::ostream &>() << std::declval<T const &>())>
    void call(print_, T const & x) {
        std::cout << x << std::endl;
    }

    void call(print_, bool x) {
        std::cout << (x ? "true" : "false") << std::endl;
    }

    template <typename T>
    void call(print_, std::vector<T> const & xs) {
        std::cout << '[';
        auto i = begin(xs);
        auto e = end(xs);
        if (i != e) std::cout << *i++;
        while (i != e) std::cout << ", " << *i++;
        std::cout << ']' << std::endl;
    }

} // example

namespace ns {
    template <typename T>
    void print(T const &) {
        std::cout
            << "This function never get called in this example" << std::endl;
    }

    struct abc {};

    /// In case there's no ambiguity in the name lookup, the `call` function can
    /// be defined in the namespace (`namespace ns`) of an argument
    /// (`struct ns::abc`) instead of the namespace of the callable
    /// (`namespace example` in this case).
    void call(example::print_, abc) {
        std::cout << "struct abc" << std::endl;
    }
} // ns

int main() {
    /// 1) Use the `minus` and `print` callables from the `example` namespace.
    using example::minus;
    using example::print; /// Hides `::print`. Also disables ADL!

    /// On the one hand, we could as well use `example::minus(...)` in this
    /// example. But having `example::print` defined as an object and not a
    /// function, there is the nice property that `using example::print;`
    /// disables the argument-dependent lookup (ADL) entirely in the current
    /// scope.
    /// 
    /// Thus, `ns::print` will never get called in the code below, even if it
    /// would normally be picked when calling `print(abc())`!

    /// 2) Show how the result type computation works.
    int i = 4;
    auto r = minus(i, 1.5);
    print(r); //=> 2.5
    print(std::is_same<decltype(r), double>::value); //=> true

    /// 3) An example of function overloading. Computes `1 - 2 - 3.5 - (-4)`.
    print(minus(1, 2, 3.5, -4)); //=> -0.5

    /// 4) Demonstrate that `constexpr` is supported with callables.
    using two = std::integral_constant<int, minus(5, 3)>;
    print(two::value); //=> 2

    /// 5) Show how ADL is prevented when there is an active `using` declaration
    /// of a callable in the current scope. (I.e. this won't call `ns::print`.)
    print(ns::abc()); //=> struct abc

    /// 6) There is a specialization of print for `std::vector`.
    std::vector<int> xs = {1, 2, 3};
    print(xs); //=> [1, 2, 3]

    /// 7) Callables can be used as 1st class citizens in functional constructs.
    std::vector<int> ys(xs.size());
    std::transform(begin(xs), end(xs),
                   begin(ys), minus);
    print(ys); //=> [-1, -2, -3]

    /// 8) When used as a polymorpic function object like this, `std::transform`
    /// is able to pick the right overload --- `minus(x, y)` in this case.
    std::vector<int> zs(xs.size());
    std::transform(begin(xs), end(xs),
                   begin(ys), begin(zs), minus);
    print(zs); //=> [2, 4, 6]
}
