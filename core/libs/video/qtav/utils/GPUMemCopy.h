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
