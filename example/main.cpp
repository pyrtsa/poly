// Copyright 2012 Pyry Jahkola.
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

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
