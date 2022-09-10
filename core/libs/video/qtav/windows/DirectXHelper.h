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

#ifndef QTAV_DIRECTX_HELPER_H
#define QTAV_DIRECTX_HELPER_H

// Qt includes

#include <qglobal.h>

// Windows includes

#ifndef Q_OS_WINRT
#   include <d3d9.h>
#endif

// Local includes

#include "QtAV_Global.h"
#include "digikam_debug.h"

namespace QtAV
{

#ifndef DX_LOG_COMPONENT
#   define DX_LOG_COMPONENT "DirectX"
#endif

#define DX_ENSURE(f, ...)    DX_CHECK(f, return __VA_ARGS__;)
#define DX_WARN(f)           DX_CHECK(f)
#define DX_ENSURE_OK(f, ...) DX_CHECK(f, return __VA_ARGS__;)

#define DX_CHECK(f, ...)                                                                \
    do {                                                                                \
        HRESULT hr = f;                                                                 \
        if (FAILED(hr)) {                                                               \
            qCWarning(DIGIKAM_QTAV_LOG_WARN)                                            \
                << QString::fromLatin1(DX_LOG_COMPONENT " error@%1. " #f ": (0x%2) %3") \
                    .arg(__LINE__)                                                      \
                    .arg(hr, 0, 16)                                                     \
                    .arg(qt_error_string(hr));                                          \
            __VA_ARGS__                                                                 \
        }                                                                               \
    } while (0)

template <class T>
void SafeRelease(T** ppT)
{
    if (*ppT)
    {
        (*ppT)->Release();
        *ppT = nullptr;
    }
}

namespace DXHelper
{

DIGIKAM_EXPORT const char* vendorName(unsigned id);

#ifndef Q_OS_WINRT

IDirect3DDevice9* CreateDevice9Ex(HINSTANCE dll, IDirect3D9Ex** d3d9ex, D3DADAPTER_IDENTIFIER9* d3dai = nullptr);
IDirect3DDevice9* CreateDevice9(HINSTANCE dll, IDirect3D9** d3d9, D3DADAPTER_IDENTIFIER9* d3dai = nullptr);

#endif

} // namespace DXHelper

} // namespace QtAV

#endif // QTAV_DIRECTX_HELPER_H
