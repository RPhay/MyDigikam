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

#ifndef QTAV_GPU_MEM_COPY_H
#define QTAV_GPU_MEM_COPY_H

// C includes

#include <stddef.h>

namespace QtAV
{

class GPUMemCopy
{
public:

    static bool isAvailable();

    GPUMemCopy();
    ~GPUMemCopy();

    // available and initialized

    bool isReady() const;
    bool initCache(unsigned int width);
    void cleanCache();
    void copyFrame(void* pSrc, void* pDest, unsigned int width, unsigned int height, unsigned int pitch);

    // memcpy

private:

    bool mInitialized = false;

    typedef struct Cache_T_
    {
        unsigned char* buffer = nullptr;
        size_t         size   = 0;
    } cache_t;

    cache_t mCache;
};

void* gpu_memcpy(void* dst, const void* src, size_t size);

} // namespace QtAV

#endif // QTAV_GPU_MEM_COPY_H
