// Copyright 2020 Pixar
//
// Licensed under the terms set forth in the LICENSE.txt file available at
// https://openusd.org/license.
//
// Modified by Jeremy Retailleau.

#ifndef PXR_AR_PY_RESOLVER_CONTEXT_H
#define PXR_AR_PY_RESOLVER_CONTEXT_H

/// \file ar/pyResolverContext.h
/// Macros for creating Python bindings for objects used with 
/// ArResolverContext.

#include <pxr/boost/python/implicit.hpp>

#include "./resolverContext.h"

namespace pxr {

/// Register the specified type as a context object that may be converted from
/// Python into a ArResolverContext object in C++.  This typically would be
/// called in the source file where the Python wrapping for the context object
/// is defined.
template <class Context>
void 
ArWrapResolverContextForPython();

#ifndef doxygen

template <class Context>
void 
ArWrapResolverContextForPython()
{
    pxr::boost::python::implicitly_convertible<Context, ArResolverContext>();
};

#endif //doxygen

}  // namespace pxr

#endif
