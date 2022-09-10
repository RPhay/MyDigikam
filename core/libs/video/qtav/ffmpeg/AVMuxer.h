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

#ifndef QTAV_AV_MUXER_H
#define QTAV_AV_MUXER_H

// Qt includes

#include <QObject>
#include <QIODevice>
#include <QScopedPointer>
#include <QStringList>
#include <QVariant>

// Local includes

#include "AVError.h"
#include "Packet.h"

namespace QtAV
{

class MediaIO;
class VideoEncoder;
class AudioEncoder;

class DIGIKAM_EXPORT AVMuxer : public QObject
{
    Q_OBJECT

public:

    static const QStringList& supportedFormats();
    static const QStringList& supportedExtensions();

    /// Supported ffmpeg/libav input protocols(not complete). A static string list

    static const QStringList& supportedProtocols();

    explicit AVMuxer(QObject* const parent = nullptr);
    ~AVMuxer();

    QString fileName()      const;
    QIODevice* ioDevice()   const;

    /// not null for QIODevice, custom protocols

    MediaIO* mediaIO()      const;

    /*!
     * \brief setMedia
     * \return whether the media is changed
     */
    bool setMedia(const QString& fileName);
    bool setMedia(QIODevice* dev);
    bool setMedia(MediaIO* io);

    /*!
     * \brief setFormat
     * Force the output format.
     * formatForced() is reset if media changed. So you have to call setFormat() for every media
     * you want to force the format.
     * Also useful for custom io
     */
    void setFormat(const QString& fmt);
    QString formatForced()  const;

    bool open();
    bool close();
    bool isOpen()           const;

    // TODO: copyAudioContext(void* avctx) for copy encoding without decoding

    void copyProperties(VideoEncoder* enc); // rename to setEncoder
    void copyProperties(AudioEncoder* enc);

    void setOptions(const QVariantHash& dict);
    QVariantHash options()  const;

public Q_SLOTS:

    // TODO: multiple streams. Packet.type,stream

    bool writeAudio(const QtAV::Packet& packet);
    bool writeVideo(const QtAV::Packet& packet);
/*
    void writeHeader();
    void writeTrailer();
*/
private:

    class Private;
    QScopedPointer<Private> d;
};

} // namespace QtAV

#endif // QTAV_AV_MUXER_H
