// Copyright 2018 Pixar
//
// Licensed under the terms set forth in the LICENSE.txt file available at
// https://openusd.org/license.
//
// Modified by Jeremy Retailleau.

#include "./packageResolver.h"

#include <pxr/tf/registryManager.h>
#include <pxr/tf/type.h>

namespace pxr {

TF_REGISTRY_FUNCTION(pxr::TfType)
{
    TfType::Define<ArPackageResolver>();
}

ArPackageResolver::ArPackageResolver()
{
}

ArPackageResolver::~ArPackageResolver()
{
}

}  // namespace pxr
