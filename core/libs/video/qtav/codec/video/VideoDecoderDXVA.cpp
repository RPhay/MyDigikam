/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * SPDX-FileCopyrightText: 2012-2022 Wang Bin <wbsecg1 at gmail dot com>
 * SPDX-FileCopyrightText:      2016 Andy Bell <andy dot bell at displaynote dot com>
 * SPDX-FileCopyrightText:      2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifdef _MSC_VER                         // krazy:exclude=cpp
#   pragma comment(lib, "ole32.lib")    // CoTaskMemFree. why link failed?
#endif

#include "VideoDecoderD3D.h"

#define DX_LOG_COMPONENT "DXVA2"

// Qt includes

#include <QOperatingSystemVersion>

// Local includes

#include "AVCompat.h"
#include "QtAV_factory.h"

//#include "QtAV_mkid.h"

#include "digikam_debug.h"
#include "SurfaceInteropD3D9.h"
#include "DirectXHelper.h"

// d3d9ex: http://dxr.mozilla.org/mozilla-central/source/dom/media/wmf/DXVA2Manager.cpp     // krazy:exclude=insecurenet

// to use c api, add define COBJMACROS and CINTERFACE

#define DXVA2API_USE_BITFIELDS

// FFMpeg includes

extern "C"
{
#include <libavcodec/dxva2.h> // will include d3d9.h, dxva2api.h
}

// Windows includes

#include <d3d9.h>
#include <dxva2api.h>
#include <initguid.h> // must be last included to not redefine existing GUIDs

/* dxva2api.h GUIDs: http://msdn.microsoft.com/en-us/library/windows/desktop/ms697067(v=vs100).aspx     // krazy:exclude=insecurenet
 * assume that they are declared in dxva2api.h
 */

//#define MS_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) // TODO: ???

#define MS_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
    static const GUID name = { l, w1, w2, {b1, b2, b3, b4, b5, b6, b7, b8 } }

#ifdef __MINGW32__                          // krazy:exclude=cpp
#   include <_mingw.h>
#   if !defined(__MINGW64_VERSION_MAJOR)
#       undef MS_GUID
#       define MS_GUID DEFINE_GUID          // dxva2api.h fails to declare those, redefine as static
#   else
#       include <dxva.h>
#   endif
#endif

namespace QtAV
{

MS_GUID(IID_IDirectXVideoDecoderService,      0xfc51a551, 0xd5e7, 0x11d9, 0xaf, 0x55, 0x00, 0x05, 0x4e, 0x43, 0xff, 0x02);
MS_GUID(IID_IDirectXVideoAccelerationService, 0xfc51a550, 0xd5e7, 0x11d9, 0xaf, 0x55, 0x00, 0x05, 0x4e, 0x43, 0xff, 0x02);

class VideoDecoderDXVAPrivate;

class Q_DECL_HIDDEN VideoDecoderDXVA : public VideoDecoderD3D
{
    DPTR_DECLARE_PRIVATE(VideoDecoderDXVA)

public:

    VideoDecoderDXVA();

    VideoDecoderId id()   const override;
    QString description() const override;
    VideoFrame frame()          override;
};

extern VideoDecoderId VideoDecoderId_DXVA;

FACTORY_REGISTER(VideoDecoder, DXVA, "DXVA")

struct Q_DECL_HIDDEN d3d9_surface_t : public va_surface_t
{
    d3d9_surface_t()
        : va_surface_t(),
          d3d         (nullptr)
    {
    }

    ~d3d9_surface_t()
    {
        SafeRelease(&d3d);
    }

    void setSurface(IUnknown* s) override
    {
        d3d = (IDirect3DSurface9*)s;
    }

    IUnknown* getSurface() const override
    {
        return d3d;
    }

private:

    IDirect3DSurface9* d3d;
};

// https://technet.microsoft.com/zh-cn/aa965266(v=vs.98).aspx

class Q_DECL_HIDDEN VideoDecoderDXVAPrivate final : public VideoDecoderD3DPrivate
{
public:

    VideoDecoderDXVAPrivate()
        : VideoDecoderD3DPrivate()
    {
        // d3d9+gl interop may not work on optimus moble platforms, 0-copy is enabled only for egl interop

        if (d3d9::InteropResource::isSupported(d3d9::InteropEGL) &&
            (QOperatingSystemVersion::current() >= QOperatingSystemVersion::Windows7))
        {
            copy_mode = VideoDecoderFFmpegHW::ZeroCopy;
        }

        hd3d9_dll   = 0;
        hdxva2_dll  = 0;
        d3dobj      = nullptr;
        d3ddev      = nullptr;
        token       = 0;
        devmng      = nullptr;
        device      = 0;
        vs          = nullptr;
        decoder     = nullptr;
        available   = loadDll();
    }

    virtual ~VideoDecoderDXVAPrivate() // can not unload dlls because dx resource will be released in VideoDecoderD3DPrivate::close
    {
        unloadDll();
    }

    AVPixelFormat vaPixelFormat()                 const override
    {
        return QTAV_PIX_FMT_C(DXVA2_VLD);
    }

private:

    bool loadDll();
    bool unloadDll();
    bool createDevice()                                 override;

    // d3d device and it's resources, device manager, video device and decoder service

    void destroyDevice()                                override;
    QVector<GUID> getSupportedCodecs()            const override;
    bool checkDevice()                                  override;
    bool createDecoder(AVCodecID codec_id,
                       int w, int h,
                       QVector<va_surface_t*>& surf)    override;
    void destroyDecoder()                               override;
    bool setupSurfaceInterop()                          override;
    void* setupAVVAContext()                            override;
    int fourccFor(const GUID *guid) const               override;

    /* DLL */

    HINSTANCE                       hd3d9_dll;
    HINSTANCE                       hdxva2_dll;
    IDirect3D9*                     d3dobj;
    IDirect3DDevice9*               d3ddev;     ///< can be d3ddev-Ex

    /* Device manager */

    UINT                            token;
    IDirect3DDeviceManager9*        devmng;
    HANDLE                          device;

    /* Video service */

    IDirectXVideoDecoderService*    vs;

    /* Video decoder */

    DXVA2_ConfigPictureDecode       cfg;
    IDirectXVideoDecoder*           decoder;

    struct dxva_context             hw_ctx;
    QString                         vendor;

public:

    d3d9::InteropResourcePtr        interop_res; ///< may be still used in video frames when decoder is destroyed
};

static D3DFORMAT fourccToD3D(int fcc)
{
    return (D3DFORMAT)fcc;
}

VideoDecoderDXVA::VideoDecoderDXVA()
    : VideoDecoderD3D(*new VideoDecoderDXVAPrivate())
{
}

VideoDecoderId VideoDecoderDXVA::id() const
{
    return VideoDecoderId_DXVA;
}

QString VideoDecoderDXVA::description() const
{
    DPTR_D(const VideoDecoderDXVA);

    if (!d.description.isEmpty())
        return d.description;

    return QLatin1String("DirectX Video Acceleration");
}

VideoFrame VideoDecoderDXVA::frame()
{
    DPTR_D(VideoDecoderDXVA);

/*
    qCDebug(DIGIKAM_QTAV_LOG).noquote()
        << QString::asprintf("frame size: %dx%d",
            d.frame->width, d.frame->height);
*/
    if (!d.frame->opaque || !d.frame->data[0])
        return VideoFrame();

    if ((d.frame->width <= 0) || (d.frame->height <= 0) || !d.codec_ctx)
        return VideoFrame();

    IDirect3DSurface9* const d3d = (IDirect3DSurface9*)(uintptr_t)d.frame->data[3];

    if ((copyMode() == ZeroCopy) && d.interop_res)
    {
        d3d9::SurfaceInterop* const interop = new d3d9::SurfaceInterop(d.interop_res);
        interop->setSurface(d3d, d.width, d.height);
        VideoFrame f(d.width, d.height, VideoFormat::Format_RGB32);

        f.setBytesPerLine(d.width * 4);             // used by gl to compute texture size
        f.setMetaData(QLatin1String("surface_interop"), QVariant::fromValue(VideoSurfaceInteropPtr(interop)));

#ifndef HAVE_FFMPEG_VERSION5

        f.setTimestamp(d.frame->pkt_pts / 1000.0);

#else // ffmpeg >= 5

        f.setTimestamp(d.frame->pts / 1000.0);

#endif

        f.setDisplayAspectRatio(d.getDAR(d.frame));

        return f;
    }

    class Q_DECL_HIDDEN ScopedD3DLock
    {
        IDirect3DSurface9* mpD3D = nullptr;

    public:

        ScopedD3DLock(IDirect3DSurface9* d3d, D3DLOCKED_RECT* rect)
            : mpD3D(d3d)
        {
            if (mpD3D)
            {
                if (FAILED(mpD3D->LockRect(rect, nullptr, D3DLOCK_READONLY)))
                {
                    qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
                        << QString::asprintf("Failed to lock surface");

                    mpD3D = nullptr;
                }
            }
        }

        ~ScopedD3DLock()
        {
            if (mpD3D)
                mpD3D->UnlockRect();
        }
    };

    D3DLOCKED_RECT lock;
    ScopedD3DLock(d3d, &lock);           // cppcheck-suppress unusedScopedObject

    if (lock.Pitch == 0)
    {
        return VideoFrame();
    }

    // picth >= desc.Width

    D3DSURFACE_DESC desc;
    d3d->GetDesc(&desc);
    const VideoFormat fmt = VideoFormat(VideoDecoderD3D::pixelFormatFromFourcc(desc.Format));

    if (!fmt.isValid())
    {
        qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
            << QString::asprintf("unsupported dxva pixel format: %#x", desc.Format);

        return VideoFrame();
    }

    // YV12 need swap, not imc3?
    // imc3 U V pitch == Y pitch, but half of the U/V plane is space. we convert to yuv420p here
    // nv12 bpp(1)==1
    // 3rd plane is not used for nv12

    int pitch[3]        = { lock.Pitch,           0, 0 };   // compute chroma later
    uint8_t* src[]      = { (uint8_t*)lock.pBits, 0, 0 };   // compute chroma later
    const bool swap_uv  = (desc.Format == MAKEFOURCC('I','M','C','3'));

    return copyToFrame(fmt, desc.Height, src, pitch, swap_uv);
}

bool VideoDecoderDXVAPrivate::loadDll()
{
    hd3d9_dll = LoadLibrary(TEXT("D3D9.DLL"));

    if (!hd3d9_dll)
    {
        qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
            << QString::asprintf("cannot load d3d9.dll");

        return false;
    }

    hdxva2_dll = LoadLibrary(TEXT("DXVA2.DLL"));

    if (!hdxva2_dll)
    {
        qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
            << QString::asprintf("cannot load dxva2.dll");

        FreeLibrary(hd3d9_dll);

        return false;
    }

    return true;
}

bool VideoDecoderDXVAPrivate::unloadDll()
{
    if (hdxva2_dll)
        FreeLibrary(hdxva2_dll);

    if (hd3d9_dll)
        FreeLibrary(hd3d9_dll); // TODO: don't unload. maybe it's used by others

    return true;
}

bool VideoDecoderDXVAPrivate::createDevice()
{
    // check whether they are created?

    D3DADAPTER_IDENTIFIER9 d3dai;
    ZeroMemory(&d3dai, sizeof(d3dai));
    d3ddev = DXHelper::CreateDevice9Ex(hd3d9_dll, (IDirect3D9Ex**)(&d3dobj), &d3dai);

    if (!d3ddev)
    {
        qCWarning(DIGIKAM_QTAV_LOG_WARN)
            << "Failed to create d3d9 device ex, fallback to d3d9 device";

        d3ddev = DXHelper::CreateDevice9(hd3d9_dll, &d3dobj, &d3dai);
    }

    if (!d3ddev)        // cppcheck-suppress duplicateCondition
    {
        qCWarning(DIGIKAM_QTAV_LOG_WARN)
            << "Failed to create d3d9 device";

        return false;
    }

    vendor      = QString::fromLatin1(DXHelper::vendorName(d3dai.VendorId));
    description = QString().asprintf("DXVA2 (%.*s, vendor %lu(%s), device %lu, revision %lu)",
                                     sizeof(d3dai.Description),
                                     d3dai.Description,
                                     d3dai.VendorId,
                                     qPrintable(vendor),
                                     d3dai.DeviceId,
                                     d3dai.Revision);
/*
    if (copy_uswc)
        copy_uswc = vendor.toLower() == "intel";
*/

    qCDebug(DIGIKAM_QTAV_LOG).noquote()
        << QString::asprintf("DXVA2 description:  %s",
            description.toUtf8().constData());

    if (!d3ddev)
        return false;

    typedef HRESULT (WINAPI* CreateDeviceManager9Func)(UINT* pResetToken, IDirect3DDeviceManager9**);

    CreateDeviceManager9Func CreateDeviceManager9 = (CreateDeviceManager9Func)GetProcAddress(hdxva2_dll, "DXVA2CreateDirect3DDeviceManager9");

    if (!CreateDeviceManager9)
    {
        qCWarning(DIGIKAM_QTAV_LOG_WARN)
            << "cannot load function DXVA2CreateDirect3DDeviceManager9";

        return false;
    }

    qCDebug(DIGIKAM_QTAV_LOG)
        << "OurDirect3DCreateDeviceManager9 Success!";

    DX_ENSURE_OK(CreateDeviceManager9(&token, &devmng), false);

    qCDebug(DIGIKAM_QTAV_LOG)
        << "obtained IDirect3DDeviceManager9";

    // http://msdn.microsoft.com/en-us/library/windows/desktop/ms693525%28v=vs.85%29.aspx   // krazy:exclude=insecurenet

    DX_ENSURE_OK(devmng->ResetDevice(d3ddev, token), false);
    DX_ENSURE_OK(devmng->OpenDeviceHandle(&device), false);
    DX_ENSURE_OK(devmng->GetVideoService(device, IID_IDirectXVideoDecoderService, (void**)&vs), false);

    return true;
}

void VideoDecoderDXVAPrivate::destroyDevice()
{
    SafeRelease(&vs);

    if (devmng && device && (device != INVALID_HANDLE_VALUE))
    {
        devmng->CloseDeviceHandle(device);
        device = 0;
    }

    SafeRelease(&devmng);
    SafeRelease(&d3ddev);
    SafeRelease(&d3dobj);
}

QVector<GUID> VideoDecoderDXVAPrivate::getSupportedCodecs() const
{
    /* Retrieve supported modes from the decoder service */

    UINT input_count = 0;
    GUID* input_list = nullptr;
    QVector<GUID> guids;
    DX_ENSURE_OK(vs->GetDecoderDeviceGuids(&input_count, &input_list), guids);
    guids.resize(input_count);
    memcpy(guids.data(), input_list, input_count*sizeof(GUID));
    CoTaskMemFree(input_list);

    return guids;
}

int VideoDecoderDXVAPrivate::fourccFor(const GUID* guid) const
{
    UINT output_count      = 0;
    D3DFORMAT* output_list = nullptr;

    if (FAILED(vs->GetDecoderRenderTargets(*guid, &output_count, &output_list)))
    {
        qCWarning(DIGIKAM_QTAV_LOG_WARN)
            << "IDirectXVideoDecoderService_GetDecoderRenderTargets failed";

        return 0;
    }

    int fmt = VideoDecoderD3D::getSupportedFourcc((int*)output_list, output_count);
    CoTaskMemFree(output_list);

    return fmt;
}

bool VideoDecoderDXVAPrivate::checkDevice()
{
    // check pix fmt DXVA2_VLD and h264 profile?

    HRESULT hr = devmng->TestDevice(device);

    if      (hr == DXVA2_E_NEW_VIDEO_DEVICE)
    {
        // https://technet.microsoft.com/zh-cn/aa965266(v=vs.98).aspx
        // CloseDeviceHandle,Release service&decoder, open a new device handle, create a new decoder

        qCWarning(DIGIKAM_QTAV_LOG_WARN)
            << "DXVA2_E_NEW_VIDEO_DEVICE. Video decoder reset is not implemented";

        return false;
    }
    else if (FAILED(hr))
    {
        qCWarning(DIGIKAM_QTAV_LOG_WARN)
            << "IDirect3DDeviceManager9.TestDevice (" << hr << "): "
            << qt_error_string(hr);

        return false;
    }

    return true;
}

bool VideoDecoderDXVAPrivate::createDecoder(AVCodecID codec_id, int w, int h, QVector<va_surface_t*>& surf)
{
    if (!vs || !d3ddev)
    {
        qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
            << QString::asprintf("dx video surface is not ready. IDirectXVideoService: %p, IDirect3DDevice9: %p",
                vs, d3ddev);

        return false;
    }

    const int nb_surfaces                             = surf.size();
    static const int kMaxSurfaceCount                 = 64;
    IDirect3DSurface9* surface_list[kMaxSurfaceCount] = { nullptr };

    qCDebug(DIGIKAM_QTAV_LOG).noquote()
        << QString::asprintf("IDirectXVideoDecoderService=%p nb_surfaces=%d surface %dx%d",
                             vs, nb_surfaces, aligned(w), aligned(h));

    DX_ENSURE_OK(vs->CreateSurface(aligned(w),
                                   aligned(h),
                                   nb_surfaces - 1, // The number of back buffers. The method creates BackBuffers + 1 surfaces.
                                   fourccToD3D(format_fcc),
                                   D3DPOOL_DEFAULT,
                                   0,
                                   DXVA2_VideoDecoderRenderTarget,
                                   surface_list,
                                   nullptr),
                 false);

    for (int i = 0 ; i < nb_surfaces ; ++i)
    {
        d3d9_surface_t* const s = new d3d9_surface_t();
        s->setSurface(surface_list[i]);
        surf[i]                 = s;
    }

    qCDebug(DIGIKAM_QTAV_LOG).noquote()
        << QString::asprintf("IDirectXVideoAccelerationService_CreateSurface succeed with %d surfaces (%dx%d)",
            nb_surfaces, w, h);

    /* */

    DXVA2_VideoDesc dsc;
    ZeroMemory(&dsc, sizeof(dsc));
    dsc.SampleWidth                     = w;                // coded_width
    dsc.SampleHeight                    = h;                // coded_height
    dsc.Format                          = fourccToD3D(format_fcc);
    dsc.InputSampleFreq.Numerator       = 0;
    dsc.InputSampleFreq.Denominator     = 0;
    dsc.OutputFrameFreq                 = dsc.InputSampleFreq;
    dsc.UABProtectionLevel              = FALSE;
    dsc.Reserved                        = 0;

    // see xbmc

    /* FIXME I am unsure we can let unknown everywhere */

    DXVA2_ExtendedFormat* const ext     = &dsc.SampleFormat;
    ext->SampleFormat                   = 0;                // DXVA2_SampleProgressiveFrame;//xbmc. DXVA2_SampleUnknown;
    ext->VideoChromaSubsampling         = 0;                // DXVA2_VideoChromaSubsampling_Unknown;
    ext->NominalRange                   = 0;                // DXVA2_NominalRange_Unknown;
    ext->VideoTransferMatrix            = 0;                // DXVA2_VideoTransferMatrix_Unknown;
    ext->VideoLighting                  = 0;                // DXVA2_VideoLighting_dim;//xbmc. DXVA2_VideoLighting_Unknown;
    ext->VideoPrimaries                 = 0;                // DXVA2_VideoPrimaries_Unknown;
    ext->VideoTransferFunction          = 0;                // DXVA2_VideoTransFunc_Unknown;

    /* List all configurations available for the decoder */

    UINT cfg_count                      = 0;
    DXVA2_ConfigPictureDecode* cfg_list = nullptr;

    DX_ENSURE_OK(vs->GetDecoderConfigurations(codec_guid,
                                              &dsc,
                                              nullptr,
                                              &cfg_count,
                                              &cfg_list),
                 false);

    const int score = SelectConfig(codec_id, cfg_list, cfg_count, &cfg);
    CoTaskMemFree(cfg_list);

    if (score <= 0)
        return false;

    /* Create the decoder */

    DX_ENSURE_OK(vs->CreateVideoDecoder(codec_guid, &dsc, &cfg, surface_list, nb_surfaces, &decoder), false);

    qCDebug(DIGIKAM_QTAV_LOG).noquote()
        << QString::asprintf("IDirectXVideoDecoderService.CreateVideoDecoder succeed. decoder=%p",
            decoder);

    return true;
}

void VideoDecoderDXVAPrivate::destroyDecoder()
{
    SafeRelease(&decoder);
}

void* VideoDecoderDXVAPrivate::setupAVVAContext()
{
    // TODO: FF_DXVA2_WORKAROUND_SCALING_LIST_ZIGZAG

    if (VideoDecoderD3D::isIntelClearVideo(&codec_guid))
    {

#ifdef FF_DXVA2_WORKAROUND_INTEL_CLEARVIDEO // 2014-03-07 - 8b2a130 - lavc 55.50.0 / 55.53.100 - dxva2.h

        qCDebug(DIGIKAM_QTAV_LOG) << "FF_DXVA2_WORKAROUND_INTEL_CLEARVIDEO";

        hw_ctx.workaround |= FF_DXVA2_WORKAROUND_INTEL_CLEARVIDEO;

#endif

    }
    else
    {
        hw_ctx.workaround = 0;
    }

    hw_ctx.decoder       = decoder;
    hw_ctx.cfg           = &cfg;
    hw_ctx.surface_count = hw_surfaces.size();
    hw_ctx.surface       = (IDirect3DSurface9**)hw_surfaces.constData();

    return &hw_ctx;
}

bool VideoDecoderDXVAPrivate::setupSurfaceInterop()
{
    if (copy_mode == VideoDecoderFFmpegHW::ZeroCopy)
        interop_res = d3d9::InteropResourcePtr(d3d9::InteropResource::create(d3ddev));

    return true;
}

} // namespace QtAV
