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

#include "SurfaceInteropD3D9.h"

#define DX_LOG_COMPONENT "D3D9EGL Interop"

// Local includes

#include "DirectXHelper.h"
#include "VideoDecoderD3D.h"
#include "OpenGLHelper.h"

#ifdef QT_OPENGL_ES_2_ANGLE_STATIC
#   define CAPI_LINK_EGL
#else
#   define EGL_CAPI_NS
#endif

#include "egl_api.h"
#include "digikam_debug.h"

// Windows inckudes

#include <EGL/eglext.h> // include after egl_capi.h to match types

namespace QtAV
{

namespace d3d9
{

class Q_DECL_HIDDEN EGL
{

public:

    EGL()
        : dpy    (EGL_NO_DISPLAY),
          surface(EGL_NO_SURFACE)
    {
    }

    EGLDisplay dpy;
    EGLSurface surface;
};

class Q_DECL_HIDDEN EGLInteropResource final : public InteropResource
{
public:

    explicit EGLInteropResource(IDirect3DDevice9* d3device);
    ~EGLInteropResource();

    bool map(IDirect3DSurface9* surface, GLuint tex, int w, int h, int) override;

private:

    void releaseEGL();
    bool ensureSurface(int w, int h);

private:

    EGL*             egl;
    IDirect3DQuery9* dx_query;

private:

    // Disable
    EGLInteropResource(const EGLInteropResource&)            = delete;
    EGLInteropResource& operator=(const EGLInteropResource&) = delete;
};

InteropResource* CreateInteropEGL(IDirect3DDevice9* dev)
{
    return new EGLInteropResource(dev);
}

EGLInteropResource::EGLInteropResource(IDirect3DDevice9* d3device)
    : InteropResource(d3device),
      egl            (new EGL()),
      dx_query       (nullptr)
{
    DX_ENSURE_OK(d3device->CreateQuery(D3DQUERYTYPE_EVENT, &dx_query));
    dx_query->Issue(D3DISSUE_END);
}

EGLInteropResource::~EGLInteropResource()
{
    releaseEGL();

    if (egl)
    {
        delete egl;
        egl = nullptr;
    }

    SafeRelease(&dx_query);
}

void EGLInteropResource::releaseEGL()
{
    if (egl->surface != EGL_NO_SURFACE)
    {
        eglReleaseTexImage(egl->dpy, egl->surface, EGL_BACK_BUFFER);
        eglDestroySurface(egl->dpy, egl->surface);
        egl->surface = EGL_NO_SURFACE;
    }
}

bool EGLInteropResource::ensureSurface(int w, int h)
{
    if (egl->surface && (width == w) && (height == h))
        return true;

    releaseEGL();
    egl->dpy = eglGetCurrentDisplay();

    qCDebug(DIGIKAM_QTAV_LOG).noquote()
        << QString::asprintf("EGL version: %s, client api: %s",
            eglQueryString(egl->dpy, EGL_VERSION), eglQueryString(egl->dpy, EGL_CLIENT_APIS));

    EGLint cfg_attribs[] =
    {
        EGL_RED_SIZE,             8,
        EGL_GREEN_SIZE,           8,
        EGL_BLUE_SIZE,            8,
        EGL_ALPHA_SIZE,           8,
        EGL_BIND_TO_TEXTURE_RGBA, EGL_TRUE,             // remove?
        EGL_SURFACE_TYPE,         EGL_PBUFFER_BIT,
        EGL_NONE
    };

    EGLint nb_cfgs;
    EGLConfig egl_cfg;

    if (!eglChooseConfig(egl->dpy, cfg_attribs, &egl_cfg, 1, &nb_cfgs))
    {
        qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
            << QString::asprintf("Failed to create EGL configuration");

        return false;
    }

    // check extensions

    QList<QByteArray> extensions = QByteArray(eglQueryString(egl->dpy, EGL_EXTENSIONS)).split(' ');

    // ANGLE_d3d_share_handle_client_buffer will be used if possible
    // TODO: strstr is enough

    const bool kEGL_ANGLE_d3d_share_handle_client_buffer = extensions.contains("EGL_ANGLE_d3d_share_handle_client_buffer");
    const bool kEGL_ANGLE_query_surface_pointer          = extensions.contains("EGL_ANGLE_query_surface_pointer");

    if (!kEGL_ANGLE_d3d_share_handle_client_buffer && !kEGL_ANGLE_query_surface_pointer)
    {
        qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
            << QString::asprintf("EGL extension 'kEGL_ANGLE_query_surface_pointer' or "
                                 "'ANGLE_d3d_share_handle_client_buffer' is required!");

        return false;
    }

    GLint has_alpha = 1; // QOpenGLContext::currentContext()->format().hasAlpha()
    eglGetConfigAttrib(egl->dpy, egl_cfg, EGL_BIND_TO_TEXTURE_RGBA, &has_alpha); // EGL_ALPHA_SIZE

    qCDebug(DIGIKAM_QTAV_LOG).noquote()
        << QString::asprintf("choose egl display:%p config: %p/%d, has alpha: %d",
            egl->dpy, egl_cfg, nb_cfgs, has_alpha);

    EGLint attribs[] =
    {
        EGL_WIDTH,          w,
        EGL_HEIGHT,         h,
        EGL_TEXTURE_FORMAT, (has_alpha ? EGL_TEXTURE_RGBA : EGL_TEXTURE_RGB),
        EGL_TEXTURE_TARGET, EGL_TEXTURE_2D,
        EGL_NONE
    };

    HANDLE share_handle = nullptr;

    if (!kEGL_ANGLE_d3d_share_handle_client_buffer && kEGL_ANGLE_query_surface_pointer)
    {
        EGL_ENSURE((egl->surface = eglCreatePbufferSurface(egl->dpy, egl_cfg, attribs)) != EGL_NO_SURFACE, false);

        qCDebug(DIGIKAM_QTAV_LOG).noquote()
            << QString::asprintf("pbuffer surface: %p", egl->surface);

        PFNEGLQUERYSURFACEPOINTERANGLEPROC eglQuerySurfacePointerANGLE
            = reinterpret_cast<PFNEGLQUERYSURFACEPOINTERANGLEPROC>(eglGetProcAddress("eglQuerySurfacePointerANGLE"));

        if (!eglQuerySurfacePointerANGLE)
        {
            qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
                << QString::asprintf("EGL_ANGLE_query_surface_pointer is not supported");

            return false;
        }

        EGL_ENSURE(eglQuerySurfacePointerANGLE(egl->dpy,
                                               egl->surface,
                                               EGL_D3D_TEXTURE_2D_SHARE_HANDLE_ANGLE,
                                               &share_handle), false);
    }

    releaseDX();

    // _A8 for a yuv plane

    /*
     * d3d resource share requires windows >= vista
     * https://msdn.microsoft.com/en-us/library/windows/desktop/bb219800(v=vs.85).aspx
     * from extension files:
     * d3d9: level must be 1, dimensions must match EGL surface's
     * d3d9ex or d3d10:
     */
    DX_ENSURE_OK(d3ddev->CreateTexture(w, h, 1,
                                       D3DUSAGE_RENDERTARGET,
                                       (has_alpha ? D3DFMT_A8R8G8B8 : D3DFMT_X8R8G8B8),
                                       D3DPOOL_DEFAULT,
                                       &dx_texture,
                                       &share_handle) , false);
    DX_ENSURE_OK(dx_texture->GetSurfaceLevel(0, &dx_surface), false);

    if (kEGL_ANGLE_d3d_share_handle_client_buffer)
    {
        // requires extension EGL_ANGLE_d3d_share_handle_client_buffer
        // egl surface size must match d3d texture's
        // d3d9ex or d3d10 is required

        EGL_ENSURE((egl->surface = eglCreatePbufferFromClientBuffer(egl->dpy,
                                                                    EGL_D3D_TEXTURE_2D_SHARE_HANDLE_ANGLE,
                                                                    share_handle,
                                                                    egl_cfg,
                                                                    attribs)), false);

        qCDebug(DIGIKAM_QTAV_LOG).noquote()
            << QString::asprintf("pbuffer surface from client buffer: %p", egl->surface);
    }

    width  = w;
    height = h;

    return true;
}

bool EGLInteropResource::map(IDirect3DSurface9* surface, GLuint tex, int w, int h, int)
{
    if (!ensureSurface(w, h))
    {
        releaseEGL();
        releaseDX();

        return false;
    }

    const RECT src =
    {
        0,
        0,
        (~0-1)&w,
        (~0-1)&h
    };

    DX_ENSURE(d3ddev->StretchRect(surface, &src, dx_surface, nullptr, D3DTEXF_NONE), false);

    if (dx_query)
    {
        // Flush the draw command now. Ideally, this should be done immediately
        // before the draw call that uses the texture. Flush it once here though.

        dx_query->Issue(D3DISSUE_END); // StretchRect does not supports odd values

        // ensure data is copied to egl surface. Solution and comment is from chromium
        // The DXVA decoder has its own device which it uses for decoding.
        // ANGLE has its own device which we don't have access to.
        // The above code attempts to copy the decoded picture into a surface which is owned by ANGLE.
        // As there are multiple devices involved in this,
        // the StretchRect call above is not synchronous.
        // We attempt to flush the batched operations to ensure that
        // the picture is copied to the surface owned by ANGLE.
        // We need to do this in a loop and call flush multiple times.
        // We have seen the GetData call for flushing the command buffer
        // fail to return success occassionally on multi core machines, leading to an infinite loop.
        // Workaround is to have an upper limit of 10 on the number of iterations
        // to wait for the Flush to finish.

        int k = 0;

        while ((dx_query->GetData(nullptr, 0, D3DGETDATA_FLUSH) == FALSE) && (++k < 10))
        {
            Sleep(1);
        }
    }

    DYGL(glBindTexture(GL_TEXTURE_2D, tex));
    eglBindTexImage(egl->dpy, egl->surface, EGL_BACK_BUFFER);
    DYGL(glBindTexture(GL_TEXTURE_2D, 0));

    return true;
}

} // namespace d3d9

} // namespace QtAV
