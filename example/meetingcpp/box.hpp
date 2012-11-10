// Copyright 2012 Pyry Jahkola.
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOX_HPP
#define BOX_HPP

#include <string>

namespace theirs {

    struct box {
        double left, top, right, bottom;

        box(double l, double t, double r, double b)
            : left(l), top(t), right(r), bottom(b) {}

        std::string to_json() const { // in no way optimal...
            return "[[" + std::to_string(left) + ", "
                        + std::to_string(top) + "], ["
                        + std::to_string(right) + ", "
                        + std::to_string(bottom) + "]]";
        }
    };

} // theirs

#endif // BOX_HPP
