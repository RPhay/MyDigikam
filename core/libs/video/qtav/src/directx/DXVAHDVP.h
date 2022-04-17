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

#ifndef QTAV_D3D9VPP_H
#define QTAV_D3D9VPP_H
#include <QRect>
#include "directx/dxcompat.h"
#include <d3d9.h>
#include <dxvahd.h>
#include <wrl/client.h>
#include <QtAV/QtAV_Global.h>
using namespace Microsoft::WRL;
// https://msdn.microsoft.com/en-us/library/windows/desktop/ee663581(v=vs.85).aspx

namespace QtAV
{
namespace dx {

class DXVAHDVP
{
public:
    // brightness, contrast, hue, saturation, rotation, source/dest rect
    DXVAHDVP(ComPtr<IDirect3DDevice9> dev);
    void setOutput(IDirect3DSurface9* surface);
    void setSourceRect(const QRect& r);
    // input color space and range
    void setColorSpace(ColorSpace value);
    void setColorRange(ColorRange value);
    bool process(IDirect3DSurface9 *surface);
private:
    bool ensureResource(UINT width, UINT height, D3DFORMAT format);

    ComPtr<IDirect3DDevice9Ex> m_dev;
    ComPtr<IDXVAHD_Device> m_viddev;
    ComPtr<IDXVAHD_VideoProcessor> m_vp;
    ComPtr<IDirect3DSurface9> m_out;
    UINT m_w, m_h; //enumerator
    ColorSpace m_cs;
    ColorRange m_range;
    QRect m_srcRect;
    PDXVAHD_CreateDevice fDXVAHD_CreateDevice;
    D3DFORMAT m_fmt;
};
} //namespace dx
} // namespace QtAV
#endif //QTAV_D3D9VPP_H
