// Copyright 2018 Pixar
//
// Licensed under the terms set forth in the LICENSE.txt file available at
// https://openusd.org/license.
//
// Modified by Jeremy Retailleau.

#ifndef PXR_AR_DEFINE_PACKAGE_RESOLVER_H
#define PXR_AR_DEFINE_PACKAGE_RESOLVER_H

/// \file ar/definePackageResolver.h
/// Macros for defining a package resolver implementation.

#include "./api.h"
#include "./packageResolver.h"

#include <pxr/tf/registryManager.h>
#include <pxr/tf/type.h>

namespace pxr {

/// \def AR_DEFINE_PACKAGE_RESOLVER
///
/// Performs registrations required for the specified package resolver
/// class to be discovered by Ar's plugin mechanism. This typically would be
/// invoked in the source file defining the resolver class. For example:
///
/// \code
/// // in .cpp file
/// AR_DEFINE_PACKAGE_RESOLVER(CustomPackageResolverClass, ArPackageResolver);
/// \endcode
#ifdef doxygen
#define AR_DEFINE_PACKAGE_RESOLVER(PackageResolverClass, BaseClass1, ...)
#else

#define AR_DEFINE_PACKAGE_RESOLVER(...)         \
TF_REGISTRY_FUNCTION(pxr::TfType) {             \
    Ar_DefinePackageResolver<__VA_ARGS__>();    \
}

class Ar_PackageResolverFactoryBase 
    : public TfType::FactoryBase 
{
public:
    AR_API
    virtual ~Ar_PackageResolverFactoryBase();

    AR_API
    virtual ArPackageResolver* New() const = 0;
};

template <class T>
class Ar_PackageResolverFactory 
    : public Ar_PackageResolverFactoryBase 
{
public:
    virtual ArPackageResolver* New() const override
    {
        return new T;
    }
};

template <class PackageResolver, class ...Bases>
void Ar_DefinePackageResolver()
{
    TfType::Define<PackageResolver, TfType::Bases<Bases...>>()
        .template SetFactory<Ar_PackageResolverFactory<PackageResolver>>();
}

#endif // doxygen

}  // namespace pxr

#endif // PXR_AR_DEFINE_PACKAGE_RESOLVER_H
