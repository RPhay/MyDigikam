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

#include "SurfaceInteropD3D11.h"

#define DX_LOG_COMPONENT "D3D11GL Interop"

// Qt includes

#include <QVector>

// Local includes

#include "VideoDecoderD3D.h"
#include "DirectXHelper.h"
#include "OpenGLHelper.h"
#include "digikam_debug.h"

// TODO: does intel supports d3d11-gl interop?
// dynamic gl or desktop gl

namespace QtAV
{

namespace d3d11
{

class Q_DECL_HIDDEN GLInteropResource final : public InteropResource
{
public:

    GLInteropResource();
    ~GLInteropResource();

    VideoFormat::PixelFormat format(DXGI_FORMAT dxfmt) const override
    {
        if (dxfmt == DXGI_FORMAT_NV12)
            return VideoFormat::Format_NV12;

        if (dxfmt == DXGI_FORMAT_P010)
            return VideoFormat::Format_YUV420P10LE;

        if (dxfmt == DXGI_FORMAT_P016)
            return VideoFormat::Format_YUV420P16LE;

        return VideoFormat::Format_Invalid;
    }

    bool map(ComPtr<ID3D11Texture2D> surface, int index, GLuint tex, int w, int h, int plane) override;
    bool unmap(GLuint tex) override;

private:

    bool ensureResource(DXGI_FORMAT fmt, int w, int h, GLuint tex, int plane);

private:

    HANDLE                            interop_dev;
    HANDLE                            interop_obj[2];
    DXGI_FORMAT                       tex_format;
    QVector<ComPtr<ID3D11Texture2D> > d3dtex;
    int                               mapped;
    GLuint                            gltex[2];
};

InteropResource* CreateInteropGL()
{
    return new GLInteropResource();
}

GLInteropResource::GLInteropResource()
    : interop_dev(nullptr),
      tex_format (DXGI_FORMAT_UNKNOWN),
      mapped     (0)
{
    d3dtex.reserve(3);
    d3dtex.resize(2);
    memset(interop_obj, 0, sizeof(interop_obj));
    memset(gltex, 0, sizeof(gltex));
}

GLInteropResource::~GLInteropResource()
{
    // FIXME: why unregister/close interop obj/dev here will
    // crash (tested on intel driver)? must be in current opengl context?
}

bool GLInteropResource::map(ComPtr<ID3D11Texture2D> surface, int index, GLuint tex, int w, int h, int plane)
{
    gltex[plane] = tex;
    D3D11_TEXTURE2D_DESC desc;
    surface->GetDesc(&desc);

    if (!ensureResource(desc.Format, w, h, tex, plane))
        return false;

    // open/close and register/unregster in every map/unmap to ensure called in current
    // context and avoid crash (tested on intel driver)
    // interop operations begin

    WGL_ENSURE((interop_dev = gl().DXOpenDeviceNV(d3ddev.Get())) != nullptr, false);

    // call in ensureResource or in map?

    WGL_ENSURE((interop_obj[plane] = gl().DXRegisterObjectNV(interop_dev,
                                                             d3dtex[plane].Get(),
                                                             tex,
                                                             GL_TEXTURE_2D,
                                                             WGL_ACCESS_READ_ONLY_NV)) != nullptr, false);

    // prepare dx resources for gl

    D3D11_BOX box;
    ZeroMemory(&box, sizeof(box));
    box.right = w;

    if (plane == 0)
    {
        box.bottom = h;
    }
    else
    {
        box.top    = desc.Height; // maybe > h
        box.bottom = box.top + h / 2;
    }

    ComPtr<ID3D11DeviceContext> ctx;
    d3ddev->GetImmediateContext(&ctx);
    ctx->CopySubresourceRegion(d3dtex[plane].Get(), 0, 0, 0, 0,
                               surface.Get(),
                               index,
                               &box);
    // lock dx resources

    WGL_ENSURE(gl().DXLockObjectsNV(interop_dev, 1, &interop_obj[plane]), false);
    WGL_ENSURE(gl().DXObjectAccessNV(interop_obj[plane], WGL_ACCESS_READ_ONLY_NV), false);
    DYGL(glBindTexture(GL_TEXTURE_2D, tex));

    return true;
}

bool GLInteropResource::unmap(GLuint tex)
{
    Q_UNUSED(tex);
    int plane = 0;

    if (gltex[plane] != tex)
        plane = 1;

    if (!interop_obj[plane])
        return false;

    DYGL(glBindTexture(GL_TEXTURE_2D, 0));
    WGL_ENSURE(gl().DXUnlockObjectsNV(interop_dev, 1, &interop_obj[plane]), false);
    WGL_WARN(gl().DXUnregisterObjectNV(interop_dev, interop_obj[plane]));

    // interop operations end

    WGL_WARN(gl().DXCloseDeviceNV(interop_dev));
    interop_obj[plane] = nullptr;

    return true;
}

static const struct Q_DECL_HIDDEN
{
    DXGI_FORMAT fmt;
    DXGI_FORMAT plane_fmt[2];
}
plane_formats[] =
{
    { DXGI_FORMAT_R8G8B8A8_UNORM, { DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_UNKNOWN      } },
    { DXGI_FORMAT_NV12,           { DXGI_FORMAT_R8_UNORM,       DXGI_FORMAT_R8G8_UNORM   } },
    { DXGI_FORMAT_P010,           { DXGI_FORMAT_R16_UNORM,      DXGI_FORMAT_R16G16_UNORM } },
    { DXGI_FORMAT_P016,           { DXGI_FORMAT_R16_UNORM,      DXGI_FORMAT_R16G16_UNORM } },
};

static DXGI_FORMAT GetPlaneFormat(DXGI_FORMAT fmt, int plane)
{
    for (size_t i = 0 ; i < (sizeof(plane_formats) / sizeof(plane_formats[0])) ; ++i)
    {
        if (plane_formats[i].fmt == fmt)
            return plane_formats[i].plane_fmt[plane];
    }

    return DXGI_FORMAT_UNKNOWN;
}

// IDirect3DDevice9 can not be used on WDDM OSes(>=vista)

bool GLInteropResource::ensureResource(DXGI_FORMAT fmt, int w, int h, GLuint tex, int plane)
{
    Q_UNUSED(tex);
    Q_ASSERT(gl().DXRegisterObjectNV && "WGL_NV_DX_interop is required");

    if ((fmt == tex_format) && d3dtex[plane].Get() && (width == w) && (height == h))
        return true;

    if (mapped == 2)
    {
        d3dtex.clear();
        d3dtex.resize(2);
        mapped = 0;
    }

    CD3D11_TEXTURE2D_DESC desc(GetPlaneFormat(fmt, plane), w, h, 1, 1);
    desc.BindFlags |= D3D11_BIND_RENDER_TARGET;
    desc.MiscFlags  = D3D11_RESOURCE_MISC_SHARED;
    DX_ENSURE(d3ddev->CreateTexture2D(&desc, nullptr, &d3dtex[plane]), false);

#if 0

    ComPtr<IDXGIResource> resource;
    DX_ENSURE(d3dtex[plane].As(&resource), false);
    HANDLE share_handle = nullptr;
    DX_ENSURE(resource->GetSharedHandle(&share_handle), false);

    // required by d3d9 not d3d10&11: https://www.opengl.org/registry/specs/NV/DX_interop2.txt

    WGL_WARN(gl().DXSetResourceShareHandleNV(d3dtex[plane].Get(), share_handle));

#endif

    width      = w;
    height     = h;
    tex_format = fmt;
    mapped++;

    return true;
}

} // namespace d3d11

} // namespace QtAV
