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

#ifndef QTAV_SURFACE_INTEROP_H
#define QTAV_SURFACE_INTEROP_H

// Qt includes

#include <QSharedPointer>

// Local includes

#include "VideoFormat.h"

namespace QtAV
{

class DIGIKAM_EXPORT VideoSurfaceInterop              // clazy:exclude=copyable-polymorphic
{
public:

    virtual ~VideoSurfaceInterop()
    {
    }

    /*!
     * \brief map
     * currently is used to map a frame from hardware decoder to opengl texture, host memory.
     * \param type currently only support GLTextureSurface and HostMemorySurface for some decoders
     * \param fmt
     *   HostMemorySurface: must be a packed rgb format
     * \param handle address of real handle. handle value can be modified in map()
     *   and the caller (VideoShader for example) should manage the changes.
     *   GLTextureSurface: usually opengl texture. maybe other objects for some decoders in the feature
     *   HostMemorySurface: a VideoFrame ptr
     * \param plane
     * \return Null if not supported or failed. handle if success.
     */
    virtual void* map(SurfaceType type, const VideoFormat& fmt, void* handle = nullptr, int plane = 0)
    {
        Q_UNUSED(type);
        Q_UNUSED(fmt);
        Q_UNUSED(handle);
        Q_UNUSED(plane);

        return nullptr;
    }

    // TODO: SurfaceType. unmap is currently used by opengl rendering

    virtual void unmap(void* handle)
    {
        Q_UNUSED(handle);
    }
/*
    virtual void unmap(void* handle, SurfaceType type) { Q_UNUSED(handle); } //for SourceSurfaceType
*/
    /*!
     * \brief createHandle
     * It is used by opengl renderer to create a texture when rendering frame from VDA/VideoToolbox decoder
     * VideoSurfaceInterop does not have the ownership. VideoShader does
     * \return nullptr if not used for opengl rendering. handle if create here
     */
    virtual void* createHandle(void* handle, SurfaceType type, const VideoFormat& fmt,
                               int plane, int planeWidth, int planeHeight)
    {
        Q_UNUSED(handle);
        Q_UNUSED(type);
        Q_UNUSED(fmt);
        Q_UNUSED(plane);
        Q_UNUSED(planeWidth);
        Q_UNUSED(planeHeight);

        return nullptr;
    }
};

typedef QSharedPointer<VideoSurfaceInterop> VideoSurfaceInteropPtr;

} // namespace QtAV

Q_DECLARE_METATYPE(QtAV::VideoSurfaceInteropPtr)

#endif // QTAV_SURFACE_INTEROP_H
