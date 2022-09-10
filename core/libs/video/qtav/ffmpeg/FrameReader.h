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

#ifndef QTAV_FRAME_READER_H
#define QTAV_FRAME_READER_H

// Qt includes

#include <QObject>

// Local includes

#include "VideoFrame.h"

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
class DIGIKAM_EXPORT FrameReader : public QObject
{
    Q_OBJECT

public:

    // TODO: load and get info
    // TODO: asnyc option

    explicit FrameReader(QObject* const parent = nullptr);
    ~FrameReader();

    void setMedia(const QString& url);
    QString mediaUrl()          const;
    void setVideoDecoders(const QStringList& names);
    QStringList videoDecoders() const;
    VideoFrame getVideoFrame();
    bool hasVideoFrame()        const;
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
