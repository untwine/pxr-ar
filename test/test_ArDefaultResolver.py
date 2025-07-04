# Copyright 2016 Pixar
#
# Licensed under the terms set forth in the LICENSE.txt file available at
# https://openusd.org/license.
#
# Modified by Jeremy Retailleau.

import os
import pathlib
import sys
from pxr import Ar, Tf

import unittest
import shutil

class TestArDefaultResolver(unittest.TestCase):

    def assertPathsEqual(self, path1, path2):
        # XXX: Explicit conversion to str to accommodate change in
        # return type to Ar.ResolvedPath in Ar 2.0.
        path1 = str(path1)
        path2 = str(path2)

        # Flip backslashes to forward slashes to accommodate platform
        # differences. 
        self.assertEqual(os.path.normpath(path1), os.path.normpath(path2))

    def _CreateEmptyTestFile(self, path):
        dir = os.path.dirname(path)
        if not os.path.exists(dir):
            os.makedirs(dir)
        pathlib.Path(path).touch()
        return os.path.abspath(path)

    @classmethod
    def setUpClass(cls):
        # configure the environment variable for testing the initial state of
        # the default search path
        os.environ["PXR_AR_DEFAULT_SEARCH_PATH"] = "env"

        # Force Ar to use the default resolver implementation.
        Ar.SetPreferredResolver('ArDefaultResolver')

        # Verify that the underlying resolver is an Ar.DefaultResolver.
        assert(isinstance(Ar.GetUnderlyingResolver(), Ar.DefaultResolver))

    # Note: In order for this test to work correctly, it needs to execute before
    # any other test which calls Ar.DefaultResolver.SetDefaultSearchPath.
    # The reason for this is because after calling SetDefaultSearchPath, the
    # search path specified in PXR_AR_DEFAULT_SEARCH_PATH will no longer be
    # taken into account when computing all search directories.
    def test_EnvironmentSearchPath(self):
        """Tests that the default resolver initially contains the search path
        from PXR_AR_DEFAULT_SEARCH_PATH and subsequent calls to 
        SetDefaultSearchPath override it"""

        fileC = self._CreateEmptyTestFile("env/envFile.txt")
        resolvedC = Ar.GetResolver().Resolve("envFile.txt")
        self.assertPathsEqual(resolvedC, fileC)

        Ar.DefaultResolver.SetDefaultSearchPath(["env2"])
        resolvedC = Ar.GetResolver().Resolve("envFile.txt")
        self.assertFalse(resolvedC)

    def test_CreateIdentifier(self):
        r = Ar.GetResolver()

        def _RP(path = None):
            return Ar.ResolvedPath(os.path.abspath(path or ""))

        self.assertEqual('', r.CreateIdentifier(''))
        self.assertEqual('', r.CreateIdentifier('', _RP()))
        self.assertEqual('', r.CreateIdentifier('', _RP('AnchorAsset.txt')))

        # The identifier for an absolute path will always be that absolute
        # path normalized.
        self.assertPathsEqual(
            '/dir/AbsolutePath.txt',
            r.CreateIdentifier('/dir/AbsolutePath.txt'))

        self.assertPathsEqual(
            '/dir/AbsolutePath.txt',
            r.CreateIdentifier('/dir/AbsolutePath.txt', _RP('subdir/A.txt')))

        self.assertPathsEqual(
            '/dir/AbsolutePath.txt',
            r.CreateIdentifier('/dir/.//AbsolutePath.txt', _RP('subdir/A.txt')))
        
        # Windows is case insensitive so ensuring that the drive letter of the 
        # resulting identifier matches that of the original input path.
        if sys.platform == "win32":
            self.assertPathsEqual(
                'C:\dir\AbsolutePath.txt',
                r.CreateIdentifier('C:/dir/AbsolutePath.txt'))
            self.assertPathsEqual(
                'c:\dir\AbsolutePath.txt',
                r.CreateIdentifier('c:/dir/AbsolutePath.txt'))

        # The identifier for a file-relative path (i.e. a relative path
        # starting with "./" or "../" is obtained by anchoring that path
        # to the given anchor, or the normalized file-relative path if no
        # anchor is given.
        self.assertPathsEqual(
            'subdir/FileRelative.txt',
            r.CreateIdentifier('./subdir/FileRelative.txt'))

        self.assertPathsEqual(
            os.path.abspath('dir/subdir/FileRelative.txt'),
            r.CreateIdentifier('./subdir/FileRelative.txt', _RP('dir/A.txt')))

        # Test look-here-first behavior for search-relative paths (i.e., 
        # relative paths that do not start with "./" or "../")
        #
        # If an asset exists at the location obtained by anchoring the 
        # relative path to the given anchor, the anchored path is used as
        # the identifier.
        if not os.path.isdir('dir/subdir'):
            os.makedirs('dir/subdir')
        with open('dir/subdir/Exists.txt', 'w') as f:
            pass
        
        self.assertPathsEqual(
            os.path.abspath('dir/subdir/Exists.txt'),
            r.CreateIdentifier('subdir/Exists.txt', _RP('dir/Anchor.txt')))

        # Otherwise, the search path is used as the identifier.
        self.assertPathsEqual(
            'subdir/Bogus.txt',
            r.CreateIdentifier('subdir/Bogus.txt', _RP('dir/Anchor.txt')))

    def test_CreateIdentifierForNewAsset(self):
        r = Ar.GetResolver()

        def _RP(path = None):
            return Ar.ResolvedPath(os.path.abspath(path or ""))

        self.assertEqual(
            '', r.CreateIdentifierForNewAsset(''))
        self.assertEqual(
            '', r.CreateIdentifierForNewAsset('', _RP()))
        self.assertEqual(
            '', r.CreateIdentifierForNewAsset('', _RP('AnchorAsset.txt')))

        # The identifier for an absolute path will always be that absolute
        # path normalized.
        self.assertPathsEqual(
            '/dir/AbsolutePath.txt',
            r.CreateIdentifierForNewAsset('/dir/AbsolutePath.txt'))

        self.assertPathsEqual(
            '/dir/AbsolutePath.txt',
            r.CreateIdentifierForNewAsset(
                '/dir/AbsolutePath.txt', _RP('subdir/A.txt')))

        self.assertPathsEqual(
            '/dir/AbsolutePath.txt',
            r.CreateIdentifierForNewAsset(
                '/dir/.//AbsolutePath.txt', _RP('subdir/A.txt')))
        
        # Windows is case insensitive so ensuring that the drive letter of the 
        # resulting identifier matches that of the original input path.
        if sys.platform == "win32":
            self.assertPathsEqual(
                'C:\dir\AbsolutePath.txt',
                r.CreateIdentifier('C:/dir/AbsolutePath.txt'))
            self.assertPathsEqual(
                'c:\dir\AbsolutePath.txt',
                r.CreateIdentifier('c:/dir/AbsolutePath.txt'))

        # The identifier for a relative path (file-relative or search-relative)
        # will always be the anchored absolute path.
        self.assertPathsEqual(
            os.path.abspath('subdir/FileRelative.txt'),
            r.CreateIdentifierForNewAsset(
                './subdir/FileRelative.txt'))

        self.assertPathsEqual(
            os.path.abspath('dir/subdir/FileRelative.txt'),
            r.CreateIdentifierForNewAsset(
                './subdir/FileRelative.txt', _RP('dir/Anchor.txt')))

        self.assertPathsEqual(
            os.path.abspath('subdir/SearchRel.txt'),
            r.CreateIdentifierForNewAsset(
                'subdir/SearchRel.txt'))

        self.assertPathsEqual(
            os.path.abspath('dir/subdir/SearchRel.txt'),
            r.CreateIdentifierForNewAsset(
                'subdir/SearchRel.txt', _RP('dir/Anchor.txt')))

    def test_Resolve(self):
        testFileName = 'test_Resolve.txt'
        testFilePath = os.path.abspath(testFileName)
        with open(testFilePath, 'w') as ofp:
            print('Garbage', file=ofp)
        
        # XXX: Explicit conversion to str to accommodate change in
        # return type to Ar.ResolvedPath in Ar 2.0.
        resolvedPath = str(Ar.GetResolver().Resolve(testFileName))

        # The resolved path should be absolute.
        self.assertTrue(os.path.isabs(resolvedPath))
        self.assertPathsEqual(testFilePath, resolvedPath)

    def test_ResolveSearchPaths(self):
        testDir = os.path.abspath('test1/test2')
        if os.path.isdir(testDir):
            shutil.rmtree(testDir)
        os.makedirs(testDir)

        testFileName = 'test_ResolveWithContext.txt'
        testFilePath = os.path.join(testDir, testFileName) 
        with open(testFilePath, 'w') as ofp:
            print('Garbage', file=ofp)

        Ar.DefaultResolver.SetDefaultSearchPath([
            os.path.abspath('test1'),
            os.path.abspath('test1/test2')
        ])
        
        resolver = Ar.GetResolver()

        self.assertPathsEqual(
            os.path.abspath('test1/test2/test_ResolveWithContext.txt'),
            resolver.Resolve('test2/test_ResolveWithContext.txt'))

        self.assertPathsEqual(
            os.path.abspath('test1/test2/test_ResolveWithContext.txt'),
            resolver.Resolve('test_ResolveWithContext.txt'))

    def test_ResolveWithCache(self):
        testDir = os.path.abspath('testResolveWithCache/sub')
        if os.path.isdir(testDir):
            shutil.rmtree(testDir)
        os.makedirs(testDir)

        with open('testResolveWithCache/test.txt', 'w') as ofp:
            print('Test 1', file=ofp)

        with open('testResolveWithCache/sub/test.txt', 'w') as ofp:
            print('Test 2', file=ofp)
            
        resolver = Ar.GetResolver()

        # Set up a context that will search in the test root directory
        # first, then the subdirectory.
        context = Ar.DefaultResolverContext([
            os.path.abspath('testResolveWithCache'),
            os.path.abspath('testResolveWithCache/sub')])

        with Ar.ResolverContextBinder(context):
            with Ar.ResolverScopedCache():
                # Resolve should initially find the file in the test root
                # directory.
                self.assertPathsEqual(
                    os.path.abspath('testResolveWithCache/test.txt'),
                    resolver.Resolve('test.txt'))

                os.remove('testResolveWithCache/test.txt')

                # After removing the file from the test root directory,
                # Calling Resolve again will still return the same result
                # as before since a scoped cache is active.
                self.assertPathsEqual(
                    os.path.abspath('testResolveWithCache/test.txt'),
                    resolver.Resolve('test.txt'))

            # Once the caching scope is closed, Resolve should now return
            # the file from the subdirectory.
            self.assertPathsEqual(
                os.path.abspath('testResolveWithCache/sub/test.txt'),
                resolver.Resolve('test.txt'))

    def test_ResolveWithContext(self):
        testDir = os.path.abspath('test3/test4')
        if os.path.isdir(testDir):
            shutil.rmtree(testDir)
        os.makedirs(testDir)
        
        testFileName = 'test_ResolveWithContext.txt'
        testFilePath = os.path.join(testDir, testFileName) 
        with open(testFilePath, 'w') as ofp:
            print('Garbage', file=ofp)
        
        resolver = Ar.GetResolver()
        context = Ar.DefaultResolverContext([
            os.path.abspath('test3'),
            os.path.abspath('test3/test4')
        ])

        self.assertPathsEqual(
            '', 
            resolver.Resolve('test4/test_ResolveWithContext.txt'))

        with Ar.ResolverContextBinder(context):
            self.assertPathsEqual(
                os.path.abspath('test3/test4/test_ResolveWithContext.txt'),
                resolver.Resolve('test4/test_ResolveWithContext.txt'))
            self.assertPathsEqual(
                os.path.abspath('test3/test4/test_ResolveWithContext.txt'),
                resolver.Resolve('test_ResolveWithContext.txt'))

        self.assertPathsEqual(
            '', 
            resolver.Resolve('test4/test_ResolveWithContext.txt'))

    def test_ResolveWithDefaultAssetContext(self):
        assetFileName = 'test_Asset.txt'
        assetFilePath = os.path.abspath(assetFileName)
        with open(assetFilePath, 'w') as ofp:
            print('Garbage', file=ofp)

        testFileName = 'test_SiblingOfAsset.txt'
        testFilePath = os.path.abspath(testFileName)
        with open(testFilePath, 'w') as ofp:
            print('Garbage', file=ofp)
        
        # We use the non-absolute assetFileName to test the
        # cwd-anchoring behavior of CreateDefaultContextForAsset()
        context = Ar.GetResolver().CreateDefaultContextForAsset(assetFileName)
        with Ar.ResolverContextBinder(context):
            resolvedPath = Ar.GetResolver().Resolve(testFileName)

        self.assertPathsEqual(resolvedPath, testFilePath)

        # Make sure we get the same behavior using ConfigureResolverForAsset()
        if hasattr(Ar.Resolver, "ConfigureResolverForAsset"):
            Ar.GetResolver().ConfigureResolverForAsset(assetFileName)
            with Ar.ResolverContextBinder(Ar.GetResolver().CreateDefaultContext()):
                defaultResolvedPath = Ar.GetResolver().Resolve(testFileName)

            self.assertPathsEqual(defaultResolvedPath, testFilePath)

    def test_ResolverContext(self):
        emptyContext = Ar.DefaultResolverContext()
        self.assertEqual(emptyContext.GetSearchPath(), [])
        self.assertEqual(emptyContext, Ar.DefaultResolverContext())
        self.assertEqual(eval(repr(emptyContext)), emptyContext)

        context = Ar.DefaultResolverContext(["/test/path/1", "/test/path/2"])
        self.assertEqual(context.GetSearchPath(),
                         [os.path.abspath("/test/path/1"), 
                          os.path.abspath("/test/path/2")])
        self.assertEqual(context,
                         Ar.DefaultResolverContext(["/test/path/1", 
                                                    "/test/path/2"]))
        self.assertEqual(eval(repr(context)), context)

        self.assertNotEqual(emptyContext, context)

    def test_ResolverRefreshSearchPaths(self):
        fileA = self._CreateEmptyTestFile("dirA/file.txt")
        fileB = self._CreateEmptyTestFile("dirB/file.txt")

        Ar.DefaultResolver.SetDefaultSearchPath(["dirA"])
        resolvedPath = Ar.GetResolver().Resolve("file.txt")
        self.assertPathsEqual(resolvedPath, fileA)

        Ar.DefaultResolver.SetDefaultSearchPath(["dirB"])
        resolvedPath = Ar.GetResolver().Resolve("file.txt")
        self.assertPathsEqual(resolvedPath, fileB)

    def test_ResolverContextHash(self):
        self.assertEqual(
            hash(Ar.DefaultResolverContext()),
            hash(Ar.DefaultResolverContext())
        )

        paths = ["/path1", "/path2", "/path3", "/path4"]
        self.assertEqual(
            hash(Ar.DefaultResolverContext(paths)),
            hash(Ar.DefaultResolverContext(paths))
        )

    def test_ResolveForNewAsset(self):
        resolver  = Ar.GetResolver()

        # ResolveForNewAsset returns the path a new asset would be written
        # to for a given asset path. ArDefaultResolver assumes all asset paths
        # are filesystem paths, so this is just the absolute path of the
        # input.
        self.assertPathsEqual(
            resolver.ResolveForNewAsset('/test/path/1/newfile'),
            os.path.abspath('/test/path/1/newfile'))

        self.assertPathsEqual(
            resolver.ResolveForNewAsset('test/path/1/newfile'),
            os.path.abspath('test/path/1/newfile'))

        # This should work even if a file happens to already exist at the
        # computed path.
        testDir = os.path.abspath('ResolveForNewAsset')
        if os.path.isdir(testDir):
            shutil.rmtree(testDir)
        os.makedirs(testDir)

        testFileName = 'test_ResolveForNewAsset.txt'
        testFileAbsPath = os.path.join(testDir, testFileName)
        with open(testFileAbsPath, 'w') as ofp:
            print('Garbage', file=ofp)

        self.assertPathsEqual(
            resolver.ResolveForNewAsset(testFileAbsPath),
            testFileAbsPath)

        self.assertPathsEqual(
            resolver.ResolveForNewAsset(
                'ResolveForNewAsset/test_ResolveForNewAsset.txt'),
            testFileAbsPath)

    def test_CreateContextFromString(self):
        resolver = Ar.GetResolver()

        def _TestWithPaths(searchPaths):
            self.assertEqual(
                resolver.CreateContextFromString(os.pathsep.join(searchPaths)),
                Ar.ResolverContext(Ar.DefaultResolverContext(searchPaths)))
            self.assertEqual(
                resolver.CreateContextFromStrings(
                    [("", os.pathsep.join(searchPaths))]),
                Ar.ResolverContext(Ar.DefaultResolverContext(searchPaths)))

        _TestWithPaths([])
        _TestWithPaths(["/a"])
        _TestWithPaths(["/a", "/b"])

    def test_RefreshContextNotifications(self):
        """Tests that ArDefaultResolver emits change notifications when
           a ArDefaultResolverContext is refreshed"""

        class _Listener(object):
            def __init__(self):
                self._key = Tf.Notice.RegisterGlobally(
                    Ar.Notice.ResolverChanged, self._HandleNotice)
                self.Reset()
            
            def Reset(self):
                self.receivedNotice = False
                self.affectsContext = False

            def _HandleNotice(self, notice, sender):
                self.receivedNotice = True
                self.affectsContext = notice.AffectsContext(
                    Ar.DefaultResolverContext())

        l = _Listener()
        # update default path which should trigger a notification
        Ar.DefaultResolver.SetDefaultSearchPath(["newSearchDir"])
        self.assertTrue(l.receivedNotice)
        self.assertTrue(l.affectsContext)

        # no changes here so we should not receive notification
        l.Reset()
        Ar.DefaultResolver.SetDefaultSearchPath(["newSearchDir"])
        self.assertFalse(l.receivedNotice)
        self.assertFalse(l.affectsContext)

if __name__ == '__main__':
    unittest.main()

