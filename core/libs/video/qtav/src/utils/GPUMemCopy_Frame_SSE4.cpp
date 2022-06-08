/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * Copyright (C) 2012-2022 Wang Bin <wbsecg1 at gmail dot com>
 * Copyright (C)      2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
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

#if defined(__SSE__) || defined(_M_IX86) || defined(_M_X64) // gcc, clang defines __SSE__, vc does not

// C++ includes

// for mingw gcc

#   include <smmintrin.h> // stream load
#   include <cstdint>     // intptr_t
#   include <cstring>

// Local includes

#   define STREAM_LOAD_SI128(x) _mm_stream_load_si128(x)

// avoid name conflict

namespace sse4
{

#   define INC_FROM_NAMESPACE
#   include "GPUMemCopy_Frame_SSE2.cpp"

} // namespace sse4

// QT_FUNCTION_TARGET("sse4.1")

void CopyFrame_SSE4(void* pSrc, void* pDest, void* pCacheBlock, UINT width, UINT height, UINT pitch)
{
    sse4::CopyFrame_SSE2(pSrc, pDest, pCacheBlock, width, height, pitch);
}

void* memcpy_sse4(void* dst, const void* src, size_t size)
{
    return sse4::memcpy_sse2(dst, src, size);
}

#endif // defined(__SSE__) || defined(_M_IX86) || defined(_M_X64)
