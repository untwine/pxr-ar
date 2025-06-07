# Copyright 2021 Pixar
#
# Licensed under the terms set forth in the LICENSE.txt file available at
# https://openusd.org/license.
#
# Modified by Jeremy Retailleau.

import os

from pxr import Plug, Ar, Tf

class TestArOptionalImplementation:

    def setup_method(self):
        # Register test resolver plugins
        pr = Plug.Registry()
        pr.RegisterPlugins(os.environ.get("AR_OPTIONAL_IMPL_PLUGIN"))

    def test_Resolver(self, capfd):
        Ar.SetPreferredResolver("_TestResolver")
        execute()

        captured = capfd.readouterr()
        assert captured.out == ""
        assert captured.err == ""

    def test_ResolverWithContextMethods(self, capfd):
        Ar.SetPreferredResolver("_TestResolverWithContextMethods")
        execute()

        captured = capfd.readouterr()
        assert captured.out == (
            "_BindContext\n"
            "_UnbindContext\n"
            "_CreateDefaultContext\n"
            "_CreateDefaultContextForAsset\n"
            "_CreateContextFromString\n"
            "_RefreshContext\n"
            "_GetCurrentContext\n"
            "_IsContextDependentPath\n"
        )
        assert captured.err == ""

    def test_DerivedResolverWithContextMethods(self, capfd):
        Ar.SetPreferredResolver("_TestDerivedResolverWithContextMethods")
        execute()

        captured = capfd.readouterr()
        assert captured.out == (
            "_BindContext\n"
            "_UnbindContext\n"
            "_CreateDefaultContext\n"
            "_CreateDefaultContextForAsset\n"
            "_CreateContextFromString\n"
            "_RefreshContext\n"
            "_GetCurrentContext\n"
            "_IsContextDependentPath\n"
        )
        assert captured.err == ""


    def test_ResolverWithCacheMethods(self, capfd):
        Ar.SetPreferredResolver("_TestResolverWithCacheMethods")
        execute()

        captured = capfd.readouterr()
        assert captured.out == (
            "_BeginCacheScope\n"
            "_EndCacheScope\n"
        )
        assert captured.err == ""

    def test_DerivedResolverWithCacheMethod(self, capfd):
        Ar.SetPreferredResolver("_TestDerivedResolverWithCacheMethods")
        execute()

        captured = capfd.readouterr()
        assert captured.out == (
            "_BeginCacheScope\n"
            "_EndCacheScope\n"
        )
        assert captured.err == ""


def execute():
    r = Ar.GetResolver()

    # Call each method to see whether the corresponding virtual method
    # on the resolver is called.

    # This context manager calls ArResolver::BindContext when entered
    # and ArResolver::UnbindContext on exit.
    with Ar.ResolverContextBinder(Ar.ResolverContext()):
        pass

    r.CreateDefaultContext()
    r.CreateDefaultContextForAsset('foo')
    r.CreateContextFromString('foo')
    r.RefreshContext(Ar.ResolverContext())
    r.GetCurrentContext()
    r.IsContextDependentPath('foo')

    # This context manager calls ArResolver::BeginCacheScope when entered
    # and ArResolver::EndCacheScope on exit.
    with Ar.ResolverScopedCache():
        pass
