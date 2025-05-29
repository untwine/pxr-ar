//
// Copyright 2020 Pixar
//
// Licensed under the terms set forth in the LICENSE.txt file available at
// https://openusd.org/license.
//
#include <pxr/ar/pxr.h>

#include <pxr/ar/defineResolverContext.h>
#include <pxr/tf/hash.h>
#include <string>

AR_NAMESPACE_OPEN_SCOPE

class _TestURIResolverContext
{
public:
    _TestURIResolverContext() = default;
    _TestURIResolverContext(const _TestURIResolverContext&) = default;
    explicit _TestURIResolverContext(const std::string& s)
        : data(s)
    { }

    bool operator<(const _TestURIResolverContext& rhs) const
    { return data < rhs.data; } 

    bool operator==(const _TestURIResolverContext& rhs) const
    { return data == rhs.data; } 

    std::string data;
};

size_t 
hash_value(const _TestURIResolverContext& rhs)
{
    return TfHash()(rhs.data);
}

AR_DECLARE_RESOLVER_CONTEXT(_TestURIResolverContext);

AR_NAMESPACE_CLOSE_SCOPE
