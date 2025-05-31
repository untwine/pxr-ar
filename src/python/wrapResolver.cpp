// Copyright 2020 Pixar
//
// Licensed under the terms set forth in the LICENSE.txt file available at
// https://openusd.org/license.
//
// Modified by Jeremy Retailleau.

#include <pxr/boost/python/class.hpp>
#include <pxr/boost/python/def.hpp>
#include <pxr/boost/python/reference_existing_object.hpp>
#include <pxr/boost/python/return_value_policy.hpp>
#include <pxr/boost/python/tuple.hpp>

#include <pxr/ar/assetInfo.h>
#include <pxr/ar/resolver.h>
#include <pxr/ar/resolverContext.h>
#include <pxr/tf/pyAnnotatedBoolResult.h>
#include <pxr/tf/pyResultConversions.h>
#include <pxr/tf/refPtr.h>


using namespace pxr;

using namespace pxr::boost::python;

class Ar_PyAnnotatedBoolResult
    : public TfPyAnnotatedBoolResult<std::string>
{
public:
    Ar_PyAnnotatedBoolResult(bool val, const std::string& annotation)
        : TfPyAnnotatedBoolResult<std::string>(val, annotation)
    {
    }
};

static 
Ar_PyAnnotatedBoolResult
_CanWriteAssetToPath(
    ArResolver& resolver, const ArResolvedPath& resolvedPath)
{
    std::string whyNot;
    const bool rval = resolver.CanWriteAssetToPath(resolvedPath, &whyNot);
    return Ar_PyAnnotatedBoolResult(rval, whyNot);
}

void
wrapResolver()
{
    Ar_PyAnnotatedBoolResult::Wrap<Ar_PyAnnotatedBoolResult>
        ("_PyAnnotatedBoolResult", "whyNot");

    typedef ArResolver This;

    class_<This, noncopyable>
        ("Resolver", no_init)

        .def("CreateDefaultContext", &This::CreateDefaultContext)
        .def("CreateDefaultContextForAsset", 
             &This::CreateDefaultContextForAsset,
             args("assetPath"))

        .def("CreateContextFromString", 
             (ArResolverContext (This::*)(const std::string&) const)
                 &This::CreateContextFromString,
             args("contextStr"))

        .def("CreateContextFromString", 
             (ArResolverContext (This::*)
                 (const std::string&, const std::string&) const)
                 &This::CreateContextFromString,
             (arg("uriScheme"), arg("contextStr")))

        .def("CreateContextFromStrings", &This::CreateContextFromStrings,
             args("contextStrs"))

        .def("GetCurrentContext", &This::GetCurrentContext)

        .def("IsContextDependentPath", &This::IsContextDependentPath,
             args("assetPath"))

        .def("CreateIdentifier", &This::CreateIdentifier,
             (args("assetPath"), 
              args("anchorAssetPath") = ArResolvedPath()))
        .def("CreateIdentifierForNewAsset", &This::CreateIdentifierForNewAsset,
             (args("assetPath"), 
              args("anchorAssetPath") = ArResolvedPath()))

        .def("Resolve", &This::Resolve,
             (args("assetPath")))
        .def("ResolveForNewAsset", &This::ResolveForNewAsset,
             (args("assetPath")))

        .def("GetAssetInfo", &This::GetAssetInfo,
             (args("assetPath"), args("resolvedPath")))
        .def("GetModificationTimestamp", &This::GetModificationTimestamp,
             (args("assetPath"), args("resolvedPath")))
        .def("OpenAsset", &This::OpenAsset,
             (args("resolvedPath")))
        .def("GetExtension", &This::GetExtension,
             args("assetPath"))

        .def("CanWriteAssetToPath", &_CanWriteAssetToPath,
             args("resolvedPath"))

        .def("RefreshContext", &This::RefreshContext)
        ;

    def("GetResolver", ArGetResolver,
        return_value_policy<reference_existing_object>());

    def("GetRegisteredURISchemes", ArGetRegisteredURISchemes,
        return_value_policy<TfPySequenceToList>());

    def("SetPreferredResolver", ArSetPreferredResolver,
        arg("resolverTypeName"));

    def("GetUnderlyingResolver", ArGetUnderlyingResolver,
        return_value_policy<reference_existing_object>());
}
