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

#ifndef QTAV_DXVAHDVP_H
#define QTAV_DXVAHDVP_H

// Qt includes

#include <QRect>

// Local includes

#include "QtAV_Global.h"
#include "dxcompat.h"

// Windows includes

#include <d3d9.h>
#include <dxvahd.h>
#include <wrl/client.h>

using namespace Microsoft::WRL;

// https://msdn.microsoft.com/en-us/library/windows/desktop/ee663581(v=vs.85).aspx

namespace QtAV
{

namespace dx
{

class DXVAHDVP
{
public:

    // brightness, contrast, hue, saturation, rotation, source/dest rect

    explicit DXVAHDVP(ComPtr<IDirect3DDevice9> dev);

    void setOutput(IDirect3DSurface9* surface);
    void setSourceRect(const QRect& r);

    // input color space and range

    void setColorSpace(ColorSpace value);
    void setColorRange(ColorRange value);
    bool process(IDirect3DSurface9* surface);

private:

    bool ensureResource(UINT width, UINT height, D3DFORMAT format);

private:

    ComPtr<IDirect3DDevice9Ex>      m_dev;
    ComPtr<IDXVAHD_Device>          m_viddev;
    ComPtr<IDXVAHD_VideoProcessor>  m_vp;
    ComPtr<IDirect3DSurface9>       m_out;

    UINT                            m_w;
    UINT                            m_h;      // enumerator
    ColorSpace                      m_cs;
    ColorRange                      m_range;
    QRect                           m_srcRect;
    PDXVAHD_CreateDevice            fDXVAHD_CreateDevice;
    D3DFORMAT                       m_fmt;
};

} // namespace dx

} // namespace QtAV

#endif // QTAV_DXVAHDVP_H
