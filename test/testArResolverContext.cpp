// Copyright 2020 Pixar
//
// Licensed under the terms set forth in the LICENSE.txt file available at
// https://openusd.org/license.
//
// Modified by Jeremy Retailleau.

#include <pxr/ar/ar.h>
#include <pxr/ar/defineResolverContext.h>
#include <pxr/ar/resolverContext.h>
#include <pxr/tf/diagnostic.h>
#include <pxr/tf/hash.h>

#include <string>

using namespace pxr;

template <class Data>
class TestContextObject
{
public:
    TestContextObject(const Data& data)
        : _data(data)
    {
    }

    const Data& GetData() const
    {
        return _data;
    }

    bool operator<(const TestContextObject& rhs) const
    {
        return _data < rhs._data;
    }

    bool operator==(const TestContextObject& rhs) const
    {
        return _data == rhs._data;
    }

    friend size_t hash_value(const TestContextObject& o)
    {
        return TfHash()(o._data);
    }

private:
    Data _data;
};

using TestStringContextObject = TestContextObject<std::string>;
using TestIntContextObject = TestContextObject<int>;

namespace pxr {

AR_DECLARE_RESOLVER_CONTEXT(TestStringContextObject);
AR_DECLARE_RESOLVER_CONTEXT(TestIntContextObject);

}  // namespace pxr

static void
TestDefault()
{
    ArResolverContext ctx;
    TF_AXIOM(ctx.IsEmpty());
    TF_AXIOM(ctx.Get<TestStringContextObject>() == nullptr);
    TF_AXIOM(ctx.Get<TestIntContextObject>() == nullptr);

    ArResolverContext ctx2;
    TF_AXIOM(ctx2.IsEmpty());
    TF_AXIOM(ctx2.Get<TestStringContextObject>() == nullptr);
    TF_AXIOM(ctx.Get<TestIntContextObject>() == nullptr);
    TF_AXIOM(ctx == ctx2);
    TF_AXIOM(!(ctx < ctx2));
    TF_AXIOM(!(ctx2 < ctx));
}

static void
TestSingleContextObject()
{
    // Create an ArResolverContext holding a single context object
    // and verify those objects are accessible.
    TestStringContextObject strContextObj("test string");
    ArResolverContext ctx1(strContextObj);
    TF_AXIOM(!ctx1.IsEmpty());
   
    const TestStringContextObject* strObjFromCtx =
        ctx1.Get<TestStringContextObject>();
    TF_AXIOM(strObjFromCtx);
    TF_AXIOM(strObjFromCtx->GetData() == strContextObj.GetData());

    const TestIntContextObject* intObjFromCtx =
        ctx1.Get<TestIntContextObject>();
    TF_AXIOM(intObjFromCtx == nullptr);

    // Create an ArResolverContext that should compare equal to
    // the original context.
    ArResolverContext ctx2(TestStringContextObject("test string"));
    TF_AXIOM(ctx1 == ctx2);
    TF_AXIOM(!(ctx1 < ctx2));
    TF_AXIOM(!(ctx2 < ctx1));

    // Create ArResolverContexts holding different context objects
    // and verify they do not compare equal.
    ArResolverContext ctx3;
    TF_AXIOM(ctx1 != ctx3);

    ArResolverContext ctx4(TestStringContextObject("foo"));
    TF_AXIOM(ctx1 != ctx4);

    ArResolverContext ctx5(TestIntContextObject(42));
    TF_AXIOM(ctx1 != ctx5);
}

static void
TestMultipleContextObjects()
{
    // Create an ArResolverContext holding multiple context objects
    // and verify those objects are accessible.
    ArResolverContext context(
        TestStringContextObject("test string"),
        TestIntContextObject(42));
    TF_AXIOM(!context.IsEmpty());
    TF_AXIOM(context != ArResolverContext());
    
    const TestStringContextObject* strObjFromContext =
        context.Get<TestStringContextObject>();
    TF_AXIOM(strObjFromContext);
    TF_AXIOM(strObjFromContext->GetData() == "test string");

    const TestIntContextObject* intObjFromContext =
        context.Get<TestIntContextObject>();
    TF_AXIOM(intObjFromContext);
    TF_AXIOM(intObjFromContext->GetData() == 42);

    // Create an ArResolverContext holding the same context objects,
    // but passed in different order. This ArResolverContext should
    // still compare equal to the first context.
    {
        ArResolverContext testContext(
            *intObjFromContext,
            *strObjFromContext);
        TF_AXIOM(context == testContext);
        TF_AXIOM(!(context < testContext));
        TF_AXIOM(!(testContext < context));
        TF_AXIOM(hash_value(context) == hash_value(testContext));
    }

    {
        ArResolverContext testContext(
            *intObjFromContext,
            ArResolverContext(*strObjFromContext));
        TF_AXIOM(context == testContext);
        TF_AXIOM(!(context < testContext));
        TF_AXIOM(!(testContext < context));
        TF_AXIOM(hash_value(context) == hash_value(testContext));
    }

    {
        ArResolverContext testContext(
            ArResolverContext{*intObjFromContext},
            ArResolverContext{*strObjFromContext});
        TF_AXIOM(context == testContext);
        TF_AXIOM(!(context < testContext));
        TF_AXIOM(!(testContext < context));
        TF_AXIOM(hash_value(context) == hash_value(testContext));
    }

    // Create ArResolverContexts holding different context objects
    // and verify they do not compare equal.
    TestIntContextObject intObj(42);
    TestStringContextObject strObj("foo");

    {
        ArResolverContext testContext(intObj);
        TF_AXIOM(context != testContext);
    }

    {
        ArResolverContext testContext(strObj);
        TF_AXIOM(context != testContext);
    }

    {
        ArResolverContext testContext(strObj, intObj);
        TF_AXIOM(context != testContext);
    }

    {
        ArResolverContext testContext(
            ArResolverContext{strObj},
            ArResolverContext{intObj});
        TF_AXIOM(context != testContext);
    }

    {
        ArResolverContext testContext(
            strObj,
            ArResolverContext{intObj});
        TF_AXIOM(context != testContext);
    }
}

int main(int argc, char** argv)
{
    printf("TestDefault ...\n");
    TestDefault();

    printf("TestSingleContextObject ...\n");
    TestSingleContextObject();

    printf("TestMultipleContextObjects ...\n");
    TestMultipleContextObjects();

    printf("All tests passed!\n");
    return 0;
}
