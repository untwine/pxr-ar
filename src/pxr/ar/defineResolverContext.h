// Copyright 2016 Pixar
//
// Licensed under the terms set forth in the LICENSE.txt file available at
// https://openusd.org/license.
//
// Modified by Jeremy Retailleau.

#ifndef PXR_AR_DEFINE_RESOLVER_CONTEXT_H
#define PXR_AR_DEFINE_RESOLVER_CONTEXT_H

#include "./api.h"
#include "./resolverContext.h"

/// \file ar/defineResolverContext.h
/// Macros for defining an object for use with ArResolverContext

namespace pxr {

/// \def AR_DECLARE_RESOLVER_CONTEXT
///
/// Declare that the specified ContextObject type may be used as an asset
/// resolver context object for ArResolverContext. This typically
/// would be invoked in the header where the ContextObject is
/// declared.
///
#ifdef doxygen
#define AR_DECLARE_RESOLVER_CONTEXT(ContextObject)
#else
#define AR_DECLARE_RESOLVER_CONTEXT(context)           \
template <>                                            \
struct ArIsContextObject<context>                      \
{                                                      \
    static const bool value = true;                    \
}
#endif

}  // namespace pxr

#endif // PXR_AR_DEFINE_RESOLVER_CONTEXT_H
