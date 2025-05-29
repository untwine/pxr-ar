// Copyright 2017 Pixar
//
// Licensed under the terms set forth in the LICENSE.txt file available at
// https://openusd.org/license.
//
// Modified by Jeremy Retailleau.

#include <pxr/boost/python/class.hpp>

#include <pxr/ar/defaultResolver.h>

using namespace pxr;

using namespace pxr::boost::python;

void
wrapDefaultResolver()
{
    using This = ArDefaultResolver;

    class_<This, bases<ArResolver>, noncopyable>
        ("DefaultResolver", no_init)

        .def("SetDefaultSearchPath", &This::SetDefaultSearchPath,
             args("searchPath"))
        .staticmethod("SetDefaultSearchPath")
        ;
}
