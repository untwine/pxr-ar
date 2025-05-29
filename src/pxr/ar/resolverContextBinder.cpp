// Copyright 2016 Pixar
//
// Licensed under the terms set forth in the LICENSE.txt file available at
// https://openusd.org/license.
//
// Modified by Jeremy Retailleau.

#include "./resolverContextBinder.h"
#include "./resolver.h"

namespace pxr {

ArResolverContextBinder::ArResolverContextBinder(
    ArResolver* resolver,
    const ArResolverContext& context)
    : _resolver(resolver)
    , _context(context)
{
    if (_resolver) {
        _resolver->BindContext(_context, &_bindingData);
    }
}

ArResolverContextBinder::ArResolverContextBinder(
    const ArResolverContext& context)
    : _resolver(&ArGetResolver())
    , _context(context)
{
    if (_resolver) {
        _resolver->BindContext(_context, &_bindingData);
    }
}

ArResolverContextBinder::~ArResolverContextBinder()
{
    if (_resolver) {
        _resolver->UnbindContext(_context, &_bindingData);
    }
}

}  // namespace pxr
