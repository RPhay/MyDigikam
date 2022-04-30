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

#ifndef QTAV_D3D11VP_H
#define QTAV_D3D11VP_H

// Qt includes

#include <QRect>

// Local includes

#include "dxcompat.h"
#include "QtAV_Global.h"

// Windows includes

#include <d3d11.h>
#include <wrl/client.h>

using namespace Microsoft::WRL;

namespace QtAV
{

namespace dx
{

class D3D11VP
{
public:

    // brightness, contrast, hue, saturation, rotation, source/dest rect

    explicit D3D11VP(ComPtr<ID3D11Device> dev);
    void setOutput(ID3D11Texture2D* tex);
    void setSourceRect(const QRect& r);

    // input color space and range

    void setColorSpace(ColorSpace value);
    void setColorRange(ColorRange value);
    bool process(ID3D11Texture2D *texture, int index);

private:

    bool ensureResource(UINT width, UINT height, DXGI_FORMAT format);

private:

    ComPtr<ID3D11Device>                    m_dev;
    ComPtr<ID3D11Texture2D>                 m_out;
    ComPtr<ID3D11VideoDevice>               m_viddev;
    ComPtr<ID3D11VideoProcessorEnumerator>  m_enum;
    ComPtr<ID3D11VideoProcessor>            m_vp;
    ComPtr<ID3D11VideoProcessorOutputView>  m_outview;

    UINT                                    m_w, m_h;      // enumerator
    ColorSpace                              m_cs;
    ColorRange                              m_range;
    QRect                                   m_srcRect;
};

} // namespace dx

} // namespace QtAV

#endif // QTAV_D3D11VP_H
