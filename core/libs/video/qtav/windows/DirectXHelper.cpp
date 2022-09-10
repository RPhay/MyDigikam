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

#include "DirectXHelper.h"

namespace QtAV
{

namespace DXHelper
{

const char* vendorName(unsigned id)
{
    static const struct Q_DECL_HIDDEN
    {
        unsigned id       = 0;
        char     name[32] = { 0 };
    }
    vendors [] =
    {
        { 0x1002,     "ATI"         },
        { 0x10DE,     "NVIDIA"      },
        { 0x1106,     "VIA"         },
        { 0x8086,     "Intel"       },
        { 0x5333,     "S3 Graphics" },
        { 0x4D4F4351, "Qualcomm"    },
        { 0,          ""            }
    };

    const char* vendor = "Unknown";

    for (int i = 0 ; vendors[i].id != 0 ; ++i)
    {
        if (vendors[i].id == id)
        {
            vendor = vendors[i].name;

            break;
        }
    }

    return vendor;
}

#ifndef Q_OS_WINRT

static void InitParameters(D3DPRESENT_PARAMETERS* d3dpp)
{
    ZeroMemory(d3dpp, sizeof(*d3dpp));

    // use mozilla's parameters

    d3dpp->Flags                  = D3DPRESENTFLAG_VIDEO;
    d3dpp->Windowed               = TRUE;
    d3dpp->hDeviceWindow          = ::GetShellWindow();     // nullptr;
    d3dpp->SwapEffect             = D3DSWAPEFFECT_DISCARD;
/*
    d3dpp->MultiSampleType        = D3DMULTISAMPLE_NONE;
    d3dpp->PresentationInterval   = D3DPRESENT_INTERVAL_DEFAULT;
*/
    d3dpp->BackBufferCount        = 1;                      // 0;                  /* FIXME what to put here */
    d3dpp->BackBufferFormat       = D3DFMT_UNKNOWN;         // D3DFMT_X8R8G8B8;    /* FIXME what to put here */
    d3dpp->BackBufferWidth        = 1;                      // 0;
    d3dpp->BackBufferHeight       = 1;                      // 0;
/*
    d3dpp->EnableAutoDepthStencil = FALSE;
*/
}

IDirect3DDevice9* CreateDevice9Ex(HINSTANCE dll, IDirect3D9Ex** d3d9ex, D3DADAPTER_IDENTIFIER9* d3dai)
{
    qCDebug(DIGIKAM_QTAV_LOG).noquote()
        << QString::asprintf("creating d3d9 device ex... dll: %p", dll);

    // http://msdn.microsoft.com/en-us/library/windows/desktop/bb219676(v=vs.85).aspx   // krazy:exclude=insecurenet

    typedef HRESULT (WINAPI* Create9ExFunc)(UINT SDKVersion, IDirect3D9Ex** ppD3D);     // IDirect3D9Ex: void is ok

    Create9ExFunc Create9Ex = (Create9ExFunc)GetProcAddress(dll, "Direct3DCreate9Ex");

    if (!Create9Ex)
    {
        qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
            << QString::asprintf("Symbol not found: Direct3DCreate9Ex");

        return nullptr;
    }

    DX_ENSURE(Create9Ex(D3D_SDK_VERSION, d3d9ex), nullptr); // TODO: will D3D_SDK_VERSION be override by other headers?

    if (d3dai)
        DX_WARN((*d3d9ex)->GetAdapterIdentifier(D3DADAPTER_DEFAULT, 0, d3dai));

    D3DPRESENT_PARAMETERS d3dpp;
    InitParameters(&d3dpp);

    // D3DCREATE_MULTITHREADED is required by gl interop. https://www.opengl.org/registry/specs/NV/DX_interop.txt
    // D3DCREATE_SOFTWARE_VERTEXPROCESSING in other dxva decoders. D3DCREATE_HARDWARE_VERTEXPROCESSING is required by cuda in cuD3D9CtxCreate()

    DWORD flags                 = D3DCREATE_FPU_PRESERVE | D3DCREATE_MULTITHREADED | D3DCREATE_HARDWARE_VERTEXPROCESSING;
    IDirect3DDevice9Ex* d3d9dev = nullptr;

    // mpv:

    /**
     * Direct3D needs a HWND to create a device, even without using ::Present
     * this HWND is used to alert Direct3D when there's a change of focus window.
     * For now, use GetDesktopWindow, as it looks harmless
     */

    DX_ENSURE((*d3d9ex)->CreateDeviceEx(D3DADAPTER_DEFAULT,
                                        D3DDEVTYPE_HAL, GetShellWindow(), // GetDesktopWindow(), //GetShellWindow()?
                                        flags,
                                        &d3dpp,
                                        nullptr,
                                        (IDirect3DDevice9Ex**)(&d3d9dev)),
                                        nullptr);

    qCDebug(DIGIKAM_QTAV_LOG).noquote()
        << QString::asprintf("IDirect3DDevice9Ex created");

    return d3d9dev;
}

IDirect3DDevice9* CreateDevice9(HINSTANCE dll, IDirect3D9** d3d9, D3DADAPTER_IDENTIFIER9* d3dai)
{
    qCDebug(DIGIKAM_QTAV_LOG).noquote()
        << QString::asprintf("creating d3d9 device...");

    typedef IDirect3D9* (WINAPI* Create9Func)(UINT SDKVersion);

    Create9Func Create9 = (Create9Func)GetProcAddress(dll, "Direct3DCreate9");

    if (!Create9)
    {
        qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
            << QString::asprintf("Symbol not found: Direct3DCreate9");

        return nullptr;
    }

    *d3d9 = Create9(D3D_SDK_VERSION);

    if (!(*d3d9))
    {
        qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
            << QString::asprintf("Direct3DCreate9 failed");

        return nullptr;
    }

    if (d3dai)
        DX_WARN((*d3d9)->GetAdapterIdentifier(D3DADAPTER_DEFAULT, 0, d3dai));

    D3DPRESENT_PARAMETERS d3dpp;
    InitParameters(&d3dpp);
    DWORD flags               = D3DCREATE_FPU_PRESERVE | D3DCREATE_MULTITHREADED | D3DCREATE_MIXED_VERTEXPROCESSING;
    IDirect3DDevice9* d3d9dev = nullptr;

    DX_ENSURE(((*d3d9)->CreateDevice(D3DADAPTER_DEFAULT,
                                     D3DDEVTYPE_HAL, GetShellWindow(), // GetDesktopWindow(), //GetShellWindow()?
                                     flags,
                                     &d3dpp, &d3d9dev)),
                                     nullptr);

    qCDebug(DIGIKAM_QTAV_LOG).noquote()
        << QString::asprintf("IDirect3DDevice9 created");

    return d3d9dev;
}

#endif // Q_OS_WINRT

} // namespace DXHelper

} // namespace QtAV
