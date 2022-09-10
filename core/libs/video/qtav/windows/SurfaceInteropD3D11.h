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

#ifndef QTAV_SURFACE_INTEROP_D3D11_H
#define QTAV_SURFACE_INTEROP_D3D11_H

// Local includes

#include "SurfaceInterop.h"
#include "dxcompat.h"

// Windows includes

#include <d3d11.h>
#include <wrl/client.h>

using namespace Microsoft::WRL;

namespace QtAV
{

namespace d3d11
{

enum InteropType
{
    InteropAuto,
    InteropEGL,
    InteropGL   ///< NOT IMPLEMENTED
};

class InteropResource
{
public:

    typedef unsigned int GLuint;

    static InteropResource* create(InteropType type = InteropAuto);

    /*!
     * \brief isSupported
     * \return true if support 0-copy interop. Currently only d3d11+egl,
     * i.e. check egl build environment and runtime egl support.
     */
    static bool isSupported(InteropType type = InteropAuto);

    virtual ~InteropResource()
    {
    }

    void setDevice(ComPtr<ID3D11Device> dev);
    virtual VideoFormat::PixelFormat format(DXGI_FORMAT dxfmt) const = 0;

    /*!
     * \brief map
     * \param surface dxva decoded surface
     * \param index ID3D11Texture2D array  index
     * \param tex opengl texture
     * \param w frame width(visual width) without alignment, <= dxva surface width
     * \param h frame height(visual height)
     * \param plane useless now
     * \return true if success
     */
    virtual bool map(ComPtr<ID3D11Texture2D> surface, int index, GLuint tex, int w, int h, int plane) = 0;

    virtual bool unmap(GLuint tex)
    {
        Q_UNUSED(tex);

        return true;
    }

protected:

    ComPtr<ID3D11Device> d3ddev;

    /**
     * video frame width and dx_surface width without alignment, not dxva decoded surface width
     */
    int                  width;
    int                  height;
};

typedef QSharedPointer<InteropResource> InteropResourcePtr;

// ---------------------------------------------------------------------------

class SurfaceInterop final : public VideoSurfaceInterop
{
public:

    explicit SurfaceInterop(const InteropResourcePtr& res)
        : m_index     (0),
          m_resource  (res),
          frame_width (0),
          frame_height(0)
    {
    }

    /*!
     * \brief setSurface
     * \param surface d3d11 decoded surface
     * \param index ID3D11Texture2D array  index
     * \param frame_w frame width(visual width) without alignment, <= d3d11 surface width
     * \param frame_h frame height(visual height)
     */
    void setSurface(ComPtr<ID3D11Texture2D> surface, int index, int frame_w, int frame_h);

    /// GLTextureSurface only supports rgb32

    void* map(SurfaceType type, const VideoFormat& fmt, void* handle, int plane) override;
    void  unmap(void* handle)                                                    override;

protected:

    /// copy from gpu (optimized if possible) and convert to target format if necessary

    void* mapToHost(const VideoFormat& format, void* handle, int plane);

private:

    ComPtr<ID3D11Texture2D> m_surface;
    int                     m_index;
    InteropResourcePtr      m_resource;
    int                     frame_width;
    int                     frame_height;
};

} // namespace d3d11

} // namespace QtAV

#endif // QTAV_SURFACE_INTEROP_D3D11_H
