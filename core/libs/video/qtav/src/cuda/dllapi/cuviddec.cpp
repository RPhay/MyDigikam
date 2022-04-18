/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * Copyright (C) 2012-2022 Wang Bin <wbsecg1 at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "dllapi_p.h"
#include "dllapi.h"

// include nv_inc.h headers later to avoid build error. have not find out why it happens
#define __CUVID_INTERNAL //avoid replaced bt 64 api
#define __CUDA_API_VERSION_INTERNAL
#include "nv_inc.h"

using namespace dllapi;
namespace dllapi {
namespace cuda {

namespace cuviddec { //the macro define class dll. so a namespace wrapper is required
//DEFINE_DLL_INSTANCE_N("cuviddec", "nvcuvid", NULL) //now may crash for vc
static char* cuvid_names[] = { "nvcuvid", NULL };
DEFINE_DLL_INSTANCE_V("cuviddec", cuvid_names)
}
using namespace cuviddec;
DEFINE_DLLAPI_M_ARG(2, CUresult, CUDAAPI, cuvidCreateDecoder, CUvideodecoder *, CUVIDDECODECREATEINFO *)
DEFINE_DLLAPI_M_ARG(1, CUresult, CUDAAPI, cuvidDestroyDecoder, CUvideodecoder)
DEFINE_DLLAPI_M_ARG(2, CUresult, CUDAAPI, cuvidDecodePicture, CUvideodecoder, CUVIDPICPARAMS *)
DEFINE_DLLAPI_M_ARG(5, CUresult, CUDAAPI, cuvidMapVideoFrame, CUvideodecoder, int, unsigned int *, unsigned int *, CUVIDPROCPARAMS *)
DEFINE_DLLAPI_M_ARG(5, CUresult, CUDAAPI, cuvidMapVideoFrame64, CUvideodecoder, int, unsigned long long *, unsigned int *, CUVIDPROCPARAMS *)
DEFINE_DLLAPI_M_ARG(2, CUresult, CUDAAPI, cuvidUnmapVideoFrame, CUvideodecoder, unsigned int)
DEFINE_DLLAPI_M_ARG(2, CUresult, CUDAAPI, cuvidUnmapVideoFrame64, CUvideodecoder, unsigned long long)
DEFINE_DLLAPI_M_ARG(3, CUresult, CUDAAPI, cuvidGetVideoFrameSurface, CUvideodecoder, int, void **)
DEFINE_DLLAPI_M_ARG(2, CUresult, CUDAAPI, cuvidCtxLockCreate, CUvideoctxlock *, CUcontext)
DEFINE_DLLAPI_M_ARG(1, CUresult, CUDAAPI, cuvidCtxLockDestroy, CUvideoctxlock)
DEFINE_DLLAPI_M_ARG(2, CUresult, CUDAAPI, cuvidCtxLock, CUvideoctxlock, unsigned int)
DEFINE_DLLAPI_M_ARG(2, CUresult, CUDAAPI, cuvidCtxUnlock, CUvideoctxlock, unsigned int)

} //namespace cuviddec
} //namespace dllapi
