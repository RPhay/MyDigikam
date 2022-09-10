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

#ifndef QTAV_VIDEO_FRAME_EXTRACTOR_H
#define QTAV_VIDEO_FRAME_EXTRACTOR_H

// Qt includes

#include <QObject>

// Local includes

#include "VideoFrame.h"

// TODO: extract all streams

namespace QtAV
{

class VideoFrameExtractorPrivate;

class DIGIKAM_EXPORT VideoFrameExtractor : public QObject
{
    Q_OBJECT
    DPTR_DECLARE_PRIVATE(VideoFrameExtractor)
    Q_PROPERTY(QString source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(bool autoExtract READ autoExtract WRITE setAutoExtract NOTIFY autoExtractChanged)
    Q_PROPERTY(bool async READ async WRITE setAsync NOTIFY asyncChanged)
    Q_PROPERTY(int precision READ precision WRITE setPrecision NOTIFY precisionChanged)
    Q_PROPERTY(qint64 position READ position WRITE setPosition NOTIFY positionChanged)

public:

    explicit VideoFrameExtractor(QObject* const parent = nullptr);

    /*!
     * \brief setSource
     * Set the video file. If video changes, current loaded video will be unloaded.
     */
    void setSource(const QString& url);
    QString source() const;

    /*!
     * \brief setAsync
     * Extract video frames in another thread. Default is true.
     * In async mode, if current extraction is not finished, new
     * setPosition() will be ignored.
     */
    void setAsync(bool value);
    bool async() const;

    void setAutoExtract(bool value);
    bool autoExtract() const;

    /*!
     * \brief setPrecision
     * if the difference between the next requested position is less than the value, previous
     * one is used and no positionChanged() and frameExtracted() signals to emit.
     * \param value < 0: auto. Real value depends on video duration and fps, but always 20 <= value <=500
     * Default is auto.
     */
    void setPrecision(int value);
    int precision() const;

    void setPosition(qint64 value);
    qint64 position() const;

Q_SIGNALS:

    void frameExtracted(const QtAV::VideoFrame& frame); // parameter: VideoFrame, bool changed?
    void sourceChanged();
    void asyncChanged();
    void error(const QString& errorMessage);    ///< emitted with a helpful error message -- connect to this to show empty image in preview widget
    void aborted(const QString& abortMessage);  ///< emitted when aborting the current preview -- if user requested a new preview this usually gets emitted.  connect to this to show empty preview
    void autoExtractChanged();

    /*!
     * \brief positionChanged
     * If not autoExtract, positionChanged() => extract() in a slot
     */
    void positionChanged();
    void precisionChanged();

public Q_SLOTS:

    /*!
     * \brief extract
     * If last extracted frame can be use, use it.
     * If there is a key frame in [position, position+precision], the nearest key frame
     * before position+precision will be extracted. Otherwise, the given position frame will be extracted.
     */
    void extract();

private Q_SLOTS:

    void extractInternal(qint64 pos);

protected:

    //VideoFrameExtractor(VideoFrameExtractorPrivate& d, QObject* parent = nullptr);

    DPTR_DECLARE(VideoFrameExtractor)
};

} // namespace QtAV

#endif // QTAV_VIDEO_FRAME_EXTRACTOR_H
