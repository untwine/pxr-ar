# Copyright 2018 Pixar
#
# Licensed under the terms set forth in the LICENSE.txt file available at
# https://openusd.org/license.
#
# Modified by Jeremy Retailleau.

import os
import unittest

from pxr import Plug, Ar, Tf


class TestArAdvancedAPI(unittest.TestCase):
    def test_GetAvailableResolvers(self):
        """Tests ArGetAvailableResolvers and ArCreateResolver APIs
        via _TestResolver1 and _TestResolver2 subclasses."""

        # Register test resolver plugin and verify we have the
        # expected ArResolver subclasses.
        pr = Plug.Registry()
        plugins = pr.RegisterPlugins(os.environ.get("AR_ADVANCED_API_PLUGIN"))
        self.assertEqual(len(plugins), 1)

        resolverTypes = pr.GetAllDerivedTypes('pxr::ArResolver')
        self.assertIn(Tf.Type.FindByName('_TestResolver1'), resolverTypes)
        self.assertIn(Tf.Type.FindByName('_TestResolver2'), resolverTypes)

        # Set _TestResolver2 to be the preferred resolver; 
        # otherwise, _TestResolver1 would be initially constructed
        # by the call to Ar.GetResolver() below since its typename
        # comes before _TestResolver2.
        Ar.SetPreferredResolver('_TestResolver2')

        # Invoke Ar.GetResolver(). This will cause _TestResolver1
        # and _TestResolver2 to be created. These classes test
        # ArGetAvailableResolvers and ArCreateResolver internally.        
        resolver = Ar.GetResolver()

if __name__ == '__main__':
    unittest.main()


