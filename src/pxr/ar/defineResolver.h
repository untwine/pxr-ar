// Copyright 2016 Pixar
//
// Licensed under the terms set forth in the LICENSE.txt file available at
// https://openusd.org/license.
//
// Modified by Jeremy Retailleau.

#ifndef PXR_AR_DEFINE_RESOLVER_H
#define PXR_AR_DEFINE_RESOLVER_H

/// \file ar/defineResolver.h
/// Macros for defining a custom resolver implementation.

#include "./api.h"
#include "./resolver.h"

#include <pxr/tf/registryManager.h>
#include <pxr/tf/type.h>

namespace pxr {

/// \def AR_DEFINE_RESOLVER
///
/// Performs registrations required for the specified resolver class
/// to be discovered by Ar's plugin mechanism. This typically would be
/// invoked in the source file defining the resolver class. For example:
///
/// \code
/// // in .cpp file
/// AR_DEFINE_RESOLVER(CustomResolverClass, ArResolver);
/// \endcode
#ifdef doxygen
#define AR_DEFINE_RESOLVER(ResolverClass, BaseClass1, ...)
#else
#define AR_DEFINE_RESOLVER(...)       \
TF_REGISTRY_FUNCTION(pxr::TfType) {   \
    Ar_DefineResolver<__VA_ARGS__>(); \
}
#endif // doxygen

class Ar_ResolverFactoryBase 
    : public TfType::FactoryBase 
{
public:
    AR_API
    virtual ~Ar_ResolverFactoryBase();

    AR_API
    virtual ArResolver* New() const = 0;
};

template <class T>
class Ar_ResolverFactory 
    : public Ar_ResolverFactoryBase 
{
public:
    virtual ArResolver* New() const override
    {
        return new T;
    }
};

template <class Resolver, class ...Bases>
void Ar_DefineResolver()
{
    TfType::Define<Resolver, TfType::Bases<Bases...>>()
        .template SetFactory<Ar_ResolverFactory<Resolver> >();
}

}  // namespace pxr

#endif // PXR_AR_DEFINE_RESOLVER_H
