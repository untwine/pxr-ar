// Copyright 2016 Pixar
//
// Licensed under the terms set forth in the LICENSE.txt file available at
// https://openusd.org/license.
//
// Modified by Jeremy Retailleau.

#ifndef PXR_AR_RESOLVER_CONTEXT_BINDER_H
#define PXR_AR_RESOLVER_CONTEXT_BINDER_H

/// \file ar/resolverContextBinder.h

#include "./api.h"
#include "./resolverContext.h"
#include <pxr/vt/value.h>

namespace pxr {

class ArResolver;

/// \class ArResolverContextBinder
///
/// Helper object for managing the binding and unbinding of 
/// ArResolverContext objects with the asset resolver.
///
/// Context binding and unbinding are thread-specific. If you bind a context in
/// a thread, that binding will only be visible to that thread.
/// See \ref ar_resolver_contexts "Resolver Contexts" for more details.
///
/// \see \ref ArResolver_context "Asset Resolver Context Operations"
class ArResolverContextBinder
{
public:
    /// Bind the given \p context with the asset resolver.
    ///
    /// Calls ArResolver::BindContext on the configured asset resolver
    /// and saves the bindingData populated by that function.
    AR_API
    ArResolverContextBinder(
        const ArResolverContext& context);

    /// Bind the given \p context to the given \p assetResolver.
    ///
    /// Calls ArResolver::BindContext on the given \p assetResolver
    /// and saves the bindingData populated by that function.
    AR_API
    ArResolverContextBinder(
        ArResolver* assetResolver,
        const ArResolverContext& context);

    /// Unbinds the context specified in the constructor of this
    /// object from the asset resolver.
    ///
    /// Calls ArResolver::UnbindContext on the asset resolver that was
    /// bound to originally, passing the saved bindingData to that function.
    AR_API
    ~ArResolverContextBinder();

private:
    ArResolver* _resolver;
    ArResolverContext _context;
    VtValue _bindingData;
};

}  // namespace pxr

#endif // PXR_AR_RESOLVER_CONTEXT_BINDER_H
