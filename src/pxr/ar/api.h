// Copyright 2017 Pixar
//
// Licensed under the terms set forth in the LICENSE.txt file available at
// https://openusd.org/license.
//
// Modified by Jeremy Retailleau.

#ifndef PXR_AR_API_H
#define PXR_AR_API_H

#include <pxr/arch/export.h>

#if defined(PXR_STATIC)
#   define AR_API
#   define AR_API_TEMPLATE_CLASS(...)
#   define AR_API_TEMPLATE_STRUCT(...)
#   define AR_LOCAL
#else
#   if defined(AR_EXPORTS)
#       define AR_API ARCH_EXPORT
#       define AR_API_TEMPLATE_CLASS(...) ARCH_EXPORT_TEMPLATE(class, __VA_ARGS__)
#       define AR_API_TEMPLATE_STRUCT(...) ARCH_EXPORT_TEMPLATE(struct, __VA_ARGS__)
#   else
#       define AR_API ARCH_IMPORT
#       define AR_API_TEMPLATE_CLASS(...) ARCH_IMPORT_TEMPLATE(class, __VA_ARGS__)
#       define AR_API_TEMPLATE_STRUCT(...) ARCH_IMPORT_TEMPLATE(struct, __VA_ARGS__)
#   endif
#   define AR_LOCAL ARCH_HIDDEN
#endif

#endif
