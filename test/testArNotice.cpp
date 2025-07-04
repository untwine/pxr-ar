// Copyright 2021 Pixar
//
// Licensed under the terms set forth in the LICENSE.txt file available at
// https://openusd.org/license.
//
// Modified by Jeremy Retailleau.

#include <pxr/ar/defineResolverContext.h>
#include <pxr/ar/notice.h>
#include <pxr/ar/resolverContext.h>

#include <cstdio>

using namespace pxr;

template <class DataType>
class TestResolverContext
{
public:
    TestResolverContext() = default;
    TestResolverContext(const TestResolverContext&) = default;
    explicit TestResolverContext(const DataType& d)
        : data(d)
    { }

    bool operator<(const TestResolverContext& rhs) const
    { return data < rhs.data; } 

    bool operator==(const TestResolverContext& rhs) const
    { return data == rhs.data; } 

    DataType data;
};

template <class DataType>
size_t
hash_value(const TestResolverContext<DataType>& c)
{
    return TfHash()(c.data);
}

using IntContext = TestResolverContext<int>;
using StringContext = TestResolverContext<std::string>;

namespace pxr {
AR_DECLARE_RESOLVER_CONTEXT(IntContext);
AR_DECLARE_RESOLVER_CONTEXT(StringContext);
}  // namespace pxr

static void
TestResolverChangedNotice()
{
    ArNotice::ResolverChanged affectsAllNotice;
    TF_AXIOM(affectsAllNotice.AffectsContext(
            ArResolverContext()));
    TF_AXIOM(affectsAllNotice.AffectsContext(
            ArResolverContext(IntContext(0), StringContext("s"))));

    ArNotice::ResolverChanged affectsContext(IntContext(0));
    TF_AXIOM(!affectsContext.AffectsContext(
            ArResolverContext()));
    TF_AXIOM(!affectsContext.AffectsContext(
            ArResolverContext(IntContext(1))));
    TF_AXIOM(affectsContext.AffectsContext(
            ArResolverContext(IntContext(0))));
    TF_AXIOM(affectsContext.AffectsContext(
            ArResolverContext(IntContext(0), StringContext("s"))));

    ArNotice::ResolverChanged affectsFn(
        [](const ArResolverContext& ctx) {
            const StringContext* stringCtx = ctx.Get<StringContext>();
            return stringCtx && 
                stringCtx->data.find("needle") != std::string::npos;
        });
    TF_AXIOM(!affectsFn.AffectsContext(
            ArResolverContext()));
    TF_AXIOM(!affectsFn.AffectsContext(
            ArResolverContext(IntContext(0))));
    TF_AXIOM(!affectsFn.AffectsContext(
            ArResolverContext(IntContext(0), StringContext("s"))));
    TF_AXIOM(affectsFn.AffectsContext(
            ArResolverContext(StringContext("test-needle"))));
    TF_AXIOM(affectsFn.AffectsContext(
            ArResolverContext(IntContext(0), StringContext("test-needle"))));
}

int main(int argc, char** argv)
{
    TestResolverChangedNotice();
    printf("PASSED!\n");
    return 0;
}
