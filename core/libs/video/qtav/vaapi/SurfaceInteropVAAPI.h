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

#ifndef QTAV_SURFACE_INTEROP_VAAPI_H
#define QTAV_SURFACE_INTEROP_VAAPI_H

// Qt includes

#include <qglobal.h>

// Local includes

#include "vaapi_helper.h"

#define VA_X11_INTEROP 1

#ifndef QT_NO_OPENGL
#   include <QMap>
#   include <QSharedPointer>
#   include "SurfaceInterop.h"

namespace QtAV
{

namespace vaapi
{

bool checkEGL_DMA();
bool checkEGL_Pixmap();

class InteropResource                  // clazy:exclude=copyable-polymorphic
{
public:

    virtual ~InteropResource()
    {
    }

    // egl supports yuv extension

    /*!
     * \brief map
     * \param surface va decoded surface
     * \param tex opengl texture
     * \param w frame width(visual width) without alignment, <= dxva surface width
     * \param h frame height(visual height)
     * \param plane useless now
     * \return true if success
     */
    virtual bool map(const surface_ptr& surface, GLuint tex, int w, int h, int plane) = 0;

    virtual bool unmap(const surface_ptr& surface, GLuint tex)
    {
        Q_UNUSED(surface);
        Q_UNUSED(tex);

        return true;
    }
};

typedef QSharedPointer<InteropResource> InteropResourcePtr;

// ----------------------------------------------------------------------------------

class SurfaceInteropVAAPI final : public VideoSurfaceInterop
{
public:

    explicit SurfaceInteropVAAPI(const InteropResourcePtr& res)
        : frame_width (0),
          frame_height(0),
          m_resource  (res)
    {
    }

    void setSurface(const surface_ptr& surface, int w, int h);      // use surface->width/height if w/h is 0
    void* map(SurfaceType type, const VideoFormat& fmt, void* handle, int plane) override;
    void unmap(void* handle) override;

protected:

    void* mapToHost(const VideoFormat& format, void* handle, int plane);

private:

    int                frame_width  = 0;
    int                frame_height = 0;

    // NOTE: must ensure va-x11/va-glx is unloaded after all va calls(don't know why,
    // but it's true), for example vaTerminate(), to avoid crash
    // so declare InteropResourcePtr first then surface_ptr.
    // InteropResource (va-xxx.so) will be destroyed later than surface_t (vaTerminate())
    // also call vaInitialize() before vaTerminate() can avoid such crashes. Don't know why.

    InteropResourcePtr m_resource;
    surface_ptr        m_surface;

private:

    Q_DISABLE_COPY(SurfaceInteropVAAPI);
};

// ----------------------------------------------------------------------------------

/**
 * load/resolve symbols only once in decoder and pass a VAAPI_XXX ptr or use pool
 */
class GLXInteropResource final: public InteropResource,
                                protected VAAPI_GLX
{
public:

    bool map(const surface_ptr& surface, GLuint tex, int w, int h, int) override;

private:

    surface_glx_ptr surfaceGLX(const display_ptr& dpy, GLuint tex);

private:

    /**
     * Used to render to different texture. surface_glx_ptr is created with texture
     */
    QMap<GLuint, surface_glx_ptr> glx_surfaces;
};

// ----------------------------------------------------------------------------------

class X11;

class X11InteropResource final: public InteropResource,
                                protected VAAPI_X11
{
public:

    X11InteropResource();
    ~X11InteropResource();

    bool map(const surface_ptr& surface, GLuint tex, int w, int h, int) override;
    bool unmap(const surface_ptr& surface, GLuint tex)                  override;

private:

    bool ensurePixmaps(int w, int h);

private:

    Display* xdisplay = nullptr;
    int      width    = 0;
    int      height   = 0;
    X11*     x11      = nullptr;
};

#   if QTAV_HAVE(EGL_CAPI)

// libva-egl is dead and not complete. here we use dma

class EGL;

class EGLInteropResource final : public InteropResource
{
public:

    EGLInteropResource();
    ~EGLInteropResource();

    bool map(const surface_ptr& surface, GLuint tex, int w, int h, int plane) override;
    bool unmap(const surface_ptr& surface, GLuint tex)                        override;

private:

    bool ensure();
    void destroy(VADisplay va_dpy); // destroy dma buffer and egl images

private:

    uintptr_t         vabuf_handle;
    VAImage           va_image;
    QMap<GLuint, int> mapped;
    EGL*              egl = nullptr;

private:

    Q_DISABLE_COPY(EGLInteropResource);
};

#   endif // QTAV_HAVE(EGL_CAPI)

} // namespace vaapi

} // namespace QtAV

#endif // QT_NO_OPENGL

#endif // QTAV_SURFACE_INTEROP_VAAPI_H
