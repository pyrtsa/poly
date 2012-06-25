Poly. Solving the [Expression Problem][ep] in C++11
===================================================

_This library will likely change how you thought about polymorphism in C++._

**Remark.** Still a proof-of-concept, probably too early or fragile for production use yet.

Quickly, show me the code!
--------------------------

Here is an example header file.

    // "drawable.hpp"
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

(For comparison and reference, see the [presentation video][parent-cppnow-video] by Sean Parent from [C++Now!][cppnow] 2012, with source code [here][parent-cppnow-code].)

This is how the header might be used. The types `int`, `std::vector<int>`, `std::string`, `my::klass`, and even `std::vector<example::drawable>` almost magically convert to `example::drawable`:

    // "main.cpp"
    #include "drawable.hpp"
    #include <iostream>
    
    namespace my { struct klass {}; }
    
    namespace example {
        // Implement example::drawable for my::klass.
        void call(draw_, my::klass, std::ostream & o, std::size_t p) {
            o << std::string(p, ' ') << "my klass" << std::endl;
        }
    }
    
    using document = std::vector<example::drawable>;
    
    int main() {
        document doc;
        
        doc.push_back(123);
        doc.push_back(std::vector<int>{1, 2, 3});
        doc.push_back(std::string("a string!"));
        doc.push_back(doc);
        doc.push_back(my::klass());
        
        std::cout << "--- HERE WE GO: ---" << std::endl;
        example::draw(doc, std::cout, 0);
        std::cout << "--- DONE. ---" << std::endl;
    }


Output
------

    --- HERE WE GO: ---
    <document>
      123
      <document>
        1
        2
        3
      </document>
      a string!
      <document>
        123
        <document>
          1
          2
          3
        </document>
        a string!
      </document>
      my klass
    </document>
    --- DONE. ---


--------------------------------------------------------------------------------


Explanation
-----------

Given the above header file code, we have:

- created a _callable_ named `example::draw`,
- defined the _interface_ `example::drawable`, and
- defined two generic implementations for `example::draw` — one for ostreamable types, another for `std::vector<T>` of any `T` implementing the call to `example::draw(x, o, p)`.

This setup makes `example::run` generic in the sense that you can pass in anything for which the `example::drawable` interface is implemented, be it your class, a class written (and compiled) by somebody else, or even a built-in type like `int`!

To implement an interface for a given class `my::klass`, see the function signatures in the definition of the interface. In the case of `example::drawable`, there's only one:

    void(example::draw_, poly::self const &, std::ostream &, std::size_t)

In your mind, replace `poly::self` with your class name. The line above reads that the call

    example::draw(x, o, p);

is expected to be an expression convertible to `void`, given `my::klass const & x`, `std::ostream & o`, and `std::size_t p`. You can implement exactly this signature by overloading the free function named `call` for `example::draw_` as the first argument, followed by the said set of three arguments:

    namespace example {
        void call(draw_, my::klass const &, std::ostream &, std::size_t);
    }

The `call` function needs to be found through _argument-dependent name lookup_ ([ADL][adl]), so in this case it could have been defined in the namespace `my` as well (although the former is recommended for consistency).


Is it any good?
---------------

Yes.

- `poly::interface` is **move-aware** — you can cheaply move values of different (move-aware) types into the interface.
- **Types can be perfectly oblivious** about the interfaces they need to implement. (This is a Big Deal. See the talk about the _expression problem_ below.)
- There are **no raw pointers** you need to mess around with. It is perfectly clear, who owns the object and when it is destroyed.
- There is **no shared state**. Objects of type `poly::interface<...>` act as values, which is known to be a nice property in multithreaded applications.
- There's a backdoor too: If you can handle the type `T` wrapped in a `poly::interface<...> x`, you can **cast back to it** using `poly::cast<T>(x)`. You can even _move_ the value out: `poly::cast<T>(std::move(x))`. (Introspect the wrapped value by asking `x.type()`. It returns `std::type_info const &` like `typeid(...)`.)


How to compile the examples?
----------------------------

The library is header-only. Just add the `include` folder to your library include path, and compile in the C++11 mode (with a decent C++11-aware standard library). Here are two build commands that have worked for me. [Clang][clang] with [libc++][libcxx] (trunk builds from both):

    mkdir -p bin/example
    export CPLUS_INCLUDE_PATH=/path/to/libcxx/include
    export    LD_LIBRARY_PATH=/path/to/libcxx/lib
    /path/to/llvm-build/Release/bin/clang++ \
        -std=c++11 \
        -stdlib=libc++ \
        -ftemplate-depth-128 \
        -Iinclude \
        example/main.cpp -o bin/example/main

GCC 4.7.1:

    mkdir -p bin/example
    g++-4.7 \
        -std=c++11 \
        -ftemplate-depth-128 \
        -Iinclude \
        example/main.cpp -o bin/example/main


What are _callables_?
---------------------

They are externally overloadable polymorpic function objects. (Phew!) That's for short. Before diving into the long explanation, let's define a callable — this time, _without_ using the convenience macro `POLY_CALLABLE(name)`.

    #include <poly/callable.hpp>
    
    namespace ns {
        struct add_ : poly::callable<add_> {};
        constexpr add_ add = {};
    }

See? No function definition yet. That's correct, you couldn't use this piece of code much, not before adding an implementation like:

    namespace ns {
        int call(add_, int a, int b) { return a + b; }
        double call(add_, double a, double b) { return a + b; }
    }

Now it's possible to _call_ the callable:

    int main() {
        if (ns::add(-1.0, 2.0) != 0)
            return ns::add(-10, 10);
        return -1;
    } // returns 0, no error


Tell me more about callables
----------------------------

Calling a callable actually just forwards the callable and the arguments to the free function `call`, found by _argument-dependent name lookup_ ([ADL][adl]):

    ns::add(-1.0, 2.0); // equivalent to: call(ns::add, -1.0, 2.0);

In the previous example, you saw how to overload callables for simple types. You can use templates as well:

    // ...
    
    namespace ns {
        template <typename A, typename B>
        auto call(add_, A const & a, B const & b)
        -> decltype(a + b) { return a + b; }
    }
    
    #include <string>
    #include <iostream>
    
    int main() {
        std::string a = "abc";
        std::string b = "def";
        std::cout << ns::add(a, b) << std::endl; //=> abcdef
    }

But you can overload even more:

    // ...
    
    namespace ns {
        template <typename A, typename B, typename... More>
        auto call(add_, A const & a, B const & b, More const &... more)
        -> decltype(ns::add(a + b, more...)) { return ns::add(a + b, more...); }
    }
    
    int main() { return ns::add(short(1), 2, 3, 4, -10); } // still no error

Basically, callables are stateless function objects which decide their implementation not before they are called with some set of arguments. And as function objects they can be passed around in functional constructs like `std::for_each` or `std::bind`.

    // ...
    
    #include <algorithm>
    #include <functional>
    #include <vector>
    #include <iostream>
    
    int main() {
        using std::placeholders::_1;
        std::vector<char> secret = {8, 5, 12, 12, 15};
        std::string message;
        std::transform(begin(secret), end(secret), begin(secret),
                       back_inserter(message), std::bind(ns::add, 'a' - 1, _1));
        std::cout << message << std::endl; //=> hello
    }

Callables use [ADL][adl] to delay the selection of implementation, and as such, it is enough that the declaration (of `call(...)`) is visible before the function calling the callable is instantiated (or the before the calling piece of code, if it's not a template).

Oh, and by the way, callables should be pretty good at supporting `inline`, `constexpr`, and `noexcept`, should you use them in your definition of `call(...)`. In the following example, the use of `constexpr` is demonstrated.

    POLY_CALLABLE(minus);
    
    template <typename A, typename B>
    constexpr auto call(minus_, A const & a, B const & b)
    POLY_RETURNS(a - b);
    
    template <typename A, typename B, typename... More>
    constexpr auto call(minus_, A const & a, B const & b, More const &... more)
    POLY_RETURNS(minus(a - b, more...));
    
    using two = std::integral_constant<int, minus(5, 3)>; // minus as constexpr!
    print(two::value); //=> 2

Here, some more boilerplate was saved by using the `POLY_RETURNS(...)` macro, which conveniently avoids the need for `noexcept` and result type computation with `auto`.


But I like to define my member functions within my class
--------------------------------------------------------

Shame on you, for missing the point of separating code and data! However, it's still possible to define `call(...)` as part of a class definition:

    namespace my {
        class xxx {
            int i;
        public:
            explicit xxx(int i) : i(i) {}
            friend void call(example::draw_, xxx const & x,
                             std::ostream & o, std::size_t p)
            {
                o << std::string(p, ' ') << "xxx(" << x.i << ')' << std::endl;
            }
        };
    }
    
    int main() {
        my::xxx x(123);
        example::draw(x, std::cout, 0);  //=> xxx(123)
    }


How does `poly::interface` actually work?
-----------------------------------------

Behind the curtains, it creates a wrapper template around the type you construct it from, and holds a unique pointer to that wrapper. At the point of instantiating the wrapper, `poly::interface<...>` checks that all of the function signatures are implemented for the type provided.

Then, `poly::interface<...>` defines the `call` functions (as `friend` to itself) by forwarding the calls to the wrapper (using virtual functions internally). As a result, any callable listed in the definition of the interface is automatically overloaded for that `poly::interface<...>` itself.

In other words, given for example:

    using interface1 = poly::interface<
        result1(callable1_, poly::self &),
        result2(callable2_, poly::self &&, int),
        result3(callable3_, std::ostream &, poly::self const &)>;

the following functions get automatically defined:

    result1 call(callable1_, interface1 & x);
    result2 call(callable2_, interface1 && x, int i);
    result3 call(callable3_, std::ostream & o, interface1 const & x);

Any type `T` that needs to be convertible to `interface1` will then need to have `callable1`, `callable2`, and `callable3` likewise implemented.


I get nasty compiler errors
---------------------------

Do yourself a favor and use a (very) recent version of [Clang][clang]. :-P


Right, "thanks", but I'm using GCC
----------------------------------

Ok, at least GCC 4.7.1 seems to work fine. But its error messages still suck. (Ok ok, they aren't too good in the worst cases in Clang either. But still much better)

One more thing you can do (be it whatever compiler) is simplify the type name of your interface by passing it as the first [CRTP][crtp] template argument to `poly::interface`. Instead of:

    using drawable = poly::interface<
        void(draw_, poly::self const &, std::ostream &, std::size_t)>;

you can write:

    struct drawable : poly::interface<drawable,
        void(draw_, poly::self const &, std::ostream &, std::size_t)>
    { POLY_INTERFACE_CONSTRUCTORS(drawable); };

At least, the list of function signatures won't show up _that often_ in the error messages.


How to make the interface return its own type?
----------------------------------------------

You might need your interface to have "member" functions returning its own type. For example, a negate function. This is possible by using the _curiously recurring template pattern_ ([CRTP][crtp]) which `poly::interface` supports.

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


How to get back the value I originally put in?
----------------------------------------------

You can introspect the type of the object by asking `x.type()`.

    example::drawable x = 123;
    assert(x.type() == typeid(int));

Now you can cast the int back using `poly::cast<T>(x)`:

    int i = poly::cast<int>(x);

If the cast fails, an exception of type `poly::bad_cast` (inherits `std::bad_cast`) is thrown. Like in [Boost.Any][boost-any], you can avoid the exception by passing in a pointer to the interface:

    double * p = poly::cast<double>(&x);
    if (p) {
        // this branch is missed, because x was an int!
    } else {
        // wasn't a double!
    }

You can even move the value out by casting, when you pass in an rvalue.

    auto is = std::vector<int>{1, 2, 3};
    int ptr = &is[0];
    example::drawable xs = std::move(is);
    assert(is.empty());
    auto js = poly::cast<std::vector<int>>(std::move(xs)); // move data from xs
    assert(poly::cast<std::vector<int>>(xs).empty()); // xs is now "moved-from"
    assert(&js[0] == ptr); // still the same vector


Isn't this just a big hack?
---------------------------

Well, yes. Maybe not _that_ big.

Argument-dependent name lookup is known to be brittle, and you'll likely get problems with ODR (one definition rule) if you aren't careful and end up implementing an interface for some type differently in two compilation units. (**TODO:** Add an example and counter-example demonstrating this!) But surely you know about these issues as a proficient C++ programmer anyway, don't you?

A good way to deal with interface implementations — should the specialization be used in several source files — might be to dedicate an own header/source file pair for the implementation:

    // "klass_as_drawable.hpp"
    #ifndef KLASS_AS_DRAWABLE_HPP_O8DBR02
    #define KLASS_AS_DRAWABLE_HPP_O8DBR02
    
    #include "klass.hpp"    // namespace my { struct klass {}; }
    #include "drawable.hpp" // namespace example { using drawable = ... ; }
    
    namespace example {
        void call(draw_, my::klass const&, std::ostream&, std::size_t);
    }
    
    #endif // KLASS_AS_DRAWABLE_HPP_O8DBR02

To header file above would then be included by anybody using `my::klass` as `example::drawable`. The source file with the implementation could be written as:

    // "klass_as_drawable.cpp"
    #include "klass_as_drawable.cpp"
    
    void example::call(example::draw_,
                       my::klass const&, std::ostream& o, std::size_t p)
    {
        o << std::string(p, ' ') << "my klass" << std::endl;
    }


Isn't this just like Haskell's [type classes][typeclass] or Clojure's [protocols][protocol]?
-----------------------------------------------------------------------

That's what I'm trying to do, so yes.

Well, with the exception that there is no garbage collector involved in this case. And that the implementations are resolved at compile time as much as possible, and that the runtime cost is that of a virtual function lookup per call.

Having `poly::interface<...>` means, you can add or remove interfaces for arbitrary value types without modifying or recompiling their source code. Likewise, you can change an already used interface, and you only need to recompile those files where the interface is used. If I'm not missing a point, this idea turns out to solve the [_expression problem_][ep] in C++11.


What are the requirements?
--------------------------

1. A recent C++11 compiler. Clang 3.1 (or trunk) and GCC 4.7 should work. Visual C++ probably won't. I don't know about the other compilers yet. (I've only tested it on OS X so far.) Compile with `-std=c++11` enabled.
2. The type implementing the interface needs to be copyable and movable. (This might change to only require types being movable, when I'm a bit more informed about the use cases.)
3. The interface must be implemented equally for a given type in all compilation units converting the type to the interface. This requirement exists to not violate the one definition rule (ODR).
4. Every function signature passed as template arguments to `poly::interface` needs to start with a _callable_ type as first argument. In addition, one of the arguments shall be either of
  - `poly::self &`,
  - `poly::self const &`,
  - `poly::self &&`, or
  - `poly::self`, corresponding to `*this` (or a copy of `*this`).

The easiest to understand position for the `self` argument is as the first actual argument, immediately after the callable.


Shortcomings and Further Development
------------------------------------

1. `poly::interface<...>` requires the types to be not only _movable_ but also _copyable_. — It probably makes sense to offer an alternative where only movable is required, maybe something like `poly::interface<poly::noncopyable, ...>`. I'll need to first see how `poly::interface` can or cannot be used as it currently is, before implementing that feature. **Please, tell me if you need a version of `poly::interface` for non-copyable types.**
2. The support for `poly::cast<T>(x)` and `std::type_info` is always enabled in `poly::interface`, even if it might not be needed. — Again, this feature might be made optional, but I'd like to learn about its possible uses first.
3. There is no (simple) way to create a `poly::interface` with reference semantics. — This is intentional. I'm trying to restrict to value semantics with this. (You can hack around this by using `std::ref(x)` and specializing `std::reference_wrapper<T>`. But on your own risk.) Again, if there is point in allowing reference semantics, let's reconsider.
4. `poly::interface<...>` relies on virtual functions internally. I have no idea yet, whether this incurs a performance penalty compared to alternative approaches. The internals might change when there is some data to justify an optimization.
5. Unit tests are missing. — Oh well, they're coming. In the meantime, deal with my products of _Example Driven Development_ in the `example` directory.
6. Error messages may be tough to decipher. — I'll try my best to make them simpler. With tools `static_assert` in place, there's at least some hope. Feel free to help if you have any insight on improving the diagnostics.
7. This might make sense to be as part of the [Boost C++ Libraries][boost]. — Maybe, yes. I'm looking forward to it. But there is a [similar proposal][watanabe-type-erasure] out already, and Steven Watanabe has done pretty good work (and much more so than me) already. Let's see if we can combine our efforts somehow.


--------------------------------------------------------------------------------

Thanks
------

- Sean Parent, for an [enlightening talk][parent-cppnow-video] about value semantics and concepts-based polymorphism in the C++Now! 2012 conference, and for the inspiration for this proof-of-concept library. See his [slides and example code][parent-cppnow] in GitHub.
- Johanness "litb" Schaub, for teaching me [how ADL works][litb-adl], and Joel Falcou et al., for showing how to further [trick the ADL system][nt2-adl].
- Dave Abrahams, for the combined `RETURNS(...)` / `noexcept(noexcept(...))` [trick][abrahams-meta] as well as some other metaprogramming tricks used.
- Steven Watanabe, for the idea of emulating [`boost::any`][boost-any], and very likely several other ideas used in this library, like operator overloading or the use of `self` as a placeholder type, all inherited from his proposed [Boost.TypeErasure][watanabe-type-erasure] library (see [docs][watanabe-docs]). Here's me hoping we can somehow unite our efforts, and bring type erasure and non-intrusive interfaces to the mainstream of C++ programming.


--------------------------------------------------------------------------------

License
-------

Copyright 2012 Pyry Jahkola.

Distributed under the Boost Software License, Version 1.0. (See accompanying file `LICENSE_1_0.txt` or copy at <http://www.boost.org/LICENSE_1_0.txt>)


--------------------------------------------------------------------------------

Contact
-------

**Pyry Jahkola**

Twitter: [@pyrtsa](https://twitter.com/pyrtsa)  
Email: <pyry.jahkola@iki.fi>  
Blog: <http://pyrtsa.posterous.com>  

I write software at [Futurice][futurice].  
Have you [heard][gptw-2012] about us already?


[abrahams-meta]: http://thread.gmane.org/gmane.comp.lib.boost.devel/229563/focus=229649
[adl]: http://en.wikipedia.org/wiki/Argument-dependent_name_lookup
[boost-any]: http://boost.org/libs/any
[boost]: http://boost.org
[clang-trunk]: http://clang.llvm.org/get_started.html
[clang]: http://clang.llvm.org
[cppnow]: http://cppnow.org
[crtp]: http://en.wikipedia.org/wiki/Curiously_recurring_template_pattern
[ep]: http://en.wikipedia.org/wiki/Expression_Problem
[gptw-2012]: http://blog.futurice.com/futurice-europes-best-workplace
[futurice]: http://www.futurice.com
[libcxx]: http://libcxx.llvm.org
[litb-adl]: http://stackoverflow.com/a/5707849
[nt2-adl]: https://github.com/MetaScale/nt2/commit/f41d74fb93791b63f0ad09529fe4d69b1c46421d
[parent-cppnow-code]: https://github.com/boostcon/cppnow_presentations_2012/blob/master/fri/value_semantics/value_semantics.cpp
[parent-cppnow-video]: http://www.youtube.com/watch?v=_BpMYeUFXv8
[parent-cppnow]: https://github.com/boostcon/cppnow_presentations_2012/tree/master/fri/value_semantics
[protocol]: http://clojure.org/protocols
[typeclass]: http://www.haskell.org/tutorial/classes.html
[watanabe-docs]: http://steven_watanabe.users.sourceforge.net/type_erasure/libs/type_erasure/
[watanabe-type-erasure]: http://svn.boost.org/svn/boost/sandbox/type_erasure/
