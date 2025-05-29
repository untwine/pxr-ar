// Copyright 2016 Pixar
//
// Licensed under the terms set forth in the LICENSE.txt file available at
// https://openusd.org/license.
//
// Modified by Jeremy Retailleau.

#include "./debugCodes.h"

#include <pxr/tf/registryManager.h>

namespace pxr {

TF_REGISTRY_FUNCTION(pxr::TfDebug)
{
    TF_DEBUG_ENVIRONMENT_SYMBOL(
        AR_RESOLVER_INIT, 
        "Print debug output during asset resolver initialization");
}

}  // namespace pxr
