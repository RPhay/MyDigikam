/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * SPDX-FileCopyrightText: 2012-2022 Wang Bin <wbsecg1 at gmail dot com>
 * SPDX-FileCopyrightText:      2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef QTAV_NV_INC_H
#define QTAV_NV_INC_H

#undef NV_CONFIG
#define NV_CONFIG(FEATURE) (defined QTAV_HAVE_##FEATURE && QTAV_HAVE_##FEATURE)

// TODO: remove DLLAPI_CUDA code

// high version will define cuXXX macro, so functions here will be not they look like

#if !NV_CONFIG(DLLAPI_CUDA) && !defined(CUDA_LINK)
/*
#define CUDA_FORCE_API_VERSION 3010 // enable this line to build with old cuda APIs
#define __CUDA_API_VERSION_INTERNAL
*/
#endif

#if NV_CONFIG(DLLAPI_CUDA)

namespace dllapi
{

namespace cuda
{

#endif // NV_CONFIG(DLLAPI_CUDA)

#ifdef HAVE_CUDA_H
#   include "cuda.h"
#   include "nvcuvid.h"
// NOTE: __CUDA_API_VERSION is undefined in cuda.h
#else
#   ifdef CUDA_FORCE_API_VERSION
#       define __CUDA_API_VERSION CUDA_FORCE_API_VERSION
#   else
#       ifndef __CUDA_API_VERSION
#           define __CUDA_API_VERSION 4000
#       endif
#   endif
#   include "dynlink_cuda.h"
#   include "dynlink_nvcuvid.h"
#endif

#if NV_CONFIG(DLLAPI_CUDA)

} // namespace cuda

} // namespace dllapi

#endif // NV_CONFIG(DLLAPI_CUDA)

#endif // QTAV_NV_INC_H
