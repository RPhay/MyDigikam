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

#ifndef QTAV_VIDEO_DECODER_H
#define QTAV_VIDEO_DECODER_H

// Qt includes

#include <QStringList>

// Local includes

#include "AVDecoder.h"
#include "VideoFrame.h"

namespace QtAV
{

typedef int VideoDecoderId;

/*!
    Useful properties.
    A key is a string, a value can be int, bool or string. Both int and string are valid for enumerate
    properties. Flag properties must use int if more than 1 value is used.
    e.g. decoder->setProperty("display", 1) equals decoder->setProperty("display", "GLX")
    setOptions() also applies the properties.
    avcodec (also for VA-API, DXVA, VDA)
      Use AVCodecContext options
    CUDA
      surfaces: 0 is auto
      deinterlace: 0 "Weave", 1 "Bob", 2 "Adaptive"
    VA-API
      display: 0 "X11", 1 "GLX", 2 "DRM"
    DXVA, VA-API
      surfaces: 0 default
    DXVA, VA-API, VDA:
      sse4: bool
    FFmpeg
      skip_loop_filter, skip_idct, skip_frame: -16 "None", 0: "Default", 8 "NoRef", 16 "Bidir", 32 "NoKey", 64 "All"
      threads: int, 0 is auto
      vismv(motion vector visualization): flag, 0 "NO", 1 "PF", 2 "BF", 4 "BB"
 */

class VideoDecoderPrivate;

class DIGIKAM_EXPORT VideoDecoder : public AVDecoder
{
    Q_OBJECT
    Q_DISABLE_COPY(VideoDecoder)
    DPTR_DECLARE_PRIVATE(VideoDecoder)

public:

    static QStringList supportedCodecs();
    static VideoDecoder* create(VideoDecoderId id);

    /*!
     * \brief create
     * create a decoder from registered name. FFmpeg decoder will be created for empty name
     * \param name can be "FFmpeg", "CUDA", "VDA", "VAAPI", "DXVA"
     * \return 0 if not registered
     */
    static VideoDecoder* create(const char* name = "FFmpeg");
    virtual VideoDecoderId id() const = 0;
    QString name() const override;                   // name from factory
    virtual VideoFrame frame()        = 0;

public:

    typedef int Id;
    static QVector<VideoDecoderId> registered();

    template<class C> static bool Register(VideoDecoderId id, const char* name)
    {
        return Register(id, create<C>, name);
    }

    /*!
     * \brief next
     * \param id nullptr to get the first id address
     * \return address of id or nullptr if not found/end
     */
    static VideoDecoderId* next(VideoDecoderId* id = nullptr);
    static const char* name(VideoDecoderId id);
    static VideoDecoderId id(const char* name);

private:

    template<class C>
    static VideoDecoder* create()
    {
        return new C();
    }

    typedef VideoDecoder* (*VideoDecoderCreator)();

protected:

    static bool Register(VideoDecoderId id, VideoDecoderCreator, const char* name);

protected:

    VideoDecoder(VideoDecoderPrivate& d);

private:

    // Disable

    VideoDecoder();
    VideoDecoder(QObject*);
};

extern DIGIKAM_EXPORT VideoDecoderId VideoDecoderId_FFmpeg;
extern DIGIKAM_EXPORT VideoDecoderId VideoDecoderId_CUDA;
extern DIGIKAM_EXPORT VideoDecoderId VideoDecoderId_DXVA;
extern DIGIKAM_EXPORT VideoDecoderId VideoDecoderId_D3D11;
extern DIGIKAM_EXPORT VideoDecoderId VideoDecoderId_VAAPI;
extern DIGIKAM_EXPORT VideoDecoderId VideoDecoderId_VDA;
extern DIGIKAM_EXPORT VideoDecoderId VideoDecoderId_VideoToolbox;
extern DIGIKAM_EXPORT VideoDecoderId VideoDecoderId_MediaCodec;
extern DIGIKAM_EXPORT VideoDecoderId VideoDecoderId_MMAL;
extern DIGIKAM_EXPORT VideoDecoderId VideoDecoderId_QSV;
extern DIGIKAM_EXPORT VideoDecoderId VideoDecoderId_CrystalHD;

} // namespace QtAV

#endif // QTAV_VIDEO_DECODER_H
