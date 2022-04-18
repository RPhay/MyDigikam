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

namespace nvcuvid { //the macro define class dll. so a namespace wrapper is required
//DEFINE_DLL_INSTANCE_N("nvcuvid", "nvcuvid", NULL) //now may crash for vc
static char* cuvid_names[] = { "nvcuvid", NULL };
DEFINE_DLL_INSTANCE_V("nvcuvid", cuvid_names)
}
using namespace nvcuvid;

DEFINE_DLLAPI_M_ARG(3, CUresult, CUDAAPI, cuvidCreateVideoSource, CUvideosource *, const char *, CUVIDSOURCEPARAMS *)
DEFINE_DLLAPI_M_ARG(3, CUresult, CUDAAPI, cuvidCreateVideoSourceW, CUvideosource *, const int *, CUVIDSOURCEPARAMS *)
DEFINE_DLLAPI_M_ARG(1, CUresult, CUDAAPI, cuvidDestroyVideoSource, CUvideosource)
DEFINE_DLLAPI_M_ARG(2, CUresult, CUDAAPI, cuvidSetVideoSourceState, CUvideosource, cudaVideoState)
DEFINE_DLLAPI_M_ARG(1, cudaVideoState, CUDAAPI, cuvidGetVideoSourceState, CUvideosource)
DEFINE_DLLAPI_M_ARG(3, CUresult, CUDAAPI, cuvidGetSourceVideoFormat, CUvideosource, CUVIDEOFORMAT *, unsigned int)
DEFINE_DLLAPI_M_ARG(3, CUresult, CUDAAPI, cuvidGetSourceAudioFormat, CUvideosource, CUAUDIOFORMAT *, unsigned int)
DEFINE_DLLAPI_M_ARG(2, CUresult, CUDAAPI, cuvidCreateVideoParser, CUvideoparser *, CUVIDPARSERPARAMS *)
DEFINE_DLLAPI_M_ARG(2, CUresult, CUDAAPI, cuvidParseVideoData, CUvideoparser, CUVIDSOURCEDATAPACKET *)
DEFINE_DLLAPI_M_ARG(1, CUresult, CUDAAPI, cuvidDestroyVideoParser, CUvideoparser)

} //namespace cuvid
} //namespace dllapi
