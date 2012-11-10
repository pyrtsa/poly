/*
    Copyright 2012 Adobe Systems Incorporated
    Distributed under the MIT License (see license at
    http://stlab.adobe.com/licenses.html)
    
    This file is intended as example code and is not production quality.
*/

#ifndef COPY_ON_WRITE_HPP
#define COPY_ON_WRITE_HPP

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cassert>

/******************************************************************************/
// copy_on_write

/*
    Disclaimer:

    This is a simplified version of copy_on_write from the Adobe Source
    Libraries. It has not been well tested, thread safety has not been
    tested at all as my compiler doesn't support std::atomic<> yet.
    
    The version in ASL uses Intel TBB's atomic type and has been well
    tested.
*/

#if __has_feature(cxx_atomic)
#include <atomic>
using counter_t = std::atomic<size_t>;
#else
using counter_t = size_t;
#endif

/*
A copy-on-write wrapper for any model of a regular type.

Requirements:
    T models regular.

Copy-on-write sematics allow for object to be lazily copied - only creating a
copy when the value is modified and there is more than one reference to the
value.

Note:
    copy_on_write is thread safe when C++11 atomics are supported.

*/

template <typename T>
struct cow_implementation_t
{
    using value_type = T;

    explicit cow_implementation_t(T x)
        : count_m(1)
        , value_m(std::move(x))
    { }

    counter_t   count_m;
    value_type  value_m;
};

template <typename T>
std::size_t cow_use_count(cow_implementation_t<T> const & x) { return x.count_m; }
template <typename T>
void cow_increment(cow_implementation_t<T> & x) { ++x.count_m; }
template <typename T>
bool cow_decrement(cow_implementation_t<T> & x) { return --x.count_m; }
template <typename T>
T & cow_value(cow_implementation_t<T> & x) { return x.value_m; }
template <typename T>
void cow_destroy(cow_implementation_t<T> * x) { delete x; }

template <typename T, // models Regular
          typename Cow = cow_implementation_t<T>>
class copy_on_write
{
public:
    // The type of value stored.
    using value_type = T;
    using implementation_t = Cow;

    /*
        The first call to the default constructor will construct a default
        instance of value_type which will be used for subsequent calls to the
        default constructor. The default instance will be released at exit.
    */
    copy_on_write()
        : object_m(0)
    {
        /*
            NOTE : for thread safety this assumes static initization is
            thread safe - required in C++11. In non-compliant compilers
            use a once-init library.
        */
    
        // static implementation_t default_s{value_type()};
        // object_m = &default_s;
        // ++object_m->count_m;
    }

    /*
        Constructs a new copy_on_write object with a value x.

        Paramter: x A default value to assign to this object
    */
    copy_on_write(T x) :
        object_m(new implementation_t(std::move(x)))
    {}

    template <typename U>
    copy_on_write(std::initializer_list<U> u)
        : object_m(new implementation_t(T(std::move(u))))
    {}

    explicit copy_on_write(implementation_t* ptr) : object_m(ptr) {}

    /*
        Copy construction is a non-throwing operation and simply increments
        the reference count on the stored object.
    */
    copy_on_write(const copy_on_write& x) noexcept :
        object_m(x.object_m)
    {
        if (object_m) cow_increment(*object_m);
    }

    copy_on_write(copy_on_write&& x) noexcept :
        object_m(x.object_m)
    {
        x.object_m = 0;
    }

    ~copy_on_write()
    {
        release();
    }

    /*
        As with copy construction, assignment is a non-throwing operation which
        releases the old value and increments the reference count of the item
        being assigned to.
    */
    copy_on_write& operator=(copy_on_write x) noexcept
    { std::swap(object_m, x.object_m); return *this; }


    copy_on_write& operator=(T x)
    {
        if (!object_m) object_m = new implementation_t(std::move(x));
        else if (cow_use_count(*object_m) == 1) cow_value(*object_m) = std::move(x);
        else reset(new implementation_t(std::move(x)));

        return *this;
    }

    /*
    Obtain a reference to the value the object is referencing. This will copy
    the underlying value (if necessary) so changes to the value do not affect
    other copy_on_write objects.

    Note that write() does not have the same preconditions as operator=().
    write() returns a reference to the underlying object's value, thus requiring
    that an underlying object exist. operator=() on the other hand will perform
    an allocation if one is necessary.

    Return: A reference to the underlying object
    */
    value_type& write()
    {
        assert(object_m && "FATAL : using a moved copy_on_write object");

        if (cow_use_count(*object_m) != 1)
            reset(new implementation_t(cow_value(*object_m)));

        return cow_value(*object_m);
    }

    /*!
    Obtain a const reference to the underlying object.

    Return: A const reference to the underlying object
    */
    const value_type& read() const noexcept
    {
        assert(object_m && "FATAL : using a moved copy_on_write object");
        return cow_value(*object_m);
    }
    
    bool operator==(copy_on_write const & r) const {
        return object_m == r.object_m ||
              (object_m && r.object_m && object_m->value_m == r.object_m->value_m);
    }
    
    bool operator!=(copy_on_write const & r) const { return !(*this == r); }

private:
    void release()
    {
        if (!object_m || cow_decrement(*object_m)) return;
        
        cow_destroy(object_m);
    }

    void reset(implementation_t* to)
    {
        release();
        object_m = to;
    }

    implementation_t* object_m;
};

#endif // COPY_ON_WRITE_HPP
