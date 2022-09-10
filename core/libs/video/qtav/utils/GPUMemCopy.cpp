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

#include "GPUMemCopy.h"

// C++ includes

#include <cstring>      // memcpy
#include <algorithm>

// FFMPEG includes

extern "C"
{
#include <libavutil/cpu.h>
}

// Local includes

#include "QtAV_Global.h"

// read qsimd_p.h

#define UINT unsigned int

void CopyFrame_SSE2(void* pSrc, void* pDest, void* pCacheBlock, UINT width, UINT height, UINT pitch);
void CopyFrame_SSE4(void* pSrc, void* pDest, void* pCacheBlock, UINT width, UINT height, UINT pitch);

void* memcpy_sse2(void* dst, const void* src, size_t size);
void* memcpy_sse4(void* dst, const void* src, size_t size);

namespace QtAV
{

bool detect_sse4()
{
    static bool is_sse4 = !!(av_get_cpu_flags() & AV_CPU_FLAG_SSE4);

    return is_sse4;
}

bool detect_sse2()
{
    static bool is_sse2 = !!(av_get_cpu_flags() & AV_CPU_FLAG_SSE2);

    return is_sse2;
}

bool GPUMemCopy::isAvailable()
{

#if QTAV_HAVE(SSE4_1) && defined(Q_PROCESSOR_X86)

    if (detect_sse4())
    {
        return true;
    }

#endif

#if QTAV_HAVE(SSE2) && defined(Q_PROCESSOR_X86)

    if (detect_sse2())
    {
        return true;
    }

#endif

    return false;
}

GPUMemCopy::GPUMemCopy()
    : mInitialized(false)
{
    mCache.buffer = nullptr;
    mCache.size   = 0;
}

GPUMemCopy::~GPUMemCopy()
{
    cleanCache();
}

bool GPUMemCopy::isReady() const
{
    return (mInitialized && GPUMemCopy::isAvailable());
}

#define CACHED_BUFFER_SIZE 4096

bool GPUMemCopy::initCache(unsigned width)
{
    mInitialized  = false;

#if QTAV_HAVE(SSE2) && defined(Q_PROCESSOR_X86)

    mCache.size   = std::max<size_t>((width + 0x0f) & ~ 0x0f, CACHED_BUFFER_SIZE);
    mCache.buffer = (unsigned char*)qMallocAligned(mCache.size, 16);
    mInitialized  = !!mCache.buffer;

    return mInitialized;

#else

    Q_UNUSED(width);

#endif

    return false;
}

void GPUMemCopy::cleanCache()
{
    mInitialized = false;

#if QTAV_HAVE(SSE2) && defined(Q_PROCESSOR_X86)

    if (mCache.buffer)
    {
        qFreeAligned(mCache.buffer);
    }

    mCache.buffer = 0;
    mCache.size   = 0;

#endif

}

void GPUMemCopy::copyFrame(void* pSrc, void* pDest, unsigned width, unsigned height, unsigned pitch)
{

#if QTAV_HAVE(SSE4_1) && defined(Q_PROCESSOR_X86)

    if (detect_sse4())
    {
        CopyFrame_SSE4(pSrc, pDest, mCache.buffer, width, height, pitch);
    }

#elif QTAV_HAVE(SSE2) && defined(Q_PROCESSOR_X86)

    if (detect_sse2())
    {
        CopyFrame_SSE2(pSrc, pDest, mCache.buffer, width, height, pitch);
    }

#else

    Q_UNUSED(pSrc);
    Q_UNUSED(pDest);
    Q_UNUSED(width);
    Q_UNUSED(height);
    Q_UNUSED(pitch);

#endif

}

void* gpu_memcpy(void* dst, const void* src, size_t size)
{

#if QTAV_HAVE(SSE4_1) && defined(Q_PROCESSOR_X86)

    if (detect_sse4())
    {
        return memcpy_sse4(dst, src, size);
    }

#elif QTAV_HAVE(SSE2) && defined(Q_PROCESSOR_X86)

    if (detect_sse2())
    {
        return memcpy_sse2(dst, src, size);
    }

#endif

    return memcpy(dst, src, size);
}

} // namespace QtAV
