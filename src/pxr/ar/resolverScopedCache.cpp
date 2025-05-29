// Copyright 2016 Pixar
//
// Licensed under the terms set forth in the LICENSE.txt file available at
// https://openusd.org/license.
//
// Modified by Jeremy Retailleau.

#include "./resolverScopedCache.h"
#include "./resolver.h"

namespace pxr {

ArResolverScopedCache::ArResolverScopedCache()
{
    ArGetResolver().BeginCacheScope(&_cacheScopeData);
}

ArResolverScopedCache::ArResolverScopedCache(const ArResolverScopedCache* parent)
    : _cacheScopeData(parent->_cacheScopeData)
{
    ArGetResolver().BeginCacheScope(&_cacheScopeData);
}

ArResolverScopedCache::~ArResolverScopedCache()
{
    ArGetResolver().EndCacheScope(&_cacheScopeData);
}

}  // namespace pxr
