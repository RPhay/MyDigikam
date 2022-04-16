/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * Copyright (C) 2012-2022 Wang Bin <wbsecg1 at gmail dot com>
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

#ifndef QAV_AVMUXER_H
#define QAV_AVMUXER_H

#include <QtAV/AVError.h>
#include <QtAV/Packet.h>
#include <QtCore/QObject>
#include <QtCore/QIODevice>
#include <QtCore/QScopedPointer>
#include <QtCore/QStringList>
#include <QtCore/QVariant>

namespace QtAV
{

class MediaIO;
class VideoEncoder;
class AudioEncoder;
class Q_AV_EXPORT AVMuxer : public QObject
{
    Q_OBJECT
public:
    static const QStringList& supportedFormats();
    static const QStringList& supportedExtensions();
    /// Supported ffmpeg/libav input protocols(not complete). A static string list
    static const QStringList& supportedProtocols();

    AVMuxer(QObject *parent = 0);
    ~AVMuxer();
    QString fileName() const;
    QIODevice* ioDevice() const;
    /// not null for QIODevice, custom protocols
    MediaIO *mediaIO() const;
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
    QString formatForced() const;

    bool open();
    bool close();
    bool isOpen() const;

    // TODO: copyAudioContext(void* avctx) for copy encoding without decoding
    void copyProperties(VideoEncoder* enc); //rename to setEncoder
    void copyProperties(AudioEncoder* enc);

    void setOptions(const QVariantHash &dict);
    QVariantHash options() const;

public Q_SLOTS:
    // TODO: multiple streams. Packet.type,stream
    bool writeAudio(const QtAV::Packet& packet);
    bool writeVideo(const QtAV::Packet& packet);

    //void writeHeader();
    //void writeTrailer();
private:
    class Private;
    QScopedPointer<Private> d;
};
} // namespace QtAV
#endif //QAV_AVMUXER_H
