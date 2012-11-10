#include "copy_on_write.hpp"
#include <cassert>
#include <iostream>
#include <sstream>

template <typename T>
std::ostream & repr(std::ostream & out, std::vector<T> const & v) {
    using std::begin;
    using std::end;
    auto i = begin(v), e = end(v);
    out << '[';
    if    (i != e) out << *i++;
    while (i != e) out << ", " << *i++;
    out << ']';
    return out;
}

template <typename T>
std::string repr(T const & x) {
    std::ostringstream o;
    repr(o, x);
    return o.str();
}

template <typename T>
void show(T const & x) { repr(std::cout, x) << std::endl; }

using std::vector;

int main() {
    using ints = copy_on_write<vector<int>>;
    ints a = {1, 2, 3};
    ints b = a;

    vector<int> const & ar = a.read();     // .read() returns `T const &`.
    vector<int> const & br = b.read();
    assert(ar == br && &ar[0] == &br[0]);  // Vectors share the same memory

    vector<int> & r = a.write();           // Vector is copied when shared.
    r[0] = 42;                             // Modify newly copied vector.

    assert(a.read()[0] == 42 && b.read()[0] == 1);
    
    assert((a == ints{42, 2, 3}) && (b == ints{1, 2, 3}));
    show(a.read());
    show(b.read());
}
