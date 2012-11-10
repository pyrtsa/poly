#ifndef GRAPHICS_OBJECT_HPP
#define GRAPHICS_OBJECT_HPP

#include <poly/interface.hpp>

namespace graphics {

    using drawable = poly::interface<
        void(draw_fn, context &, poly::self const &)>;

    struct object : poly::interface<object,
          void(draw_fn, context &, poly::self const &)
        , size_type(size_fn, poly::self const &)
        , point_type(center_fn, poly::self const &)
        , void(size_fn, poly::self &, size_type)
        , void(center_fn, poly::self &, point_type)
        >
    {
        POLY_INTERFACE_CONSTRUCTORS(object);
    };

} // graphics

#endif // GRAPHICS_OBJECT_HPP
