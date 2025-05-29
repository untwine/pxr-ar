// Copyright 2016 Pixar
//
// Licensed under the terms set forth in the LICENSE.txt file available at
// https://openusd.org/license.
//
// Modified by Jeremy Retailleau.

#include <pxr/boost/python/class.hpp>
#include <pxr/boost/python/object.hpp>

#include <pxr/ar/resolverContextBinder.h>

#include <memory>

using namespace pxr;

using namespace pxr::boost::python;

namespace {

class _PyResolverContextBinder
{
public:
    _PyResolverContextBinder(const ArResolverContext& context)
        : _context(context)
    {
    }

    _PyResolverContextBinder(const _PyResolverContextBinder&) = delete;
    
    _PyResolverContextBinder& 
    operator=(const _PyResolverContextBinder&) = delete;

    void Enter()
    {
        _binder.reset(new ArResolverContextBinder(_context));
    }

    bool Exit(
        pxr::boost::python::object& /* exc_type */,
        pxr::boost::python::object& /* exc_val  */,
        pxr::boost::python::object& /* exc_tb   */)
    {
        _binder.reset(0);
        // Re-raise exceptions.
        return false;
    }

private:
    ArResolverContext _context;
    std::unique_ptr<ArResolverContextBinder> _binder;
};

} // anonymous namespace 

void
wrapResolverContextBinder()
{
    typedef _PyResolverContextBinder This;

    class_<This, noncopyable>
        ("ResolverContextBinder", init<const ArResolverContext&>())
        .def("__enter__", &This::Enter)
        .def("__exit__", &This::Exit)
        ;
}
