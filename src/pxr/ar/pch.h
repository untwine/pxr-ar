// Copyright 2017 Pixar
//
// Licensed under the terms set forth in the LICENSE.txt file available at
// https://openusd.org/license.
//
// Modified by Jeremy Retailleau.

#define TF_MAX_ARITY 7
#include <pxr/arch/defines.h>
#if defined(ARCH_OS_DARWIN)
#include <glob.h>
#include <limits.h>
#include <sys/mount.h>
#include <sys/param.h>
#include <unistd.h>
#include <mach/mach_time.h>
#endif
#if defined(ARCH_OS_LINUX)
#include <glob.h>
#include <limits.h>
#include <sys/param.h>
#include <sys/statfs.h>
#include <unistd.h>
#include <x86intrin.h>
#endif
#if defined(ARCH_OS_WINDOWS)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>
#include <intrin.h>
#include <io.h>
#include <stringapiset.h>
#endif
#include <algorithm>
#include <any>
#include <atomic>
#include <cinttypes>
#include <cmath>
#include <complex>
#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <fcntl.h>
#include <functional>
#include <initializer_list>
#include <iosfwd>
#include <iostream>
#include <iterator>
#include <limits>
#include <list>
#include <locale>
#include <map>
#include <math.h>
#include <memory>
#include <mutex>
#include <new>
#include <numeric>
#include <optional>
#include <ostream>
#include <set>
#include <sstream>
#include <stdarg.h>
#include <stddef.h>
#include <string>
#include <string_view>
#include <sys/stat.h>
#include <sys/types.h>
#include <tuple>
#include <type_traits>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
#include <tbb/concurrent_hash_map.h>
#include <tbb/enumerable_thread_specific.h>
#include <tbb/spin_rw_mutex.h>
#ifdef PXR_PYTHON_SUPPORT_ENABLED
#include <pxr/tf/pySafePython.h>
#endif // PXR_PYTHON_SUPPORT_ENABLED
