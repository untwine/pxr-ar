// Copyright 2021 Pixar
//
// Licensed under the terms set forth in the LICENSE.txt file available at
// https://openusd.org/license.
//
// Modified by Jeremy Retailleau.

#include "./asset.h"
#include "./inMemoryAsset.h"

namespace pxr {

ArAsset::ArAsset()
{
}

ArAsset::~ArAsset()
{
}

std::shared_ptr<ArAsset>
ArAsset::GetDetachedAsset() const
{
    return ArInMemoryAsset::FromAsset(*this);
}

}  // namespace pxr
