// Copyright 2020 Pixar
//
// Licensed under the terms set forth in the LICENSE.txt file available at
// https://openusd.org/license.
//
// Modified by Jeremy Retailleau.

#include <pxr/ar/definePackageResolver.h>
#include <pxr/ar/packageResolver.h>

#include <pxr/tf/diagnosticLite.h>
#include <pxr/tf/stringUtils.h>

using namespace pxr;

// Test package resolver that handles packages of the form
// "foo.package[...]"
class _TestPackageResolver
    : public ArPackageResolver
{
public:
    virtual std::string Resolve(
        const std::string& resolvedPackagePath,
        const std::string& packagedPath) override
    {
        TF_AXIOM(TfStringEndsWith(resolvedPackagePath, ".package"));
        return packagedPath;
    }

    virtual std::shared_ptr<ArAsset> OpenAsset(
        const std::string& resolvedPackagePath,
        const std::string& resolvedPackagedPath) override
    {
        return nullptr;
    }

    virtual void BeginCacheScope(
        VtValue* cacheScopeData) override
    {
    }

    virtual void EndCacheScope(
        VtValue* cacheScopeData) override
    {
    }

};

AR_DEFINE_PACKAGE_RESOLVER(_TestPackageResolver, ArPackageResolver);
