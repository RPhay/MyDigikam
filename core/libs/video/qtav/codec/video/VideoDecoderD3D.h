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

#ifndef QTAV_VIDEO_DECODER_D3D_H
#define QTAV_VIDEO_DECODER_D3D_H

// C++ includes

#include <unknwn.h>
#include <inttypes.h>

// Local includes

#include "VideoDecoderFFmpegHW.h"
#include "VideoDecoderFFmpegHW_p.h"
#include "QtAV_Global.h"
#include "digikam_debug.h"

namespace QtAV
{

struct d3d_format_t
{
    const char*              name   = nullptr;
    int                      fourcc = 0;
    VideoFormat::PixelFormat pixfmt = VideoFormat::Format_Invalid;
};

class VideoDecoderD3DPrivate;

class VideoDecoderD3D : public VideoDecoderFFmpegHW
{
    DPTR_DECLARE_PRIVATE(VideoDecoderD3D)
    Q_OBJECT
    Q_PROPERTY(int surfaces READ surfaces WRITE setSurfaces NOTIFY surfacesChanged)

public:

    // properties

    void setSurfaces(int num);
    int surfaces() const;

    static bool isIntelClearVideo(const GUID* guid);
    static bool isNoEncrypt(const GUID* guid);
    static int getSupportedFourcc(int* formats, UINT nb_formats);
    static VideoFormat::PixelFormat pixelFormatFromFourcc(int format);

Q_SIGNALS:

    void surfacesChanged();

protected:

    explicit VideoDecoderD3D(VideoDecoderD3DPrivate& d);
};

struct va_surface_t
{
    va_surface_t()
        : ref   (0),
          order (0)
    {
    }

    virtual ~va_surface_t()
    {
    }

    virtual void setSurface(IUnknown* s) = 0;
    virtual IUnknown* getSurface() const = 0;

    int ref;
    int order;
};

class VideoDecoderD3DPrivate : public VideoDecoderFFmpegHWPrivate
{
public:

    VideoDecoderD3DPrivate();

    bool  open()                                     override;
    void  close()                                    override;
    void* setup(AVCodecContext* avctx)               override;
    bool  getBuffer(void** opaque, uint8_t** data)   override;
    void  releaseBuffer(void* opaque, uint8_t* data) override;

    int aligned(int x);

private:

    virtual bool setupSurfaceInterop()
    {
        return true;
    }

    virtual bool createDevice()                      = 0;      // d3d device, video context etc.
    virtual void destroyDevice()                     = 0;

    virtual bool checkDevice()
    {
        return true;
    }

    virtual QVector<GUID> getSupportedCodecs() const = 0;

    virtual void* setupAVVAContext()                 = 0;

    /// create surfaces and decoder. width and height are coded value, maybe not aligned for d3d surface
    /// surfaces count is given, but not allocated

    virtual bool createDecoder(AVCodecID codec, int width, int height, QVector<va_surface_t*>& surf) = 0;
    virtual void destroyDecoder()                                                                    = 0;
    virtual int fourccFor(const GUID* guid) const                                                    = 0;
    const d3d_format_t* getFormat(const AVCodecContext* avctx, const QVector<GUID>& guids, GUID* selected) const;

public:

    // set by user. don't reset in when call destroy

    bool                    surface_auto;
    int                     surface_count;
    QVector<IUnknown*>      hw_surfaces;
    int                     format_fcc;
    GUID                    codec_guid;

private:

    int                     surface_width;
    int                     surface_height;
    unsigned                surface_order;
    QVector<va_surface_t*>  surfaces;           // TODO: delete on close()
};

template<typename T>
int SelectConfig(AVCodecID codec_id, const T* cfgs, int nb_cfgs, T* cfg)
{
    qCDebug(DIGIKAM_QTAV_LOG).noquote()
        << QString::asprintf("Decoder configurations: %d", nb_cfgs);

    /* Select the best decoder configuration */

    int cfg_score = 0;

    for (int i = 0 ; i < nb_cfgs ; ++i)
    {
        const T& c = cfgs[i];

        qCDebug(DIGIKAM_QTAV_LOG).noquote()
            << QString::asprintf("configuration[%d] ConfigBitstreamRaw %d",
                i, c.ConfigBitstreamRaw);

        /* */

        int score = 0;

        if      (c.ConfigBitstreamRaw == 1)
            score = 1;
        else if ((codec_id == QTAV_CODEC_ID(H264)) && (c.ConfigBitstreamRaw == 2))
            score = 2;
        else
            continue;

        if (VideoDecoderD3D::isNoEncrypt(&c.guidConfigBitstreamEncryption))
            score += 16;

        if (cfg_score < score)
        {
            *cfg      = c;
            cfg_score = score;
        }
    }

    if (cfg_score <= 0)
    {
        qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
            << QString::asprintf("Failed to find a supported decoder configuration");
    }

    return cfg_score;
}

#ifndef MAKEFOURCC // winrt

#   define MAKEFOURCC(ch0, ch1, ch2, ch3) \
  ((DWORD)(BYTE)(ch0)|((DWORD)(BYTE)(ch1) << 8) | ((DWORD)(BYTE)(ch2) << 16) | ((DWORD)(BYTE)(ch3) << 24))

#endif

} // namespace QtAV

#endif // QTAV_VIDEO_DECODER_D3D_H
