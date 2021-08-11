/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2017-05-25
 * Description : a tool to generate video slideshow from images.
 *
 * Copyright (C) 2017-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_VIDSLIDE_SETTINGS_H
#define DIGIKAM_VIDSLIDE_SETTINGS_H

// Qt includes

#include <QString>
#include <QList>
#include <QUrl>
#include <QSize>
#include <QMap>

// Local includes

#include "filesaveconflictbox.h"
#include "transitionmngr.h"
#include "effectmngr.h"
#include "digikam_export.h"

class KConfigGroup;

namespace Digikam
{

class DIGIKAM_EXPORT VidSlideSettings
{
public:

    /**
     * Images selection mode
     */
    enum Selection
    {
        IMAGES = 0,
        ALBUMS
    };

    /**
     * Video player to use
     */
    enum VidPlayer
    {
        NOPLAYER = 0,
        INTERNAL,
        DESKTOP
    };

    /**
     * Video Codecs
     */
    enum VidCodec
    {
        X264 = 0,    ///< https://en.wikipedia.org/wiki/X264
        MPEG4,       ///< https://en.wikipedia.org/wiki/MPEG-4
        MPEG2,       ///< https://en.wikipedia.org/wiki/MPEG-2
        MJPEG,       ///< https://en.wikipedia.org/wiki/Motion_JPEG
        FLASH,       ///< https://en.wikipedia.org/wiki/Adobe_Flash
        WEBMVP8,     ///< https://en.wikipedia.org/wiki/VP8
        THEORA,      ///< https://en.wikipedia.org/wiki/Theora
        WMV7,        ///< https://en.wikipedia.org/wiki/Windows_Media_Video
        WMV8,        ///< https://en.wikipedia.org/wiki/Windows_Media_Video
        WMV9         ///< https://en.wikipedia.org/wiki/Windows_Media_Video
    };

    /**
     * Video Standards
     */
    enum VidStd
    {
        PAL = 0,    ///< 25 FPS
        NTSC        ///< 29.97 FPS
    };

    /**
     * Video types (size of target screen)
     * See https://en.wikipedia.org/wiki/List_of_common_resolutions#Digital_TV_standards
     *     https://en.wikipedia.org/wiki/Aspect_ratio_(image)
     */
    enum VidType
    {
        QVGA = 0,   ///< 320   x 180  - 16:9
        VCD1,       ///< 352   x 240  - 7:5     - Pure Video
        VCD2,       ///< 352   x 288  - 6:5     - Pure Video
        CVD1,       ///< 352   x 480  - 4:3     - Pure Video
        CVD2,       ///< 352   x 576  - 16:9    - Pure Video
        HVGA,       ///< 480   x 270  - 16:9
        SVCD1,      ///< 480   x 480  - 1:1     - Pure Video
        SVCD2,      ///< 480   x 576  - 5:6     - Pure Video
        EGA,        ///< 640   x 350  - 16:9
        VGA,        ///< 640   x 480  - 4:3
        DVD1,       ///< 720   x 480  - 3:2     - Pure Video
        DVD2,       ///< 720   x 576  - 5:4     - Pure Video
        WVGA,       ///< 800   x 450  - 16:9
        SVGA,       ///< 800   x 600  - 4:3
        DVGA,       ///< 960   x 640  - 3:2
        XVGA,       ///< 1024  x 576  - 16:9
        HDTV,       ///< 1280  x 720  - 16:9    - Pure Video
        WXGA1,      ///< 1280  x 768  - 5:3
        WXGA2,      ///< 1280  x 800  - 8:5
        SXGA,       ///< 1280  x 1024 - 5:4
        SXGAPLUS,   ///< 1400  x 1050 - 4:3
        WSXGA,      ///< 1440  x 900  - 8:5
        HDPLUS,     ///< 1600  x 900  - 16:9    - Pure Video
        UXGA,       ///< 1600  x 1200  - 4:3
        WSXGAPLUS,  ///< 1680  x 1050  - 8:5
        BLUERAY,    ///< 1920  x 1080 - 19:9    - Pure Video
        WUXGA,      ///< 1920  x 1200 - 8:5
        TXGA,       ///< 1920  x 1440 - 7:5
        QXGA,       ///< 2048  x 1536 - 4:3
        UWFHD,      ///< 2560  < 1080 - 21:9
        WQHD,       ///< 2560  x 1440 - 16:9
        WQXGA,      ///< 2560  x 1600 - 8:5
        QSXGA,      ///< 2560  x 2048 - 5:4
        QSXGAPLUS,  ///< 2800  x 2100 - 4:3
        WQXGAPLUS,  ///< 3200  x 1800 - 16:9
        WQSXGA,     ///< 3200  x 2048 - 25:16
        QUXGA,      ///< 3200  x 2400 - 4:3
        UHD4K,      ///< 3840  x 2160 - 19:9    - Pure Video
        WQUXGA,     ///< 3840  x 2400 - 8:5
        HXGA,       ///< 4096  x 3072 - 4:3
        UHD5K,      ///< 5120  x 2880 - 16:9
        WHXGA,      ///< 5120  x 3200 - 8:5
        HSXGA,      ///< 5120  x 4096 - 5:4
        UHD6K,      ///< 6016  x 3384 - 16:9
        WHSXGA,     ///< 6400  x 4096 - 25:16
        HUXGA,      ///< 6400  x 4800 - 4:3
        UHD8K,      ///< 7680  x 4320 - 16:9    - Pure Video
        WHUXGA,     ///< 7680  x 4800 - 8:5
        UW10K,      ///< 10240 x 4320 - 21:9
        UW16K       ///< 15360 x 8640 - 16:9
    };

    /**
     * Video rates in bits per seconds.
     */
    enum VidBitRate
    {
        VBR04 = 0,  ///< 400000
        VBR05,      ///< 500000
        VBR10,      ///< 1000000
        VBR12,      ///< 1200000
        VBR15,      ///< 1500000
        VBR20,      ///< 2000000
        VBR25,      ///< 2500000
        VBR30,      ///< 3000000
        VBR40,      ///< 4000000
        VBR45,      ///< 4500000
        VBR50,      ///< 5000000
        VBR60,      ///< 6000000
        VBR80       ///< 8000000
    };

    /**
     * Video Container Formats
     */
    enum VidFormat
    {
        AVI = 0,    ///< https://en.wikipedia.org/wiki/Audio_Video_Interleave
        MKV,        ///< https://en.wikipedia.org/wiki/Matroska
        MP4,        ///< https://en.wikipedia.org/wiki/MPEG-4_Part_14
        MPG         ///< https://en.wikipedia.org/wiki/MPEG-2
    };

public:

    explicit VidSlideSettings();
    ~VidSlideSettings();

    /**
     * Read and write settings in config file between sessions.
     */
    void  readSettings(KConfigGroup& group);
    void  writeSettings(KConfigGroup& group);

    QSize   videoSize()             const;              ///< Return the current video size.
    int     videoBitRate()          const;              ///< Return the current video bit rate.
    qreal   videoFrameRate()        const;              ///< Return the current video frame rate.
    QString videoCodec()            const;              ///< Return the current video ffmpeg codec name.
    QString videoFormat()           const;              ///< Return the current video format extension.

    /**
     * Helper methods to fill combobox from GUI.
     */
    static QMap<VidType,    QString> videoTypeNames();
    static QMap<VidBitRate, QString> videoBitRateNames();
    static QMap<VidStd,     QString> videoStdNames();
    static QMap<VidCodec,   QString> videoCodecNames();
    static QMap<VidFormat,  QString> videoFormatNames();
    static QMap<VidPlayer,  QString> videoPlayerNames();

    static QSize videoSizeFromType(VidType type);           ///< Return the current size from a type of video.
    static bool isPureVideoFormat(VidType type);            ///< Return true if type is a pure video format. If false is returned type is screen format.

public:

    Selection                         selMode;          ///< Items selection mode

    // -- Generator settings ------

    QList<QUrl>                       inputImages;      ///< Images stream.
    QList<QUrl>                       inputAudio;       ///< Soundtracks stream.

    TransitionMngr::TransType         transition;       ///< Transition type between images.

    int                               imgFrames;        ///< Amount of frames by image to encode in video (ex: 125 frames = 5 s at 25 img/s).

    int                               abitRate;         ///< Encoded Audio stream bit rate in bit/s.
    VidBitRate                        vbitRate;         ///< Encoded Video stream bit rate in bit/s.
    VidStd                            vStandard;        ///< Encoded Video standard => frame rate in img/s.
    VidType                           vType;            ///< Encoded video type.
    VidCodec                          vCodec;           ///< Encoded video codec.
    VidFormat                         vFormat;          ///< Encoded video container format.
    EffectMngr::EffectType            vEffect;          ///< Encoded video effect while displaying images.

    FileSaveConflictBox::ConflictRule conflictRule;     ///< Rule to follow if video file already exists.
    QUrl                              outputDir;        ///< Encoded video stream directory.

    // ---------------------

    QString                           outputVideo;      ///< Target video file encoded at end.
    VidPlayer                         outputPlayer;     ///< Open video stream in player at end.
};

} // namespace Digikam

#endif // DIGIKAM_VIDSLIDE_SETTINGS_H
