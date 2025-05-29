// Copyright 2021 Pixar
//
// Licensed under the terms set forth in the LICENSE.txt file available at
// https://openusd.org/license.
//
// Modified by Jeremy Retailleau.

#include "./timestamp.h"
#include <pxr/tf/diagnostic.h>

namespace pxr {

void
ArTimestamp::_IssueInvalidGetTimeError() const
{
    TF_CODING_ERROR("Cannot call GetTime on an invalid ArTimestamp");
}

}  // namespace pxr
