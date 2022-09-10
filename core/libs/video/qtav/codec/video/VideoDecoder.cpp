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

#include "VideoDecoder.h"

// Local includes

#include "AVDecoder_p.h"
#include "QtAV_factory.h"
#include "QtAV_mkid.h"
#include "digikam_debug.h"

namespace QtAV
{

FACTORY_DEFINE(VideoDecoder)

VideoDecoderId VideoDecoderId_FFmpeg        = mkid::id32base36_6<'F', 'F', 'm', 'p', 'e', 'g'>::value;
VideoDecoderId VideoDecoderId_CUDA          = mkid::id32base36_4<'C', 'U', 'D', 'A'>::value;
VideoDecoderId VideoDecoderId_DXVA          = mkid::id32base36_4<'D', 'X', 'V', 'A'>::value;
VideoDecoderId VideoDecoderId_D3D11         = mkid::id32base36_5<'D', '3', 'D', '1', '1'>::value;
VideoDecoderId VideoDecoderId_VAAPI         = mkid::id32base36_5<'V', 'A', 'A', 'P', 'I'>::value;
VideoDecoderId VideoDecoderId_VDA           = mkid::id32base36_3<'V', 'D', 'A'>::value;
VideoDecoderId VideoDecoderId_VideoToolbox  = mkid::id32base36_5<'V', 'T', 'B', 'o', 'x'>::value;
VideoDecoderId VideoDecoderId_MediaCodec    = mkid::id32base36_4<'F', 'F', 'M', 'C'>::value;
VideoDecoderId VideoDecoderId_MMAL          = mkid::id32base36_6<'F', 'F', 'M', 'M', 'A', 'L'>::value;
VideoDecoderId VideoDecoderId_QSV           = mkid::id32base36_5<'F', 'F', 'Q', 'S', 'V'>::value;
VideoDecoderId VideoDecoderId_CrystalHD     = mkid::id32base36_5<'F', 'F', 'C', 'H', 'D'>::value;

static void VideoDecoder_RegisterAll()
{
    static bool called = false;

    if (called)
        return;

    called = true;

    // factory.h does not check whether an id is registered

    if (VideoDecoder::name(VideoDecoderId_FFmpeg)) // registered on load
        return;

    extern bool RegisterVideoDecoderFFmpeg_Man();
    RegisterVideoDecoderFFmpeg_Man();

#if QTAV_HAVE(DXVA)

    extern bool RegisterVideoDecoderDXVA_Man();
    RegisterVideoDecoderDXVA_Man();

#endif // QTAV_HAVE(DXVA)

#if QTAV_HAVE(D3D11VA)

    extern bool RegisterVideoDecoderD3D11_Man();
    RegisterVideoDecoderD3D11_Man();

#endif // QTAV_HAVE(DXVA)

#if QTAV_HAVE(CUDA)

    extern bool RegisterVideoDecoderCUDA_Man();
    RegisterVideoDecoderCUDA_Man();

#endif // QTAV_HAVE(CUDA)

#if QTAV_HAVE(VAAPI)

    extern bool RegisterVideoDecoderVAAPI_Man();
    RegisterVideoDecoderVAAPI_Man();

#endif // QTAV_HAVE(VAAPI)

#if QTAV_HAVE(VIDEOTOOLBOX)

    extern bool RegisterVideoDecoderVideoToolbox_Man();
    RegisterVideoDecoderVideoToolbox_Man();

#endif // QTAV_HAVE(VIDEOTOOLBOX)

#if QTAV_HAVE(VDA)

    extern bool RegisterVideoDecoderVDA_Man();
    RegisterVideoDecoderVDA_Man();

#endif // QTAV_HAVE(VDA)

}

// TODO: called in ::create()/next() etc. to ensure registered?

QVector<VideoDecoderId> VideoDecoder::registered()
{
    VideoDecoder_RegisterAll();

#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)

    const auto ids = VideoDecoderFactory::Instance().registeredIds();

    return { ids.begin(), ids.end() };

#else

    return QVector<VideoDecoderId>::fromStdVector(VideoDecoderFactory::Instance().registeredIds());

#endif

}

QStringList VideoDecoder::supportedCodecs()
{
    static QStringList codecs;

    if (!codecs.isEmpty())
        return codecs;

    const AVCodec* c = nullptr;

#if AVCODEC_STATIC_REGISTER

    void* it         = nullptr;

    while ((c = av_codec_iterate(&it)))
    {

#else

    avcodec_register_all();

    while ((c = av_codec_next(c)))
    {

#endif
        if (!av_codec_is_decoder(c) || (c->type != AVMEDIA_TYPE_VIDEO))
            continue;

        codecs.append(QString::fromLatin1(c->name));
    }

    return codecs;
}

VideoDecoder::VideoDecoder(VideoDecoderPrivate& d):
    AVDecoder(d)
{
}

QString VideoDecoder::name() const
{
    return QLatin1String(VideoDecoder::name(id()));
}

} // namespace QtAV
