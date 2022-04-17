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

#ifndef QTAV_FRAME_READER_H
#define QTAV_FRAME_READER_H

#include <QObject>
#include <QtAV/VideoFrame.h>

namespace QtAV
{
/*!
 * \brief The FrameReader class
 * while (reader->readMore()) {
 *      while (reader->hasVideoFrame()) { //or hasEnoughVideoFrames()
 *          reader->getVideoFrame();
 *          ...
 *      }
 * }
 * or (faster)
 * while (reader->readMore()) {
 *     reader->getVideoFrame(); //we can ensure 1 frame is available, but may block here
 * }
 * while (r.hasVideoFrame()) { //get buffered frames
 *     reader->getVideoFrame();
 * }
 * TODO: multiple tracks
 */
class Q_AV_EXPORT FrameReader : public QObject
{
    Q_OBJECT

public:

    // TODO: load and get info
    // TODO: asnyc option
    explicit FrameReader(QObject *parent = 0);
    ~FrameReader();

    void setMedia(const QString& url);
    QString mediaUrl() const;
    void setVideoDecoders(const QStringList& names);
    QStringList videoDecoders() const;
    VideoFrame getVideoFrame();
    bool hasVideoFrame() const;
    bool hasEnoughVideoFrames() const;
    // return false if eof
    bool readMore();
    // TODO: tryLoad on seek even at eof
    // TODO: compress seek requests
    bool seek(qint64 pos);

Q_SIGNALS:

    void frameRead(const QtAV::VideoFrame& frame);
    void readEnd();
    void seekFinished(qint64 pos);

    // internal
    void readMoreRequested();
    void seekRequested(qint64);

private Q_SLOTS:

    void readMoreInternal();
    bool seekInternal(qint64 value);

private:

    class Private;
    QScopedPointer<Private> d;
};

} // namespace QtAV

#endif // QTAV_FRAME_READER_H
