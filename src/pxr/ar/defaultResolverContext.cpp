// Copyright 2018 Pixar
//
// Licensed under the terms set forth in the LICENSE.txt file available at
// https://openusd.org/license.
//
// Modified by Jeremy Retailleau.

#include "./defaultResolverContext.h"

#include <pxr/tf/diagnostic.h>
#include <pxr/tf/hash.h>
#include <pxr/tf/ostreamMethods.h>
#include <pxr/tf/pathUtils.h>
#include <pxr/tf/stringUtils.h>

namespace pxr {

ArDefaultResolverContext::ArDefaultResolverContext(
    const std::vector<std::string>& searchPath)
{
    _searchPath.reserve(searchPath.size());
    for (const std::string& p : searchPath) {
        if (p.empty()) {
            continue;
        }

        const std::string absPath = TfAbsPath(p);
        if (absPath.empty()) {
            TF_WARN(
                "Could not determine absolute path for search path prefix "
                "'%s'", p.c_str());
            continue;
        }

        _searchPath.push_back(absPath);
    }
}

bool
ArDefaultResolverContext::operator<(const ArDefaultResolverContext& rhs) const
{
    return _searchPath < rhs._searchPath;
}

bool 
ArDefaultResolverContext::operator==(const ArDefaultResolverContext& rhs) const
{
    return _searchPath == rhs._searchPath;
}

bool 
ArDefaultResolverContext::operator!=(const ArDefaultResolverContext& rhs) const
{
    return !(*this == rhs);
}

std::string 
ArDefaultResolverContext::GetAsString() const
{
    std::string result = "Search path: ";
    if (_searchPath.empty()) {
        result += "[ ]";
    }
    else {
        result += "[\n    ";
        result += TfStringJoin(_searchPath, "\n    ");
        result += "\n]";
    }
    return result;
}

size_t 
hash_value(const ArDefaultResolverContext& context)
{
    return TfHash()(context.GetSearchPath());
}

}  // namespace pxr
