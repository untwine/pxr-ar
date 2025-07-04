// Copyright 2016 Pixar
//
// Licensed under the terms set forth in the LICENSE.txt file available at
// https://openusd.org/license.
//
// Modified by Jeremy Retailleau.

#include "./defaultResolver.h"

#include "./assetInfo.h"
#include "./defaultResolverContext.h"
#include "./defineResolver.h"
#include "./filesystemAsset.h"
#include "./filesystemWritableAsset.h"
#include "./notice.h"
#include "./resolverContext.h"
#include "./writableAsset.h"

#include <pxr/arch/fileSystem.h>
#include <pxr/arch/systemInfo.h>
#include <pxr/tf/errorMark.h>
#include <pxr/tf/fileUtils.h>
#include <pxr/tf/getenv.h>
#include <pxr/tf/pathUtils.h>
#include <pxr/tf/safeOutputFile.h>
#include <pxr/tf/staticData.h>
#include <pxr/tf/stringUtils.h>
#include <pxr/vt/value.h>

namespace pxr {

AR_DEFINE_RESOLVER(ArDefaultResolver, ArResolver);

static bool
_IsFileRelative(const std::string& path) {
    return path.find("./") == 0 || path.find("../") == 0;
}

static bool
_IsRelativePath(const std::string& path)
{
    return (!path.empty() && TfIsRelativePath(path));
}

static bool
_IsSearchPath(const std::string& path)
{
    return _IsRelativePath(path) && !_IsFileRelative(path);
}

static std::string
_AnchorRelativePath(
    const std::string& anchorPath, 
    const std::string& path)
{
    if (TfIsRelativePath(anchorPath) ||
        !_IsRelativePath(path)) {
        return path;
    }

    // Ensure we are using forward slashes and not back slashes.
    std::string forwardPath = anchorPath;
    std::replace(forwardPath.begin(), forwardPath.end(), '\\', '/');

    // If anchorPath does not end with a '/', we assume it is specifying
    // a file, strip off the last component, and anchor the path to that
    // directory.
    const std::string anchoredPath = TfStringCatPaths(
        TfStringGetBeforeSuffix(forwardPath, '/'), path);
    return TfNormPath(anchoredPath);
}


static std::vector<std::string>
_ParseSearchPaths(const std::string& pathStr)
{
    return TfStringTokenize(pathStr, ARCH_PATH_LIST_SEP);
}

struct _ArDefaultResolverFallbackContext {
    _ArDefaultResolverFallbackContext() {
        const std::string envPath = TfGetenv("PXR_AR_DEFAULT_SEARCH_PATH");
        if (!envPath.empty()) {
            context = ArDefaultResolverContext(_ParseSearchPaths(envPath));
        }
    }

    ArDefaultResolverContext context;
};

static TfStaticData<_ArDefaultResolverFallbackContext> _DefaultPath;

void
ArDefaultResolver::SetDefaultSearchPath(
    const std::vector<std::string>& searchPath)
{
    ArDefaultResolverContext newFallback = ArDefaultResolverContext(searchPath);

    if (newFallback == _DefaultPath->context) {
        return;
    }

    _DefaultPath->context = std::move(newFallback);

    ArNotice::ResolverChanged([](const ArResolverContext& ctx){
        return ctx.Get<ArDefaultResolverContext>() != nullptr;
    }).Send();
}

std::string
ArDefaultResolver::_CreateIdentifier(
    const std::string& assetPath,
    const ArResolvedPath& anchorAssetPath) const
{
    if (assetPath.empty()) {
        return assetPath;
    }

    if (!anchorAssetPath) {
        return TfNormPath(assetPath);
    }

    // If assetPath is a search path, we want to use that as the asset's
    // identifier instead of turning it into an absolute path by anchoring
    // it. This ensures that resolving that identifier will always invoke
    // the search path mechanism, so that assets that are added/removed
    // from directories in the search path can be discovered by re-resolving
    // the identifier.
    //
    // This is determined using the look-here-first scheme. For any relative
    // path, we first look relative to the specified anchor. If an asset 
    // exists there, we just return the anchored path. Otherwise, we return
    // the asset path as-is.
    const std::string anchoredAssetPath =
        _AnchorRelativePath(anchorAssetPath, assetPath);

    if (_IsSearchPath(assetPath) && Resolve(anchoredAssetPath).empty()) {
        return TfNormPath(assetPath);
    }

    return TfNormPath(anchoredAssetPath);
}

std::string
ArDefaultResolver::_CreateIdentifierForNewAsset(
    const std::string& assetPath,
    const ArResolvedPath& anchorAssetPath) const
{
    if (assetPath.empty()) {
        return assetPath;
    }

    if (_IsRelativePath(assetPath)) {
        return TfNormPath(anchorAssetPath ? 
            _AnchorRelativePath(anchorAssetPath, assetPath) :
            TfAbsPath(assetPath));
    }

    return TfNormPath(assetPath);
}

static ArResolvedPath
_ResolveAnchored(
    const std::string& anchorPath,
    const std::string& path)
{
    std::string resolvedPath = path;
    if (!anchorPath.empty()) {
        // XXX - CLEANUP:
        // It's tempting to use AnchorRelativePath to combine the two
        // paths here, but that function's file-relative anchoring
        // causes consumers to break. 
        // 
        // Ultimately what we should do is specify whether anchorPath 
        // in both Resolve and _AnchorRelativePath can be files or directories 
        // and fix up all the callers to accommodate this.
        resolvedPath = TfStringCatPaths(anchorPath, path);
    }

    // Use TfAbsPath to ensure we return an absolute path using the
    // platform-specific representation (e.g. '\' as path separators
    // on Windows.
    return TfPathExists(resolvedPath) ?
        ArResolvedPath(TfAbsPath(resolvedPath)) : ArResolvedPath();
}

ArResolvedPath
ArDefaultResolver::_Resolve(const std::string& path) const
{
    if (path.empty()) {
        return ArResolvedPath();
    }

    if (_IsRelativePath(path)) {
        // First try to resolve relative paths against the current
        // working directory.
        ArResolvedPath resolvedPath = _ResolveAnchored(ArchGetCwd(), path);
        if (resolvedPath) {
            return resolvedPath;
        }

        // If that fails and the path is a search path, try to resolve
        // against each directory in the specified search paths.
        if (_IsSearchPath(path)) {
            const ArDefaultResolverContext* contexts[2] =
                {_GetCurrentContextPtr(), &_DefaultPath->context};
            for (const ArDefaultResolverContext* ctx : contexts) {
                if (ctx) {
                    for (const auto& searchPath : ctx->GetSearchPath()) {
                        resolvedPath = _ResolveAnchored(searchPath, path);
                        if (resolvedPath) {
                            return resolvedPath;
                        }
                    }
                }
            }
        }

        return ArResolvedPath();
    }

    return _ResolveAnchored(std::string(), path);
}

ArResolvedPath
ArDefaultResolver::_ResolveForNewAsset(
    const std::string& assetPath) const
{
    return ArResolvedPath(assetPath.empty() ? assetPath : TfAbsPath(assetPath));
}

ArTimestamp
ArDefaultResolver::_GetModificationTimestamp(
    const std::string& path,
    const ArResolvedPath& resolvedPath) const
{
    return ArFilesystemAsset::GetModificationTimestamp(resolvedPath);
}

std::shared_ptr<ArAsset> 
ArDefaultResolver::_OpenAsset(
    const ArResolvedPath& resolvedPath) const
{
    return ArFilesystemAsset::Open(resolvedPath);
}

std::shared_ptr<ArWritableAsset>
ArDefaultResolver::_OpenAssetForWrite(
    const ArResolvedPath& resolvedPath,
    WriteMode writeMode) const
{
    return ArFilesystemWritableAsset::Create(resolvedPath, writeMode);
}

bool
ArDefaultResolver::_IsContextDependentPath(
    const std::string& assetPath) const
{
    return _IsSearchPath(assetPath);
}

ArResolverContext 
ArDefaultResolver::_CreateDefaultContext() const
{
    return _defaultContext;
}

ArResolverContext
ArDefaultResolver::_CreateContextFromString(
    const std::string& contextStr) const
{
    return ArDefaultResolverContext(_ParseSearchPaths(contextStr));
}

ArResolverContext 
ArDefaultResolver::_CreateDefaultContextForAsset(
    const std::string& assetPath) const
{
    if (assetPath.empty()){
        return ArResolverContext(ArDefaultResolverContext());
    }

    std::string assetDir = TfGetPathName(TfAbsPath(assetPath));
    
    return ArResolverContext(ArDefaultResolverContext(
                                 std::vector<std::string>(1, assetDir)));
}

const ArDefaultResolverContext* 
ArDefaultResolver::_GetCurrentContextPtr() const
{
    return _GetCurrentContextObject<ArDefaultResolverContext>();
}

}  // namespace pxr
